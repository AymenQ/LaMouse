MCU 		 = at90usb1286
ARCH		 = AVR8
BOARD		 = LaFortuna
F_CPU		 = 8000000
F_USB		 = $(F_CPU)
OPTIMIZATION = s
TARGET		 = main
OS_SRC       = os.c
OS_SRC += $(wildcard rios/*.c)
OS_SRC += $(wildcard ruota/*.c)
OS_SRC += $(wildcard fatfs/*.c)
OS_SRC += $(wildcard lcd/*.c)
SRC			 = $(TARGET).c Descriptors.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS) $(OS_SRC)
LUFA_PATH = lufa/LUFA
CC_FLAGS	 = -DUSE_LUFA_CONFIG_HEADER -IConfig/

all:

upload: main.hex
	dfu-programmer $(MCU) erase --force
	dfu-programmer $(MCU) flash main.hex
	dfu-programmer $(MCU) launch

# Include LUFA-specific DMBS extension modules
DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

# Include common DMBS build system modules
DMBS_PATH      ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk
