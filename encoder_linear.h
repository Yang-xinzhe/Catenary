#ifndef __ENCODER_H_
#define __ENCODER_H_
#include <stdio.h>
#include <stdint.h>
#include "list.h"
#include "mailbox.h"
#include "queue.h"
uint16_t crc16(char *data, int length);
uint16_t calculateCRC(char *data, int length);
int parseEncoderPacket(DATA *data,char *packet, int length);
int LinearFrame(DATA *data,char *frame, int len) ;

#endif