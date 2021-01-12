# $Id: Makefile,v 1.5 2008/05/24 04:17:21 hannappe Exp $
#GCC_VER_MAJ      = $(shell $(CC) --version | head -n 1 | sed -e 's/[^0-9. ]//g;s/^ *//;s/^\(.\)\..*$$/\1/')

LIBIP_PREFIX	?= ..

obj-m           += cros3.o
cros3-hdrs 	+= cros3.h

EXTRA_CFLAGS    += \
                -I$(PWD) \

ifeq ($(KERNELRELEASE),)
# on first call from remote location we get into this path
# whilst on second call all is managed by the embedding kernel makefile

KVER            = $(shell uname -r)
# KVER            = 2.6.0-0.test1.1.26custom

ifeq (1,1)
KDIR            = /lib/modules/$(KVER)/build
else
KDIR            = /usr/src/linux-$(KVER)
endif
PWD             = $(shell pwd)

cros3-cfiles    = ${cros3-c-objs:.o=.c}

default:: kmod_build

kmod_build:: $(cros3-cfiles) $(cros3-hdrs)
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

%.c:
	@ln -s ../$@

%.h:
	@ln -s ../$@

clean:
	rm -f *.o *.mod.c Module.symvers Module.markers modules.order \.*.o.cmd \.*.ko.cmd \.*.o.d
	rm -rf \.tmp_versions

distclean: mrproper
mrproper:    clean
	rm -f *.ko

dist:
	mkdir -p $(DISTDIR)
	ln cros3.c $(cros3-cfiles) $(cros3-hdrs) $(DISTDIR)
	ln Makefile 99-cros3.rules $(DISTDIR)

endif

# ### EOF ###
