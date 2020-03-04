TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

LIBRARY_IOC_vxWorks += ip230A
LIBRARY_IOC_RTEMS   += ip230A
LIBRARY_IOC_Linux   += ip230A

ip230A_SRCS += drvIP230A.cpp

DBD += ip230ASupport.dbd

ip230A_LIBS += $(EPICS_BASE_IOC_LIBS)
#=============================


#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
