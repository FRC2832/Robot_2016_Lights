################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../Arduino.ino 

CPP_SRCS += \
../.ino.cpp \
C:/Users/justi/Downloads/Adafruit_NeoPixel-master/Adafruit_NeoPixel.cpp 

LINK_OBJ += \
./.ino.cpp.o \
./Adafruit_NeoPixel.cpp.o 

INO_DEPS += \
./Arduino.ino.d 

CPP_DEPS += \
./.ino.cpp.d \
./Adafruit_NeoPixel.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
.ino.cpp.o: ../.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Users\justi\Downloads\eclipse-cpp-mars-1-win32-x86_64\eclipse\arduinoPlugin\tools\arduino\avr-gcc\4.8.1-arduino5/bin/avr-g++" -c -g -Os -std=gnu++11 -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10606 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR     -I"C:\Users\justi\Downloads\eclipse-cpp-mars-1-win32-x86_64\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.9\cores\arduino" -I"C:\Users\justi\Downloads\eclipse-cpp-mars-1-win32-x86_64\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.9\variants\standard" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

Adafruit_NeoPixel.cpp.o: C:/Users/justi/Downloads/Adafruit_NeoPixel-master/Adafruit_NeoPixel.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Users\justi\Downloads\eclipse-cpp-mars-1-win32-x86_64\eclipse\arduinoPlugin\tools\arduino\avr-gcc\4.8.1-arduino5/bin/avr-g++" -c -g -Os -std=gnu++11 -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10606 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR     -I"C:\Users\justi\Downloads\eclipse-cpp-mars-1-win32-x86_64\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.9\cores\arduino" -I"C:\Users\justi\Downloads\eclipse-cpp-mars-1-win32-x86_64\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.9\variants\standard" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

Arduino.o: ../Arduino.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Users\justi\Downloads\eclipse-cpp-mars-1-win32-x86_64\eclipse\arduinoPlugin\tools\arduino\avr-gcc\4.8.1-arduino5/bin/avr-g++" -c -g -Os -std=gnu++11 -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10606 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR     -I"C:\Users\justi\Downloads\eclipse-cpp-mars-1-win32-x86_64\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.9\cores\arduino" -I"C:\Users\justi\Downloads\eclipse-cpp-mars-1-win32-x86_64\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.9\variants\standard" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '


