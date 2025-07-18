#
# Copyright (C) 2014 Jared Boone, ShareBrained Technology, Inc.
# Copyright (C) 2016 Furrtek
#
# This file is part of PortaPack.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

enable_language(C CXX ASM)

include(CheckCXXCompilerFlag)

project(baseband_shared)

# Compiler options here.
set(USE_OPT "-O3 -g -falign-functions=16 -fno-math-errno --specs=nano.specs")

# C specific options here (added to USE_OPT).
set(USE_COPT "-std=gnu99")

# C++ specific options here (added to USE_OPT).
check_cxx_compiler_flag("-std=c++20" cpp20_supported)
if(cpp20_supported)
	set(USE_CPPOPT "-std=c++20")
else()
	set(USE_CPPOPT "-std=c++17")
endif()
set(USE_CPPOPT "${USE_CPPOPT} -fno-rtti -fno-exceptions -Weffc++ -Wuninitialized -Wno-volatile")

# Enable this if you want the linker to remove unused code and data
set(USE_LINK_GC yes)

# Linker extra options here.
set(USE_LDOPT)

# Enable this if you want link time optimizations (LTO)
set(USE_LTO no)

# If enabled, this option allows to compile the application in THUMB mode.
set(USE_THUMB yes)

# Enable this if you want to see the full log while compiling.
set(USE_VERBOSE_COMPILE no)

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Enables the use of FPU on Cortex-M4 (no, softfp, hard).
set(USE_FPU hard)

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, sources and paths
#

# Imported source files and paths
include(${CHIBIOS_PORTAPACK}/boards/PORTAPACK_BASEBAND/board.cmake)
include(${CHIBIOS_PORTAPACK}/os/hal/platforms/LPC43xx_M4/platform.cmake)
include(${CHIBIOS}/os/hal/hal.cmake)
include(${CHIBIOS_PORTAPACK}/os/ports/GCC/ARMCMx/LPC43xx_M4/port.cmake)
include(${CHIBIOS}/os/kernel/kernel.cmake)

include(${CHIBIOS}/test/test.cmake)

# Define linker script file here
set(LDSCRIPT ${PORTLD}/LPC43xx_M4.ld)

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
set(CSRC
	${PORTSRC}
	${KERNSRC}
	${TESTSRC}
	${BOARDSRC}
)

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
set(CPPSRC
	baseband.cpp
	${COMMON}/message_queue.cpp
	${COMMON}/event.cpp
	event_m4.cpp
	${COMMON}/thread_wait.cpp
	${COMMON}/gpdma.cpp
	baseband_dma.cpp
	${COMMON}/baseband_sgpio.cpp
	${COMMON}/portapack_shared_memory.cpp
	${COMMON}/buffer.cpp
	baseband_thread.cpp
	baseband_processor.cpp
	baseband_stats_collector.cpp
	dsp_decimate.cpp
	dsp_demodulate.cpp
	dsp_hilbert.cpp
	dsp_modulate.cpp
	dsp_goertzel.cpp
	matched_filter.cpp
	spectrum_collector.cpp
	tv_collector.cpp
	stream_input.cpp
	stream_output.cpp
	dsp_squelch.cpp
	clock_recovery.cpp
	packet_builder.cpp
	${COMMON}/dsp_fft.cpp
	${COMMON}/dsp_fir_taps.cpp
	${COMMON}/dsp_iir.cpp
	${COMMON}/dsp_sos.cpp
	fxpt_atan2.cpp
	rssi.cpp
	rssi_dma.cpp
	rssi_thread.cpp
	audio_compressor.cpp
	audio_output.cpp
	audio_input.cpp
	audio_dma.cpp
	audio_stats_collector.cpp
	${COMMON}/utility.cpp
	${COMMON}/chibios_cpp.cpp
	debug.cpp
	${COMMON}/gcc.cpp
	${COMMON}/performance_counter.cpp
	${COMMON}/random.cpp
	tone_gen.cpp
)

# C sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
set(ACSRC)

# C++ sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
set(ACPPSRC)

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
set(TCSRC)

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
set(TCPPSRC)

# List ASM source files here
set(ASMSRC ${PORTASM})

set(INCDIR ${COMMON} ${PORTINC} ${KERNINC} ${TESTINC}
	${HALINC} ${PLATFORMINC} ${BOARDINC}
	${CHIBIOS}/os/various
)

#
# Project, sources and paths
##############################################################################

##############################################################################
# Compiler settings
#

set(MCU cortex-m4)

# ARM-specific options here
set(AOPT)

# THUMB-specific options here
set(TOPT "-mthumb -DTHUMB")

# Define C warning options here
set(CWARN "-Wall -Wextra")

# Define C++ warning options here
set(CPPWARN "-Wall -Wextra")

#
# Compiler settings
##############################################################################

##############################################################################
# Start of default section
#

# List all default C defines here, like -D_DEBUG=1
# TODO: Switch -DCRT0_INIT_DATA depending on load from RAM or SPIFI?
# NOTE: _RANDOM_TCC to kill a GCC 4.9.3 error with std::max argument types
set(DDEFS "-DLPC43XX -DLPC43XX_M4 -D__NEWLIB__ -DHACKRF_ONE -DTOOLCHAIN_GCC -DTOOLCHAIN_GCC_ARM -D_RANDOM_TCC=0 -D'VERSION_STRING=\"${VERSION}\"'")

# List all default ASM defines here, like -D_DEBUG=1
set(DADEFS)

# List all default directories to look for include files here
set(DINCDIR)

# List the default directory to look for the libraries here
set(DLIBDIR)

# List all default libraries here
set(DLIBS)

#
# End of default section
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
set(UDEFS)

# Define ASM defines here
set(UADEFS)

# List all user directories here
set(UINCDIR)

# List the user directory to look for the libraries here
set(ULIBDIR)

# List all user libraries here
set(ULIBS)

#
# End of user defines
##############################################################################

set(RULESPATH ${CHIBIOS}/os/ports/GCC/ARMCMx)
include(${RULESPATH}/rules.cmake)

#######################################################################

add_library(${PROJECT_NAME} OBJECT ${CSRC} ${CPPSRC} ${ASMSRC})
include_directories(. ${INCDIR})

#######################################################################

set(BASEBAND_IMAGES)
set(BASEBAND_EXTERNAL_IMAGES)

macro(DeclareTargets chunk_tag name)
	project("baseband_${name}")

	include(${RULESPATH}/rules.cmake)
	set_source_files_properties(${MODE_CPPSRC} PROPERTIES COMPILE_FLAGS "${MODE_FLAGS}")
	add_executable(${PROJECT_NAME}.elf $<TARGET_OBJECTS:baseband_shared> ${MODE_CPPSRC} ${HALSRC} ${PLATFORMSRC})
	set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINK_DEPENDS ${LDSCRIPT})
	add_definitions(${DEFS})
	target_compile_definitions(${PROJECT_NAME}.elf PUBLIC "-DBASEBAND_${name}")
	include_directories(. ${INCDIR} ${MODE_INCDIR})
	link_directories(${LLIBDIR})
	target_link_libraries(${PROJECT_NAME}.elf ${LIBS})
	
	target_link_libraries(${PROJECT_NAME}.elf -Wl,-Map=${PROJECT_NAME}.map)
	target_link_libraries(${PROJECT_NAME}.elf -Wl,--print-memory-usage)

	if(add_to_firmware)

		add_custom_command(
			OUTPUT ${chunk_tag}.bin ${PROJECT_NAME}.img
			COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${chunk_tag}.bin
			COMMAND ${LZ4} -f -9 ${chunk_tag}.bin ${PROJECT_NAME}.lz4
			COMMAND ${MAKE_IMAGE_CHUNK} ${PROJECT_NAME}.lz4 ${chunk_tag} ${PROJECT_NAME}.img
			DEPENDS ${PROJECT_NAME}.elf ${MAKE_IMAGE_CHUNK}
			VERBATIM
		)

		set(BASEBAND_IMAGES ${BASEBAND_IMAGES} ${PROJECT_NAME}.img)

	else()

		add_custom_command(
			OUTPUT ${chunk_tag}.bin
			COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${chunk_tag}.bin
			DEPENDS ${PROJECT_NAME}.elf
			VERBATIM
		)

		set(BASEBAND_EXTERNAL_IMAGES ${BASEBAND_EXTERNAL_IMAGES} ${chunk_tag}.bin)

	endif()



endmacro()

set(add_to_firmware TRUE)
set(MODE_FLAGS "-O3")


### ADS-B RX

set(MODE_CPPSRC
	proc_adsbrx.cpp
)
DeclareTargets(PADR adsbrx)


### AFSK TX

set(MODE_CPPSRC
	proc_afsk.cpp
)
DeclareTargets(PAFT afsktx)

### APRS RX

set(MODE_CPPSRC
	proc_aprsrx.cpp
)
DeclareTargets(PAPR aprsrx)


### BTLE RX

set(MODE_CPPSRC
	proc_btlerx.cpp
)
DeclareTargets(PBTR btlerx)

### BTLE TX

set(MODE_CPPSRC
	proc_ble_tx.cpp
)
DeclareTargets(PBTT btletx)

### AIS

set(MODE_CPPSRC
	proc_ais.cpp
)
DeclareTargets(PAIS ais)

### AM Audio

set(MODE_CPPSRC
	proc_am_audio.cpp
)
DeclareTargets(PAMA am_audio)


### Audio transmit

set(MODE_CPPSRC
	proc_audiotx.cpp
)
DeclareTargets(PATX audio_tx)

### Capture

set(MODE_CPPSRC
	proc_capture.cpp
)
DeclareTargets(PCAP capture)

### ERT

set(MODE_CPPSRC
	proc_ert.cpp
)
DeclareTargets(PERT ert)

### Radiosonde

set(MODE_CPPSRC
	proc_sonde.cpp
)
DeclareTargets(PSON sonde)

### FSK RX

set(MODE_CPPSRC
	proc_fsk_rx.cpp
)
DeclareTargets(PFSR fskrx)

### FSK TX

set(MODE_CPPSRC
	proc_fsk.cpp
)
DeclareTargets(PFSK fsktx)

### Microphone transmit

set(MODE_CPPSRC
	proc_mictx.cpp
)
DeclareTargets(PMTX mic_tx)

### NFM Audio

set(MODE_CPPSRC
	proc_nfm_audio.cpp
)
DeclareTargets(PNFM nfm_audio)

### OOK

set(MODE_CPPSRC
	proc_ook.cpp
)
DeclareTargets(POOK ook)


### POCSAG2 RX

set(MODE_CPPSRC
	proc_pocsag2.cpp
)
DeclareTargets(PPO2 pocsag2)

### RDS

set(MODE_CPPSRC
	proc_rds.cpp
)
DeclareTargets(PRDS rds)

### Replay

set(MODE_CPPSRC
	proc_replay.cpp
)
DeclareTargets(PREP replay)

### Signal generator

set(MODE_CPPSRC
	proc_siggen.cpp
)
DeclareTargets(PSIG siggen)



### Tones

set(MODE_CPPSRC
	proc_tones.cpp
)
DeclareTargets(PTON tones)


### Wideband Spectrum

set(MODE_CPPSRC
	proc_wideband_spectrum.cpp
)
DeclareTargets(PSPE wideband_spectrum)

### WFM Audio

set(MODE_CPPSRC
	proc_wfm_audio.cpp
)
DeclareTargets(PWFM wfm_audio)

### SubGhz Decoders

set(MODE_CPPSRC
	proc_subghzd.cpp
)
DeclareTargets(PSGD subghzd)


### Weather Stations
set(MODE_CPPSRC
	proc_weather.cpp
)
DeclareTargets(PWTH weather)

set(MODE_FLAGS "-Os")

### Flash Utility

include(${CHIBIOS_PORTAPACK}/os/various/fatfs_bindings/fatfs.cmake)
set(MODE_INCDIR ${FATFSINC})
set(MODE_CPPSRC
	proc_flash_utility.cpp
	w25q80bv.cpp
	${FATFSSRC}
)
DeclareTargets(PFUT flash_utility)

### SD over USB

set(MODE_INCDIR
	${HACKRF_PATH}/firmware
	${HACKRF_PATH}/firmware/common
	${HACKRF_PATH}/firmware/libopencm3/include
)
set(MODE_CPPSRC
	sd_over_usb/proc_sd_over_usb.cpp

	sd_over_usb/scsi.c
	sd_over_usb/diskio.c
	sd_over_usb/sd_over_usb.c
	sd_over_usb/usb_descriptor.c
	sd_over_usb/hackrf_core.c

	${HACKRF_PATH}/firmware/common/usb.c
	${HACKRF_PATH}/firmware/common/usb_queue.c
	${HACKRF_PATH}/firmware/hackrf_usb/usb_device.c
	${HACKRF_PATH}/firmware/hackrf_usb/usb_endpoint.c
	${HACKRF_PATH}/firmware/common/usb_request.c
	${HACKRF_PATH}/firmware/common/usb_standard_request.c
	${HACKRF_PATH}/firmware/common/platform_detect.c
	${HACKRF_PATH}/firmware/common/gpio_lpc.c
	${HACKRF_PATH}/firmware/common/firmware_info.c
	${HACKRF_PATH}/firmware/common/si5351c.c
	${HACKRF_PATH}/firmware/common/i2c_bus.c
	${HACKRF_PATH}/firmware/common/mixer.c
	${HACKRF_PATH}/firmware/common/clkin.c
	${HACKRF_PATH}/firmware/common/spi_bus.c
	${HACKRF_PATH}/firmware/common/sgpio.c
	${HACKRF_PATH}/firmware/common/rf_path.c
	${HACKRF_PATH}/firmware/common/i2c_lpc.c
	${HACKRF_PATH}/firmware/common/spi_ssp.c
	${HACKRF_PATH}/firmware/common/rffc5071_spi.c
	${HACKRF_PATH}/firmware/common/rffc5071.c
	${HACKRF_PATH}/firmware/common/clkin.c
	${HACKRF_PATH}/firmware/common/gpdma.c

	${HACKRF_PATH}/firmware/libopencm3/lib/cm3/nvic.c
	${HACKRF_PATH}/firmware/libopencm3/lib/cm3/sync.c
	${HACKRF_PATH}/firmware/libopencm3/lib/lpc43xx/scu.c
	${HACKRF_PATH}/firmware/libopencm3/lib/lpc43xx/timer.c
	${HACKRF_PATH}/firmware/libopencm3/lib/lpc43xx/i2c.c
)
DeclareTargets(PUSB sd_over_usb)

### Place external app and disabled images below so they don't get added to the firmware
set(add_to_firmware FALSE)
set(MODE_FLAGS "-O3")

### ACARS RX

set(MODE_CPPSRC
	proc_acars.cpp
)
DeclareTargets(PACA acars)


### AFSK RX

set(MODE_CPPSRC
	proc_afskrx.cpp
)
DeclareTargets(PAFR afskrx)

### NRF RX

set(MODE_CPPSRC
	proc_nrfrx.cpp
)
DeclareTargets(PNRR nrfrx)

### Jammer

set(MODE_CPPSRC
	proc_jammer.cpp
)
DeclareTargets(PJAM jammer)

### AM TV

set(MODE_CPPSRC
	proc_am_tv.cpp
)
DeclareTargets(PAMT am_tv)

### GPS Simulator

set(MODE_CPPSRC
	proc_gps_sim.cpp
)
DeclareTargets(PGPS gps_sim)

### Spectrum painter

set(MODE_CPPSRC
	proc_spectrum_painter.cpp
)
DeclareTargets(PSPT spectrum_painter)

### Test

set(MODE_CPPSRC
	proc_test.cpp
)
DeclareTargets(PTST test)

### Audio Beep

set(MODE_CPPSRC
	proc_audio_beep.cpp
)
DeclareTargets(PABP audio_beep)


### ProtoView

set(MODE_CPPSRC
	proc_protoview.cpp
)
DeclareTargets(PPVW protoview)


### SSTV TX

set(MODE_CPPSRC
	proc_sstvtx.cpp
)
DeclareTargets(PSTX sstvtx)

### TPMS

set(MODE_CPPSRC
	proc_tpms.cpp
)
DeclareTargets(PTPM tpms)


### ADS-B TX

set(MODE_CPPSRC
	proc_adsbtx.cpp
)
DeclareTargets(PADT adsbtx)

### OOKStream

set(MODE_CPPSRC
	proc_ook_stream_tx.cpp
)
DeclareTargets(POSK ookstream)



### WeFax Rx

set(MODE_CPPSRC
	proc_wefaxrx.cpp
)
DeclareTargets(PWFX wefaxrx)


### NoaaApt Rx

set(MODE_CPPSRC
	proc_noaaapt_rx.cpp
)
DeclareTargets(PNOA noaaapt_rx)


### HackRF "factory" firmware

add_custom_command(
	OUTPUT hackrf.img
	COMMAND ${LZ4} -f -9 ${HACKRF_FIRMWARE_BIN_IMAGE} hackrf.lz4
	COMMAND ${MAKE_IMAGE_CHUNK} hackrf.lz4 HRF1 hackrf.img
	DEPENDS ${HACKRF_FIRMWARE_BIN_FILENAME} ${MAKE_IMAGE_CHUNK}
	VERBATIM
)

set(BASEBAND_IMAGES ${BASEBAND_IMAGES} hackrf.img)

### Terminator image

add_custom_command(
	OUTPUT terminator.img
	COMMAND ${MAKE_IMAGE_CHUNK} terminator.img
	DEPENDS ${MAKE_IMAGE_CHUNK}
	VERBATIM
)

set(BASEBAND_IMAGES ${BASEBAND_IMAGES} terminator.img)

#######################################################################

project(baseband)

add_custom_command(
	OUTPUT ${PROJECT_NAME}.img
	COMMAND cat ${BASEBAND_IMAGES} > ${PROJECT_NAME}.img
	DEPENDS ${BASEBAND_IMAGES} ${BASEBAND_EXTERNAL_IMAGES}
	DEPENDS hackrf.img terminator.img
	VERBATIM
)

add_custom_target(
	${PROJECT_NAME}
	DEPENDS ${PROJECT_NAME}.img
)
