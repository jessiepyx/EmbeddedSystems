# Lab 7: Matrix

Drive an 8x8 LED matrix with the wiringPi library and a kernel module respectively.

<img src=https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab7_Matrix/wiring.jpg width=500>

- MAX7219 chip
- Raspberry Pi 3B+

Wiring:

- DIN: GPIO0 (BCM17)
- CLK: GPIO1 (BCM18)
- CS:  GPIO2 (BCM27)

## With wiringPi

- wiringPi:

```
$ git clone git://git.drogon.net/wiringPi
$ cd wiringPi
$ ./build
```

- [*matrix.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab7_Matrix/raspi/matrix.c):

```
$ gcc matrix.c -lwiringPi
$ ./a.out
```

- Characters will be displayed in ASCII order.

## With kernel module

- Cross-compile [*driver.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab7_Matrix/Ubuntu/modules/driver.c) in Ubuntu.

- Run scripts [*driver.sh*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab7_Matrix/raspi/driver.sh) in Raspberry Pi.

- Number 0-9 will be displayed.

