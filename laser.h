#ifndef __LASER_H_
#define __LASER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include "list.h"
#include "mailbox.h"
#include "queue.h"

extern void parseLaserSensorResponse(DATA *data, const char *recv_buf);
extern void open_laser(int fd);
extern void close_laser(int fd);
extern void read_state(int fd);
extern void auto_messure(int fd);
extern void slow_messure(int fd);
extern void fast_messure(int fd);
extern void check_version(int fd);
extern void close_module(int fd);
extern int laser_data(DATA *data);
#endif