TREE_ROOT?=$(DRIVER_TOPDIR)/
CONFIG_STGFB_PATH?=$(DRIVER_TOPDIR)/stgfb/stmfb
CCFLAGSY += -I$(TREE_ROOT)/linux/include/linux/stm/
CCFLAGSY += -I$(TREE_ROOT)/linux/drivers/media/video/stm/
CCFLAGSY += -I$(KERNEL_LOCATION)/drivers/media/dvb/dvb-core
CCFLAGSY += -I$(CONFIG_STGFB_PATH)/include
CCFLAGSY += -DDEBUG_DVB
CCFLAGSY += -I$(TREE_ROOT)/include

ifdef SPARK7162
CCFLAGSY += -I$(TREE_ROOT)/frontends/spark7162
CCFLAGSY += -I$(TREE_ROOT)/frontends/spark7162/stv0367
CCFLAGSY += -I$(TREE_ROOT)/frontends/spark7162/stv0367/stv0367qam
CCFLAGSY += -I$(TREE_ROOT)/frontends/spark7162/stv0367/stv0367ter
CCFLAGSY += -Wall #-Werror
endif

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
stv090x-objs += spark_dvbapi5/vz7903.o
obj-m += stv090x.o
obj-y += lnb/
else ifdef SPARK7162
spark7162-objs +=        spark7162/spark7162.o

spark7162-objs +=        spark7162/D3501.o

spark7162-objs +=        base/ix7306.o
spark7162-objs +=        base/sharp7803.o
spark7162-objs +=        base/sharp6465.o
spark7162-objs +=        base/sharp5469c.o

spark7162-objs +=        spark7162/stv0367/D0367.o
spark7162-objs +=        spark7162/stv0367/D0367ter.o
spark7162-objs +=        spark7162/stv0367/D0367cab.o

spark7162-objs +=        spark7162/stv0367/stv0367ter/chip_0367ter.o
spark7162-objs +=        spark7162/stv0367/stv0367ter/stv0367ofdm_drv.o
spark7162-objs +=        spark7162/stv0367/stv0367ter/d0367_ter.o

spark7162-objs +=        spark7162/stv0367/stv0367qam/chip_0367qam.o
spark7162-objs +=        spark7162/stv0367/stv0367qam/stv0367qam_drv.o
spark7162-objs +=        spark7162/stv0367/stv0367qam/d0367_qam.o

obj-m += spark7162.o
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
else ifdef SPARK7162
spark7162-objs +=        spark7162/spark7162.o

spark7162-objs +=        spark7162/D3501.o

spark7162-objs +=        base/ix7306.o
spark7162-objs +=        base/sharp7803.o
spark7162-objs +=        base/sharp6465.o
spark7162-objs +=        base/sharp5469c.o

spark7162-objs +=        spark7162/stv0367/D0367.o
spark7162-objs +=        spark7162/stv0367/D0367ter.o
spark7162-objs +=        spark7162/stv0367/D0367cab.o

spark7162-objs +=        spark7162/stv0367/stv0367ter/chip_0367ter.o
spark7162-objs +=        spark7162/stv0367/stv0367ter/stv0367ofdm_drv.o
spark7162-objs +=        spark7162/stv0367/stv0367ter/d0367_ter.o

spark7162-objs +=        spark7162/stv0367/stv0367qam/chip_0367qam.o
spark7162-objs +=        spark7162/stv0367/stv0367qam/stv0367qam_drv.o
spark7162-objs +=        spark7162/stv0367/stv0367qam/d0367_qam.o

obj-m += spark7162.o
endif

# other drivers already in kernel

endif
