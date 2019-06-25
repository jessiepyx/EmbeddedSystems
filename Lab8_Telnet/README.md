# Lab 8: Telnet

Telnet server that displays client messages on LED matrix and writes logs to an SQLite database.

- Raspberry Pi 3B+
- SQLite3
- LED matrix and its wiring is the same as in [Lab7](https://github.com/jessiepyx/EmbeddedSystems/tree/master/Lab7_Matrix)

## Install matrix driver

Register device ```/dev/matrix``` by installing [driver.ko](https://github.com/jessiepyx/EmbeddedSystems/tree/master/Lab7_Matrix/Ubuntu/modules) of Lab7:

```
$ sudo insmod ../Lab7_Matrix/Ubuntu/modules/driver.ko
$ sudo chown pi:pi /dev/matrix
```

## Test write() to matrix

[*write_matrix.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab8_Telnet/write_matrix.c)

## Establish a server

[*server.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab8_Telnet/server.c)

To run client locally:

```
$ telnel localhost 8080
```

## Add database to the server

[*server2.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab8_Telnet/server2.c)

- Using SQLite C/C++ API
- Server logs are stored in Tabel *Log* of *test.db*

Compile and run the server:

```
$ gcc server2.c -o server2 -lsqlite3
$ ./server2 &
```

Compile and run [*read_log.c*](https://github.com/jessiepyx/EmbeddedSystems/blob/master/Lab8_Telnet/read_log.c) to print the logs. Alternatively, run:

```
$ sqlite3
sqlite> ATTACH DATABASE 'test.db' as test;
sqlite> SELECT * FROM log;
```
