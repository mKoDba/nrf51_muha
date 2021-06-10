PROJECT_NAME     := nrf51_muha
TARGETS          := nrf51422_xxac
OUTPUT_DIRECTORY := _build

SDK_ROOT := C:/Users/Mario/projects/nRF/nRF5_SDK_12.2.0_f012efa
SDK_DIR := C:/Users/Mario/projects/nrf51_muha/SDK
PROJ_DIR := C:/Users/Mario/projects/nrf51_muha
FREERTOS_DIR := C:/Users/Mario/projects/FreeRTOSV8.2.1/FreeRTOS

$(OUTPUT_DIRECTORY)/nrf51422_xxac.out: \
  LINKER_SCRIPT  := nrf51_muha.ld

# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/main.c \
  $(PROJ_DIR)/application/nrf51_muha.c \
  $(PROJ_DIR)/application/ble_muha.c \
  $(PROJ_DIR)/application/ble_ecgs.c \
  $(PROJ_DIR)/application/ble_mpu.c \
  $(PROJ_DIR)/application/bsp/bsp_ecg_ADS1192.c \
  $(PROJ_DIR)/application/bsp/bsp_mpu9150.c \
  $(PROJ_DIR)/application/config/bsp/cfg_bsp_ecg_ADS1192.c \
  $(PROJ_DIR)/application/config/bsp/cfg_bsp_mpu9150.c \
  $(PROJ_DIR)/application/config/drivers/cfg_drv_timer.c \
  $(PROJ_DIR)/application/config/drivers/cfg_drv_spi.c \
  $(PROJ_DIR)/application/config/drivers/cfg_drv_nrf_twi.c \
  $(PROJ_DIR)/application/config/hal/cfg_hal_watchdog.c \
  $(PROJ_DIR)/application/config/cfg_ble_muha.c \
  $(PROJ_DIR)/application/drivers/drv_common.c \
  $(PROJ_DIR)/application/drivers/drv_timer.c \
  $(PROJ_DIR)/application/drivers/drv_spi.c \
  $(PROJ_DIR)/application/hal/hal_timer.c \
  $(PROJ_DIR)/application/hal/hal_clk.c \
  $(PROJ_DIR)/application/hal/hal_spi.c \
  $(PROJ_DIR)/application/hal/hal_watchdog.c \
  $(SDK_DIR)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_DIR)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_DIR)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_DIR)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_DIR)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_DIR)/components/libraries/util/app_error.c \
  $(SDK_DIR)/components/libraries/util/app_error_weak.c \
  $(SDK_DIR)/components/libraries/util/nrf_assert.c \
  $(SDK_DIR)/components/libraries/util/sdk_errors.c \
  $(SDK_DIR)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_DIR)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \
  $(SDK_DIR)/components/drivers_nrf/twi_master/nrf_drv_twi.c \
  $(SDK_DIR)/components/drivers_nrf/twi_master/deprecated/twi_hw_master.c \
  $(SDK_DIR)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SDK_DIR)/components/toolchain/system_nrf51.c \
  $(SDK_DIR)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
  $(SDK_DIR)/components/softdevice/common/softdevice_handler/softdevice_handler_appsh.c \
  $(SDK_DIR)/components/libraries/util/app_util_platform.c \
  $(SDK_DIR)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_DIR)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_DIR)/components/libraries/fstorage/fstorage.c \
  $(SDK_DIR)/components/libraries/timer/app_timer.c \
  $(SDK_DIR)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_DIR)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_DIR)/components/ble/common/ble_advdata.c \
  $(SDK_DIR)/components/ble/common/ble_conn_params.c \
  $(SDK_DIR)/components/ble/common/ble_srv_common.c \
  $(SDK_DIR)/components/ble/ble_services/ble_bas/ble_bas.c \
    
# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/application \
  $(PROJ_DIR)/application/bsp \
  $(PROJ_DIR)/application/drivers \
  $(PROJ_DIR)/application/hal \
  $(PROJ_DIR)/application/config \
  $(PROJ_DIR)/application/config/bsp \
  $(PROJ_DIR)/application/config/drivers \
  $(PROJ_DIR)/application/config/hal \
  $(SDK_DIR)/components/libraries/util \
  $(SDK_DIR)/components/device \
  $(SDK_DIR)/components/drivers_nrf/hal \
  $(SDK_DIR)/components/drivers_nrf/common \
  $(SDK_DIR)/components/drivers_nrf/delay \
  $(SDK_DIR)/components/drivers_nrf/gpiote \
  $(SDK_DIR)/components/toolchain \
  $(SDK_DIR)/components/toolchain/cmsis/include \
  $(SDK_DIR)/components/drivers_nrf/comp \
  $(SDK_DIR)/components/drivers_nrf/twi_master \
  $(SDK_DIR)/components/drivers_nrf/twi_master/deprecated \
  $(SDK_DIR)/components/drivers_nrf/twi_master/deprecated/config \
  $(SDK_DIR)/components/ble/ble_services/ble_ancs_c \
  $(SDK_DIR)/components/ble/ble_services/ble_ias_c \
  $(SDK_DIR)/components/softdevice/s130/headers \
  $(SDK_DIR)/components/softdevice/s130/headers/nrf51 \
  $(SDK_DIR)/components/softdevice/common/softdevice_handler \
  $(SDK_DIR)/components/libraries/log \
  $(SDK_DIR)/components/libraries/scheduler \
  $(SDK_DIR)/components/ble/ble_services/ble_gls \
  $(SDK_DIR)/components/boards \
  $(SDK_DIR)/components/ble/ble_advertising \
  $(SDK_DIR)/components/ble/ble_services/ble_bas_c \
  $(SDK_DIR)/components/ble/ble_services/ble_hrs_c \
  $(SDK_DIR)/components/ble/ble_dtm \
  $(SDK_DIR)/components/ble/ble_services/ble_rscs_c \
  $(SDK_DIR)/components/ble/common \
  $(SDK_DIR)/components/ble/ble_services/ble_lls \
  $(SDK_DIR)/components/libraries/bsp \
  $(SDK_DIR)/components/ble/ble_services/ble_bas \
  $(SDK_DIR)/components/libraries/experimental_section_vars \
  $(SDK_DIR)/components/ble/ble_services/ble_ans_c \
  $(SDK_DIR)/components/libraries/slip \
  $(SDK_DIR)/external/segger_rtt \
  $(SDK_DIR)/components/libraries/usbd/class/cdc \
  $(SDK_DIR)/components/drivers_nrf/uart \
  $(SDK_DIR)/components/ble/ble_services/ble_nus_c \
  $(SDK_DIR)/components/drivers_nrf/rtc \
  $(SDK_DIR)/components/ble/ble_services/ble_ias \
  $(SDK_DIR)/components/libraries/usbd/class/hid/mouse \
  $(SDK_DIR)/components/drivers_nrf/ppi \
  $(SDK_DIR)/components/ble/ble_services/ble_dfu \
  $(SDK_DIR)/components \
  $(SDK_DIR)/components/ble/ble_services/ble_lbs \
  $(SDK_DIR)/components/ble/ble_services/ble_hts \
  $(SDK_DIR)/components/libraries/csense_drv \
  $(SDK_DIR)/components/libraries/csense \
  $(SDK_DIR)/components/libraries/low_power_pwm \
  $(SDK_DIR)/components/libraries/hardfault \
  $(SDK_DIR)/components/ble/ble_services/ble_cscs \
  $(SDK_DIR)/components/libraries/uart \
  $(SDK_DIR)/components/libraries/hci \
  $(SDK_DIR)/components/libraries/usbd/class/hid/kbd \
  $(SDK_DIR)/components/drivers_nrf/lpcomp \
  $(SDK_DIR)/components/libraries/timer \
  $(SDK_DIR)/components/drivers_nrf/power \
  $(SDK_DIR)/components/libraries/usbd/config \
  $(SDK_DIR)/components/libraries/led_softblink \
  $(SDK_DIR)/components/drivers_nrf/qdec \
  $(SDK_DIR)/components/ble/ble_services/ble_cts_c \
  $(SDK_DIR)/components/drivers_nrf/spi_master \
  $(SDK_DIR)/components/ble/ble_services/ble_nus \
  $(SDK_DIR)/components/ble/ble_services/ble_hids \
  $(SDK_DIR)/components/ble/peer_manager \
  $(SDK_DIR)/components/ble/ble_services/ble_tps \
  $(SDK_DIR)/components/ble/ble_services/ble_dis \
  $(SDK_DIR)/components/device \
  $(SDK_DIR)/components/ble/nrf_ble_gatt \
  $(SDK_DIR)/components/ble/nrf_ble_qwr \
  $(SDK_DIR)/components/ble/ble_services/ble_lbs_c \
  $(SDK_DIR)/components/ble/ble_racp \
  $(SDK_DIR)/components/toolchain/gcc \
  $(SDK_DIR)/components/libraries/twi \
  $(SDK_DIR)/components/libraries/fstorage \
  $(SDK_DIR)/components/drivers_nrf/clock \
  $(SDK_DIR)/components/ble/ble_services/ble_rscs \
  $(SDK_DIR)/components/softdevice/common/softdevice_handler \
  $(SDK_DIR)/components/ble/ble_services/ble_hrs \
  $(SDK_DIR)/components/libraries/log/src \

# Libraries common to all targets
LIB_FILES += \

# C flags common to all targets
CFLAGS += -DNRF51
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DBOARD_PCA10028
CFLAGS += -D__HEAP_SIZE=0
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -DS130
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Wno-error -O0 -g3
CFLAGS += -mfloat-abi=soft
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DRNF51
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DBOARD_PCA10028
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DS130

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash flash_softdevice

# Default target - first one defined
default: nrf51422_xxac

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	nrf51422_xxac

TEMPLATE_PATH := $(SDK_DIR)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash: $(OUTPUT_DIRECTORY)/nrf51422_xxac.hex
	@echo Flashing: $<
	nrfjprog --program $< -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

# Flash softdevice
flash_softdevice:
	@echo Flashing: s130_nrf51_2.0.1_softdevice.hex
	nrfjprog --program $(SDK_DIR)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex -f nrf51 --sectorerase 
	nrfjprog --reset -f nrf51

erase:
	nrfjprog --eraseall -f nrf51
