CPPINCDIR=/home/sean/Code/C++
PIOPKGDIR=/home/sean/.platformio/packages
SDKDIR=$(PIOPKGDIR)/framework-arduinoespressif32/tools/sdk/include

INC=$(SDKDIR)/micro-ecc ./src ./include  $(PIOPKGDIR)/framework-arduinoespressif32/libraries/SPI/src $(PIOPKGDIR)/framework-arduinoespressif32/libraries/Wire/src $(PIOPKGDIR)/framework-arduinoespressif32/cores/esp32 $(SDKDIR)/freertos $(SDKDIR)/newlib $(SDKDIR)/config $(SDKDIR)/esp32 $(SDKDIR)/soc $(SDKDIR)/heap $(SDKDIR)/esp32/rom $(SDKDIR)/driver $(SDKDIR)/log $(SDKDIR)/spi_flash $(PIOPKGDIR)/framework-arduinoespressif32/variants/esp32micromod $(SDKDIR)/esp_adc_cal

INC_PARAMS=$(INC:%=-I%) -I"$(CPPINCDIR)/SparkFun VEML6075 Arduino Library/src" -I"$(CPPINCDIR)/SparkFun AS3935 Lightning Detector Arduino Library/src" -I"$(CPPINCDIR)/SparkFun BME280/src"
INC_DIR = ./include
CFLAGS=-std=gnu99 -Wno-old-style-declaration -Os -g3 -nostdlib -Wpointer-arith -Wno-error=unused-but-set-variable -Wno-error=unused-variable -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -Wno-error=deprecated-declarations -Wno-error=unused-function -Wno-unused-parameter -Wno-sign-compare -fstack-protector -fexceptions -Werror=reorder $(INC_PARAMS)
CPPFLAGS=-fno-rtti -fno-exceptions -std=gnu++11 -Os -g3 -nostdlib -Wpointer-arith -Wno-error=unused-but-set-variable -Wno-error=unused-variable -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -Wno-error=deprecated-declarations -Wno-error=unused-function -Wno-unused-parameter -Wno-sign-compare -fstack-protector -fexceptions -Werror=reorder $(INC_PARAMS) -DARDUINO=101
# Specify extensions of files to delete when cleaning
CLEANEXTS   = o a 

# Specify the target file and the install directory
OUTPUTFILE  = libMicroModWeatherStation.a
INSTALLDIR  = ../binaries

# Default target
.PHONY: all
all: $(OUTPUTFILE)

# Build libjohnpaul.a from john.o, paul.o, and johnpaul.o
$(OUTPUTFILE): station.o
	ar ru $@ $^
	ranlib $@

# No rule to build john.o, paul.o, and johnpaul.o from .cpp 
# files is required; this is handled by make's database of
# implicit rules

.PHONY: install
install:
	mkdir -p $(INSTALLDIR)
	cp -p $(OUTPUTFILE) $(INSTALLDIR)

.PHONY: clean 
clean:
	for file in $(CLEANEXTS); do rm -f *.$$file; done

# Indicate dependencies of .ccp files on .hpp files
src/station_init.o: src/station_init.cpp
src/station_loop.o: src/station_loop.cpp
src/station_as3935.o: src/station_as3935.cpp
src/station_bme280.o: src/station_bme280.cpp
src/station_veml6075.o: src/station_veml6075.cpp
src/station_wind.o: src/station_wind.cpp
src/station_rain.o: src/station_rain.cpp

station.o: src/station_init.o src/station_loop.o src/station_as3935.o src/station_bme280.o src/station_veml6075.o src/station_wind.o src/station_rain.o

