#ifndef __TILT_H_
#define __TILT_H_
#include <stdio.h>
#include <stdint.h> 
#include "list.h"
#include "mailbox.h"
#include "queue.h"

extern double bcdToDec(uint8_t bcd) ;
extern int parseTiltFrame(DATA *data,char *frame, int len);

#endif