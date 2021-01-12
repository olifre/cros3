#ifndef CROS3_INCLUDED
#define CROS3_INCLUDED
/*
CROS3 driftchamber readout PCI board register definitions
$Id: cros3.h,v 1.8 2008/04/03 17:27:55 hannappe Exp $
 */

#define CROS3_HEADER_REVISION "$Revision: 1.8 $"
#define CROS3_HEADER_ID "$Id: cros3.h,v 1.8 2008/04/03 17:27:55 hannappe Exp $"

struct cros3_csb_csr_fid_regbits {
    unsigned dd0:4;
    unsigned dd1:4;
    unsigned mm0:4;
    unsigned mm1:4;
    unsigned yy0:4;
    unsigned yy1:4;
    unsigned yy2:4;
    unsigned yy3:4;
};

struct cros3_csb_csr_cfg_regbits {
    unsigned trig:1;
    unsigned trsv:1;
    unsigned tbsy:1;
    unsigned fbsy:1;
    unsigned spare:4;
    unsigned opm:3;
    unsigned inta:1;
    unsigned rxer:1;
    unsigned rxdv:1;
    unsigned zero1:2;
    unsigned pec:8;
    unsigned lpen:1;
    unsigned tsen:1;
    unsigned lock:1;
    unsigned rxsd:1;
    unsigned txer:1;
    unsigned txen:1;
    unsigned zero2:1;
    unsigned dven:1;
};

struct cros3_csb_csr_sts_regbits {
    unsigned rwc:10;
    unsigned spare1:5;
    unsigned ints:1;
    unsigned dec:8;
    unsigned rfef:1;
    unsigned spare2:1;
    unsigned rfaf:1;
    unsigned rfff:1;
    unsigned strg:1;
    unsigned svet:1;
    unsigned sbsy:1;
    unsigned ibsy:1;
};

union cros3_csb_csr_fid_reg { /* (0x20) CSB Firmware Identifier	     */
    unsigned int data;
    struct cros3_csb_csr_fid_regbits bits;
};
union cros3_csb_csr_cfg_reg { /* (0x24) CSB Configuration / Status     */
    unsigned int data;
    struct cros3_csb_csr_cfg_regbits bits;
};
union cros3_csb_act_cmd_reg { /* (0x28) CROS-3 Command		     */
    unsigned int data;
/*    struct cros3_csb_act_cmd_regbits bits; */
};
union cros3_csb_csr_sts_reg { /* (0x2C) CSB Receive FIFO Status	*/
    unsigned int data;
    struct cros3_csb_csr_sts_regbits bits;
};
struct cros3_register_image_v071125 {
    int CSB_ACT_RES; /* (0x00) CSB Reset                      */
    int CSB_ACT_RFR; /* (0x04) CSB Receive FIFO Reset	     */
    int CSB_ACT_DCR; /* (0x08) CSB Data Error Counter Reset   */
    int CSB_ACT_PCR; /* (0x0C) CSB PRBS Error Counter Reset   */
    int CSB_ACT_INS; /* (0x10) CSB Interrupt Set		     */
    int CSB_ACT_INR; /* (0x14) CSB Interrupt Reset	     */
    int filler1; /* (0x18) */
    int filler2; /* (0x1C) */
    union cros3_csb_csr_fid_reg CSB_CSR_FID; /* (0x20) CSB Firmware Identifier	     */
    union cros3_csb_csr_cfg_reg CSB_CSR_CFG; /* (0x24) CSB Configuration / Status     */
    union cros3_csb_act_cmd_reg CSB_ACT_CMD; /* (0x28) CROS-3 Command		     */
    union cros3_csb_csr_sts_reg CSB_CSR_STS; /* (0x2C) CSB Receive FIFO Status	     */
    int filler3; /* 0x30) */
    int filler4; /* 0x34) */
    int filler5; /* 0x38) */
    int CSB_DAT_RW;  /* (0x3C) CSB Read/Write Register        */
    unsigned int CSB_CSR_RCT[(0x7ffc - 0x0040)/4+1]; /*0x0040 - 0x7FFC CSB Receipt / Readout Data */
};

struct cros3_register_image_v100115 {
	int CSB_ACT_RES; /* (0x00) CSB Reset                      */
	int CSB_ACT_CLR; /* (0x04) CSB Receive FIFO Reset	     */
	int CSB_ACT_DCR; /* (0x08) CSB Data Error Counter Reset   */
	int CSB_ACT_PCR; /* (0x0C) CSB PRBS Error Counter Reset   */
	int CSB_ACT_INS; /* (0x10) CSB Interrupt Set		     */
	int CSB_ACT_INR; /* (0x14) CSB Interrupt Reset	     */
	int CSB_ACT_INI; /* (0x18) CSB Fiber Link Initialize */
	int filler2; /* (0x1C) */
	union cros3_csb_csr_fid_reg CSB_CSR_FID; /* (0x20) CSB Firmware Identifier	     */
	union cros3_csb_csr_cfg_reg CSB_CSR_CFG; /* (0x24) CSB Configuration / Status     */
	union cros3_csb_act_cmd_reg CSB_ACT_CMD; /* (0x28) CROS-3 Command		     */
	union cros3_csb_csr_sts_reg CSB_CSR_STS; /* (0x2C) CSB Receive FIFO Status	     */
	int filler3; /* 0x30) */
	int filler4; /* 0x34) */
	int filler5; /* 0x38) */
	int CSB_DAT_RW;  /* (0x3C) CSB Read/Write Register        */
	unsigned int CSB_CSR_RCT[(0x7ffc - 0x0040)/4+1]; /*0x0040 - 0x7FFC CSB Receipt / Readout Data */
};

union cros3_register_image {
    struct cros3_register_image_v071125 v071125;
	struct cros3_register_image_v100115 v100115;
};


/* register offsets */
#define CROS3_CSB_ACT_RES (0x0000) 
#define CROS3_CSB_ACT_RFR (0x0004) 
#define CROS3_CSB_ACT_DCR (0x0008) 
#define CROS3_CSB_ACT_PCR (0x000C) 
#define CROS3_CSB_ACT_INS (0x0010) 
#define CROS3_CSB_ACT_INR (0x0014) 
#define CROS3_CSB_CSR_FID (0x0020) 
#define CROS3_CSB_CSR_CFG (0x0024) 
#define CROS3_CSB_ACT_CMD (0x0028) 
#define CROS3_CSB_CSR_STS (0x002C) 
#define CROS3_CSB_DAT_RW  (0x003C) 
#define CROS3_CSB_CSR_RCT (0x0040) 


/* fifo status register bitmasks and offsets */
#define CSB_CSR_STS_BM_IBSY (0x80000000)
#define CSB_CSR_STS_BM_SBSY (0x40000000)
#define CSB_CSR_STS_BM_SRSV (0x20000000)
#define CSB_CSR_STS_BM_STRG (0x10000000)
#define CSB_CSR_STS_BM_RFFF (0x08000000)
#define CSB_CSR_STS_BM_RFAF (0x04000000)
#define CSB_CSR_STS_BM_RFEF (0x01000000)
#define CSB_CSR_STS_BM_INTS (0x00008000)
#define CSB_CSR_STS_BM_DEC  (0x00FF0000)
#define CSB_CSR_STS_OF_DEC  (16)
#define CSB_CSR_STS_BM_RWC  (0x000003FF)
#define CSB_CSR_STS_OF_RWC  (0)

/*
  This data type contains the data that can be obtained via read() on the
  device, the tell when the last interrupt has occurred
  and how many interrupts since the last read() have occured.
  If that number is greater than 1 you have missed number-1 interrupts... 
  The tv_last_irq array contains in it's first irqs_since_read fields 
  the times when the interrupts happened, if there are more than 
  CROS3_MAX_IRQTIMES only the first CROS3_MAX_IRQTIMES are recorded.
 */

#define CROS3_MAX_IRQTIMES (1<<8)

struct cros3_interrupt_data_type {
  unsigned long csr_sts;   /* value of status register at interrupt time */
  unsigned long irq_number;	/* absolute number of irq since open() */
  struct timespec ts_irq;         /* time of last irqs */
};

struct cros3_read_data_type {
  unsigned long irqs_since_read;     /* num of irq since last read, ie. lost */
  struct cros3_interrupt_data_type irq[CROS3_MAX_IRQTIMES];
};

#endif
