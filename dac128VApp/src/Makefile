TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

LIBRARY_IOC_vxWorks += dac128V
LIBRARY_IOC_RTEMS   += dac128V
LIBRARY_IOC_Linux   += dac128V

dac128V_SRCS += drvDac128V.c

DBD += dac128VSupport.dbd

dac128V_LIBS += $(EPICS_BASE_IOC_LIBS)
#=============================


#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
