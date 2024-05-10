#include "encoder_linear.h"

// CRC校验函数
uint16_t crc16(char *data, int length) {
    uint16_t crc = 0xFFFF;
    int i , j;
    for (i = 0; i < length; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 1) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// 封装的CRC校验函数调用
uint16_t calculateCRC(char *data, int length) {
    return crc16(data, length - 2);  // Exclude CRC bytes from CRC calculation
}

// 数据包解析函数
int parseEncoderPacket(DATA *data,char *packet, int length) 
{
    // 检查数据包长度至少为9字节
    if (length < 9) {
        printf("Error: Packet too short.\n");
        return -1;
    }

    // 检查前三个字节
    if (packet[0] != 0x02 || packet[1] != 0x03 || packet[2] != 0x04) {
        printf("Error: Invalid packet header.\n");
        return -1;
    }

    // 提取并计算CRC校验
    uint16_t receivedCRC = (packet[length - 2]) | (packet[length - 1] << 8);
    uint16_t calculatedCRC = calculateCRC(packet, length);

    if (receivedCRC != calculatedCRC) {
        printf("Error: CRC check failed. Received: %04X, Calculated: %04X\n", receivedCRC, calculatedCRC);
        return -1;  // CRC校验失败，停止解析
    }

    // CRC校验通过后，解析数据
    uint16_t dataValue = (packet[3] << 8) | packet[4];
    double angle = (dataValue * 36000.0) / 65536.0;  // 先乘以360，然后再除以1024
    angle /= 100;  // 使用round函数进行四舍五入到小数点后两位
    data->angle = angle;
    // printf("CRC check passed. Data value: %d,angle = %.2f\n", dataValue,angle);

    return 0;  // 解析成功
}

// int main() {
//     uint8_t packet[] = {0x01, 0x03, 0x04, 0x01, 0xD7, 0x02, 0x09, 0x8A, 0x91};

//     if (parseEncoderPacket(packet, sizeof(packet)) == 0) {
//         printf("Packet parsed successfully.\n");
//     } else {
//         printf("Failed to parse packet.\n");
//     }

//     return 0;
// }

