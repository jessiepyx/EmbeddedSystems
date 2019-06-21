# Lab 5: Thermometer

## Environment

- Demoboard: STM32F103C8Tx Core Board
- Host: Windows 7 x64 (VMware Fusion Virtual Machine)
- IDE: Keil uVision5 (MDK-ARM)
- Debugger/Programmer: ST-LINK V2
- Kernel: uC/OS-II [source code](https://www.micrium.com/download/micrium_stm32xxx_ucos-ii/)
- Peripherals
	- 7 segments LED: MT03641BR
	- Temperature and humidity sensor: DHT11

## Discription

A simple thermometer that gets data from DHT11 and displays them on 7 segments.

This project uses the standard library instead of HAL. uC/OS-II kernel source code is integrated for handling multiple tasks.

