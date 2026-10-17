# SPDX-License-Identifier: GPL-2.0

obj-$(CONFIG_DAMON)		:= core.o
obj-$(CONFIG_DAMON_VADDR)	+= prmtv-common.o vaddr.o
obj-$(CONFIG_DAMON_PADDR)	+= prmtv-common.o paddr.o
obj-$(CONFIG_DAMON_DBGFS)	+= dbgfs.o
obj-$(CONFIG_DAMON_RECLAIM)	+= reclaim.o
# [VENDOR-PORT Phase6] makefile
obj-$(CONFIG_DAMON_LRU_SORT)	+= modules-common.o lru_sort.o
# [VENDOR-PORT Phase6] makefile
obj-$(CONFIG_DAMON_SYSFS)	+= sysfs-common.o sysfs-schemes.o sysfs.o
