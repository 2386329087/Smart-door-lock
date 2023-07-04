################################################################################
# MRS Version: {"version":"1.8.5","date":"2023/05/22"}
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/AP3216C.c \
../User/AS608.c \
../User/IIC.c \
../User/LCD.c \
../User/PWM.c \
../User/ch32v30x_it.c \
../User/keyboard.c \
../User/main.c \
../User/ov2640.c \
../User/system_ch32v30x.c 

OBJS += \
./User/AP3216C.o \
./User/AS608.o \
./User/IIC.o \
./User/LCD.o \
./User/PWM.o \
./User/ch32v30x_it.o \
./User/keyboard.o \
./User/main.o \
./User/ov2640.o \
./User/system_ch32v30x.o 

C_DEPS += \
./User/AP3216C.d \
./User/AS608.d \
./User/IIC.d \
./User/LCD.d \
./User/PWM.d \
./User/ch32v30x_it.d \
./User/keyboard.d \
./User/main.d \
./User/ov2640.d \
./User/system_ch32v30x.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"E:\DOC\git\zhizhangmengsuo-1\Debug" -I"E:\DOC\git\zhizhangmengsuo-1\Core" -I"E:\DOC\git\zhizhangmengsuo-1\User" -I"E:\DOC\git\zhizhangmengsuo-1\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

