TREE_ROOT?=$(DRIVER_TOPDIR)/
CONFIG_STGFB_PATH?=$(DRIVER_TOPDIR)/stgfb/stmfb
CCFLAGSY += -I$(TREE_ROOT)/linux/include/linux/stm/
CCFLAGSY += -I$(TREE_ROOT)/linux/drivers/media/video/stm/
CCFLAGSY += -I$(KERNEL_LOCATION)/drivers/media/dvb/dvb-core
CCFLAGSY += -I$(CONFIG_STGFB_PATH)/include
CCFLAGSY += -DDEBUG_DVB
CCFLAGSY += -I$(TREE_ROOT)/include
ifneq (,$(findstring 2.6.3,$(KERNELVERSION)))
ccflags-y += $(CCFLAGSY)
else
CFLAGS += $(CCFLAGSY)
endif

ifeq (,$(findstring 2.6.3,$(KERNELVERSION)))
# < 2.6.30
ifdef SPARK
stv090x-objs +=	spark/stv090x.o
stv090x-objs +=	spark/stv6110x.o
stv090x-objs +=	spark/ix7306.o
stv090x-objs +=	spark/core.o
obj-m += stv090x.o
obj-y += lnb/
endif

else

# >= 2.6.30
ifdef SPARK
stv090x-objs +=	spark_dvbapi5/stv090x.o
stv090x-objs +=	spark_dvbapi5/stv6110x.o
stv090x-objs +=	spark_dvbapi5/ix7306.o
stv090x-objs +=	spark_dvbapi5/core.o
obj-m += stv090x.o
obj-y += lnb/
endif

# other drivers already in kernel

endif
