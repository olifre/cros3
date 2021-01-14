/*
$Id: cros3.c,v 1.21 2008/05/24 03:30:03 hannappe Exp $
Linux driver for CROS3 chamber readout PCI board.
 */
#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/delay.h>

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/fcntl.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include <linux/version.h>
#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif

#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <asm/bitops.h>
#include <asm/segment.h>
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,37))
#include <asm/system.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
  #include <linux/semaphore.h>
#else
  #include <asm/semaphore.h>
#endif


#define DEBUG

#include "cros3.h"

#define PCI_VENDOR_ID_PNPI 0x1999
#define PCI_DEVICE_ID_CROS3 0x680C

#define CROS3_MAXDEV 4

struct cros3_device {
  struct pci_dev *pcidev;
  int in_use;				/* flag to avoid multiple use */
  int irq;                            /* irq line number */
  wait_queue_head_t irqwait;   	/* needed to wait for irq */
  atomic_t irqs_since_read;
  unsigned long total_irqs;
  struct cros3_interrupt_data_type irqdata[CROS3_MAX_IRQTIMES];	/* data to record irq activity */
  unsigned int write_index;
  unsigned int read_index;
  unsigned long base_addr;            /* base address of memory image */
  struct cdev cdev;
  char *registers; 			/* register image of board */
};


static struct cros3_device device[CROS3_MAXDEV] = {{0}};
static int nof_devices = 0;
static struct class *cros3_class = NULL;
static int cros3_major = 0;

static int
cros3_open(struct inode *inode, struct file *file)
{
    unsigned int minor = MINOR(inode->i_rdev);
    printk(KERN_INFO "cros3: attempt to open device with minor device %d\n", minor);
    if (minor < nof_devices) {
	if (device[minor].in_use) {
	    printk(KERN_WARNING "cros3: device already in use\n");
	    return -EBUSY;
	}
	device[minor].in_use = 1;

	file->private_data = &(device[minor]);
    } else { /* we do not have a minor device that high... */
	return -ENODEV;
    }
    return 0;
}

static int
cros3_release(struct inode *inode, struct file *file)
{

    unsigned int minor = MINOR(inode->i_rdev);
    if (minor < nof_devices) {
	if (!device[minor].in_use) {
	    printk(KERN_WARNING "cros3: trying to close non open minor device\n");
	}
	device[minor].in_use = 0;
	file->private_data = NULL;
    } else { /* we do not have a minor device that high... */
	return  -ENODEV;
    }

    
    return 0;
}

static int 
cros3_mmap(struct file *file,
	   struct vm_area_struct *vma)
{
    struct cros3_device *device;
    device = (struct cros3_device *) file->private_data;
    if (!device->in_use) {
	printk(KERN_WARNING "cros3: trying to mmap non open minor device\n");
    }
    if (vma->vm_end - vma->vm_start > 0x7FFF+1) {
	printk(KERN_NOTICE "cros3: mmap INVALID "
	       "start at 0x%08lx  end 0x%08lx\n",
	       vma->vm_start, vma->vm_end);
	return -EINVAL;
    }
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    if (remap_pfn_range(vma,
			vma->vm_start,
			device->base_addr >> PAGE_SHIFT,
			vma->vm_end - vma->vm_start,
			vma->vm_page_prot) < 0) {
	printk(KERN_WARNING "cros3: remap_pfn_range failed\n");
	return -EIO;
    }

 
    return 0;
}

static ssize_t 
cros3_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    struct cros3_device *device;
    ssize_t copied_bytes, bytes_to_copy;
    unsigned int irqs_since_read; 
    int end, missing;
    device = (struct cros3_device *) file->private_data;

    if (*ppos + count > sizeof(struct cros3_read_data_type)) {
	return -EINVAL;
    }

    irqs_since_read  = atomic_xchg(&device->irqs_since_read,0);
    bytes_to_copy =  sizeof(irqs_since_read);
    copied_bytes = bytes_to_copy -  copy_to_user(buf,(char *)&(irqs_since_read),
				 bytes_to_copy);

    end = device->read_index + irqs_since_read;
    missing = (end - CROS3_MAX_IRQTIMES);
    if (end>CROS3_MAX_IRQTIMES) end=CROS3_MAX_IRQTIMES;
    bytes_to_copy = sizeof(device->irqdata[0])*(end-device->read_index);
    copied_bytes += bytes_to_copy - copy_to_user(buf + copied_bytes,
						 ((char *)&(device->irqdata[device->read_index])),
						 bytes_to_copy);
    device->read_index = end;
    
/*    if (missing>0) {
      bytes_to_copy = sizeof(device->irqdata[0])*missing;
      copied_bytes += bytes_to_copy - copy_to_user(buf+ copied_bytes,
						   ((char *)&(device->irqdata[0])),
						   bytes_to_copy);
      device->read_index = missing;
    }*/
    *ppos += copied_bytes;
    return copied_bytes;
}



static unsigned int 
cros3_poll(struct file *file,struct poll_table_struct *poll_table)
{
    struct cros3_device *device;
    device = (struct cros3_device *) file->private_data;
    
    /* check if there are already data worth reading,
       if so, return immediately */
    if (atomic_read(&(device->irqs_since_read))) {
      return POLLIN | POLLRDNORM;
    }

    /* no old data, wait for next event */
    poll_wait(file, &(device->irqwait), poll_table);
    if (atomic_read(&(device->irqs_since_read))) {
      return POLLIN | POLLRDNORM;
    }
    return 0;
}


static const struct file_operations cros3_fops = {
	.owner		= THIS_MODULE,
	.mmap		= cros3_mmap,
	.open		= cros3_open,
	.release	= cros3_release,
	.read		= cros3_read,
	.poll           = cros3_poll,
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19))
static irqreturn_t 
cros3_interrupt(int irq, void *dev_id)
#else
static irqreturn_t 
cros3_interrupt(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
    struct cros3_device *device= (struct cros3_device *) dev_id;

    /* get fifo status, which contains the flag to check if an interrupt
       was triggered by the board first */
    unsigned long fifo_status;
    fifo_status = readl(device->registers + CROS3_CSB_CSR_STS);
    
    if (fifo_status & CSB_CSR_STS_BM_INTS) { /* interrupt is active */
    /* now clear the interrupt line */
      writel(0,device->registers + CROS3_CSB_ACT_INR); /* any write clears! */
	/*writel(0x00000808,device->registers + CROS3_CSB_CSR_CFG);*/
	//getnstimeofday(&(device->irqdata[device->write_index].ts_irq));
	device->irqdata[device->write_index].ts_irq = ktime_get_real();
	device->irqdata[device->write_index].csr_sts = fifo_status;
	device->irqdata[device->write_index].irq_number = ++(device->total_irqs);
	device->write_index = ((device->write_index + 1) & (CROS3_MAX_IRQTIMES-1));
        atomic_inc(&device->irqs_since_read);
	wake_up_interruptible(&(device->irqwait));
	/*writel(0x00000801,device->registers + CROS3_CSB_CSR_CFG);*/
	return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

static void
cros3_check_pattern(struct cros3_device *device, unsigned long pattern)
{
    unsigned long rvalue;
    writel(pattern,device->registers + CROS3_CSB_DAT_RW);
    rvalue = readl(device->registers + CROS3_CSB_DAT_RW);
    if (rvalue != pattern) {
	printk(KERN_INFO "cros3: wrote 0x%08lx, read 0x%08lx, xor is 0x%08lx\n",
	       pattern, rvalue, pattern ^ rvalue);
    }
}

static void
cros3_check_board(struct cros3_device *device)
{
    {
	unsigned long fid;
	char fidstring[9]={0};
	int i;
	fid = readl(device->registers + CROS3_CSB_CSR_FID);
	for (i=7; i>=0; i--) {
	    fidstring[i] = '0' + (fid & 0x0000000f);
	    fid >>= 4;
	}
	printk(KERN_INFO "cros3: found firmware version %s\n",fidstring);
    }
    {
	int i;
	printk(KERN_INFO "cros3: checkig read/write register\n");
	for (i=0; i<32; i++) {
	    cros3_check_pattern(device,0x00000001 << i);
	    cros3_check_pattern(device,~(0x00000001 << i));
	}
	cros3_check_pattern(device,0xAAAAAAAA);
	cros3_check_pattern(device,0x55555555);
	printk(KERN_INFO "cros3: done\n");
    }
}

static int __init cros3_init_module (void)
{
	dev_t devreg = 0;
	printk(KERN_INFO "cros3: driver startup\n");
	//printk(KERN_INFO "cros3: driver startup, compiled " __DATE__ " " __TIME__ "\n");
	printk(KERN_INFO "cros3: $Id: cros3.c,v 1.21 2008/05/24 03:30:03 hannappe Exp $\n");
	printk(KERN_INFO "cros3: " CROS3_HEADER_ID "\n");

	if (alloc_chrdev_region(&devreg, 0, CROS3_MAXDEV, "cros3") < 0) {
		printk (KERN_ERR "cros3: unable to allocate character device region\n");
		return -EIO;
	}
	cros3_major = MAJOR(devreg);

	cros3_class = class_create(THIS_MODULE, "cros3");
	if (IS_ERR(cros3_class)) {
		printk (KERN_ERR "cros3: unable to create device class\n");
		return -EIO;
	}

    {
		struct pci_dev *pdev = NULL;
		nof_devices=0;
		while ((pdev = pci_get_device (PCI_VENDOR_ID_PNPI, 
						PCI_DEVICE_ID_CROS3, 
						pdev))) {
			unsigned char pci_bus;
			unsigned char pci_device_fn;
			unsigned int pci_ioaddr;
			int err = 0;
			struct device *dev = NULL;
			dev_t devno = MKDEV(cros3_major, nof_devices);

			cdev_init(&device[nof_devices].cdev, &cros3_fops);
			err = cdev_add(&device[nof_devices].cdev, devno, 1);
			if (err) {
				printk(KERN_WARNING "cros3: Error %d while trying to add %s_%d",
					err, "cros3", nof_devices);
				continue;
			}

			dev = device_create(cros3_class, NULL, /* no parent device */ 
				devno, NULL, /* no additional data */
				"cros3-%d", nof_devices);
			if (IS_ERR(dev)) {
				err = PTR_ERR(dev);
				printk(KERN_WARNING "cros3: Error %d while trying to create %s-%d",
					err, "cros3", nof_devices);
				cdev_del(&device[nof_devices].cdev);
				continue;
			}

			pci_bus = pdev->bus->number;
			pci_device_fn = pdev->devfn;
			pci_ioaddr = pci_resource_start (pdev,0);
			printk(KERN_INFO "cros3: found at bus %x device %x\n",
			pci_bus, pci_device_fn);
			
			if (nof_devices+1 > CROS3_MAXDEV) {
				printk(KERN_WARNING "cros3: found %d devices, but driver is confugured for only %d, ignoring this one\n",
					nof_devices+1, CROS3_MAXDEV);
			} else {
				if (pci_enable_device(pdev))
					continue;
				device[nof_devices].pcidev = pdev;
				device[nof_devices].in_use = 0;
				device[nof_devices].base_addr = pci_resource_start (pdev,0);
				device[nof_devices].irq = pdev->irq;
				atomic_set(&(device[nof_devices].irqs_since_read),0);
				device[nof_devices].total_irqs = 0;
				device[nof_devices].write_index = 0;
				device[nof_devices].read_index = 0;
				request_irq(device[nof_devices].irq,cros3_interrupt,IRQF_SHARED,
						"cros3", &device[nof_devices]);
				device[nof_devices].registers = ioremap(device[nof_devices].base_addr,0x10000);
				init_waitqueue_head(&(device[nof_devices].irqwait));

				cros3_check_board(&device[nof_devices]);
			}
			nof_devices++;
		}
		printk(KERN_INFO "cros3: found %d devices\n", nof_devices);
		if (nof_devices == 0)
		{
			printk(KERN_ERR "cros3: No Cros3 chip found. giving up.\n");
			unregister_chrdev_region(MKDEV(cros3_major, 0), CROS3_MAXDEV);
			return -ENOSYS;
		}
	}
	return 0;
}
static void cros3_cleanup_module (void)
{
	int n;
	for (n=0; n<CROS3_MAXDEV; n++) {
		if (device[n].pcidev) { /* this device is found and configured */
			iounmap(device[n].registers);
			free_irq(device[n].irq,&device[n]);
			cdev_del(&device[nof_devices].cdev);
		}
	}
	if (cros3_class) {
		class_destroy(cros3_class);
	}
	unregister_chrdev_region(MKDEV(cros3_major, 0), CROS3_MAXDEV);
}

module_init(cros3_init_module);
module_exit(cros3_cleanup_module);
MODULE_LICENSE("GPL");
