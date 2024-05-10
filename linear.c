#include <stdio.h>
#include <stdint.h>
#include "encoder_linear.h"
// // CRC校验函数
// uint16_t crc16(uint8_t *data, int length) {
//     uint16_t crc = 0xFFFF;
//     for (int i = 0; i < length; i++) {
//         crc ^= data[i];
//         for (int j = 0; j < 8; j++) {
//             if (crc & 1) {
//                 crc >>= 1;
//                 crc ^= 0xA001;
//             } else {
//                 crc >>= 1;
//             }
//         }
//     }
//     return crc;
// }

// // 封装的CRC校验函数调用
// uint16_t calculateCRC(uint8_t *data, int length) {
//     return crc16(data, length);
// }

// 解析应答帧函数
int LinearFrame(DATA *data, char *frame, int len)
{
    // int i;
    // for(i = 0 ; i < len ; i++)
    // {
    //     printf("%#x ",frame[i]);
    // }
    // printf("\n");
    // static double lastValidValue = 0.0;  // 用于保存上一次有效的测量结果
    // 检查数据长度和起始字节
    if (len != 9 || frame[0] != 0x01 || frame[1] != 0x03)
    {
        return -1; // 返回上一次有效的值
    }

    // 检查CRC
    uint16_t receivedCRC = (frame[len - 2]) | (frame[len - 1] << 8); // 最后两个字节是CRC，低位在前，高位在后
    if (calculateCRC(frame, len) != receivedCRC)
    {
        return -1; // 返回上一次有效的值
    }
    // 检查符号位，并清除整数部分的符号位
    // int sign = (frame[3] & 0x80) ? -1 : 1; // 判断符号
    // frame[3] &= 0 << 15;                   // 清除符号位

    // 提取整数部分和小数部分
    int integerPart = (frame[3] << 8) | frame[4];
    int decimalPart = (frame[5] << 8) | frame[6];

    // 将整数和小数部分结合，并应用符号
    double result = (integerPart + decimalPart / 65536.0);
    data->lenth = result;
    // printf("linear:%.2f\n",data->lenth);
    // 更新上一次有效的测量结果
    return 0;
}

// int main() {
//     // 示例帧数据，假设这是一个负数值
//     uint8_t responseFrame[] = {0x01, 0x03, 0x04, 0x80, 0x07, 0xAD, 0xA7, 0x5f, 0x18};

//     // 解析帧数据
//     double value = LinearFrame(responseFrame, sizeof(responseFrame));
//     printf("解析得到的数值为: %.3f\n", value);

//     return 0;
// }

