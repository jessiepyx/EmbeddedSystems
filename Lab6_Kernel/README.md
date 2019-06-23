# Lab 6: Kernel

Linux source code compilation, system call and kernel module experiments on Raspberry Pi.

- Kernel source: [Linux 4.9](https://github.com/raspberrypi/linux)
- Raspberry Pi 3B+

Cross-compilation:

- Host: Ubuntu 16.04 64-bit
- Toolchain: [arm-linux-gnueabihf-gcc](https://github.com/raspberrypi/tools/tree/master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin)

## Kernel Building

- Refer to [official documentation](https://www.raspberrypi.org/documentation/linux/kernel/building.md).

- [*linux-4.9*](https://github.com/jessiepyx/EmbeddedSystems/tree/master/Lab6_Kernel/Ubuntu/linux-4.9) is my cross-compiled kernel.

## System Call

- [*sys_mysyscall.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab6_Kernel/Ubuntu/linux-4.9/arch/arm/kernel/sys_mysyscall.c), system call number: 223
- [*syscall1.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab6_Kernel/raspi/syscall1.c) uses syscall().
- [*syscall2.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab6_Kernel/raspi/syscall2.c) uses embedded assembly code.

## Kernel Module

- [*hello.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab6_Kernel/Ubuntu/modules/hello.c) is cross-compiled on Ubuntu.

