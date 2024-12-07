cmake_minimum_required(VERSION 3.10)

# Set the project name
project(KeyReader)

# Specify the cross-compiler
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# Set the CPU and FPU options
set(CPU_FLAGS "-mcpu=cortex-m4")
set(FPU_FLAGS "-mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Set the compiler flags
set(CMAKE_C_FLAGS "${CPU_FLAGS} ${FPU_FLAGS} -mthumb -Os -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -Dgcc -O0 -g -DPART_TM4C123GH6PM -DTARGET_IS_BLIZZARD_RA1")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}")

# Set the linker flags
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/tm4c123gh6pm.ld)
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} ${FPU_FLAGS} -mthumb -Wl,--gc-sections -T ${LINKER_SCRIPT}")

# Include directories
include_directories(sw/inc)

# Define the variable for source files (you can add more source files here if needed)
set(SRC_FILES
  src/startup_gcc.c
  src/main.c
  src/PLL.c
  src/LaunchPad.c
  src/UART.c
  src/Timer0A.c
  src/Timer1A.c
  src/Timer2A.c
  src/Timer3A.c
  src/Timer5A.c
  src/Notes.c
  src/SensorIn.c
  src/Communication.c
  src/UART.c
)

# Add the executable using the SRC_FILES variable
add_executable(exe.elf ${SRC_FILES})

# Convert the ELF to BIN
add_custom_command(TARGET exe.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${CMAKE_BINARY_DIR}/exe.elf ${CMAKE_BINARY_DIR}/exe.bin
    COMMENT "Converting ELF to BIN"
)

# Clean up object and dependency files after build
add_custom_command(TARGET exe.elf POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.o ${CMAKE_BINARY_DIR}/*.d
    COMMENT "Removing object and dependency files"
)
