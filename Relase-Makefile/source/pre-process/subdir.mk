################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/pre-process/Process_ReactionParameters.cpp \
../source/pre-process/Reactions_ForReactionRates.cpp \
../source/pre-process/Species_ForSpeciesLoss.cpp 

OBJS += \
./source/pre-process/Process_ReactionParameters.o \
./source/pre-process/Reactions_ForReactionRates.o \
./source/pre-process/Species_ForSpeciesLoss.o 

CPP_DEPS += \
./source/pre-process/Process_ReactionParameters.d \
./source/pre-process/Reactions_ForReactionRates.d \
./source/pre-process/Species_ForSpeciesLoss.d 


# Each subdirectory must supply rules for building sources it contributes
source/pre-process/%.o: ../source/pre-process/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=gnu++14 -I"X:\workspace\CKS branches\headers" -O2 -m64 -march=native -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


