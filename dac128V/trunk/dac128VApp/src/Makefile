TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

LIBRARY_IOC += dac128V

dac128V_SRCS_vxWorks += dac128V.cc
dac128V_SRCS_vxWorks += devAoDAC128V.cc
dac128V_SRCS_vxWorks += dac128VServer.cc

INC += DAC128V.h

dac128V_LIBS += $(EPICS_BASE_IOC_LIBS)
#=============================


#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
