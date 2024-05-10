#ifndef __ALL_SENSOR_H_
#define __ALL_SENSOR_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include "encoder_linear.h"
#include "tilt.h"

int uart_init_all(int fd, struct termios options);
extern int parseAllSensor(DATA *data);



#endif