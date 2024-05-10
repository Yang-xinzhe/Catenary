#include <stdio.h>
#include <stdint.h>
#include "tilt.h"

double bcdToDec(uint8_t bcd) 
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

int parseTiltFrame(DATA *data,char *frame,int len) 
{
    int i = 0;
    // printf("tile:");
    // for(i = 0 ; i < len ; i++)
    // {    
    //     printf("%#x ",frame[i]);
    // }
    // printf("\n");
    uint8_t checksum = 0;

    // 校验帧头和计算校验和
    frame[1] = 0x0d;
    // printf("after:");
    // for(i = 0 ; i < len ; i++)
    // {    
    //     printf("%#x ",frame[i]);
    // }
    // printf("\n");


    for (i = 1; i < 13; i++) {
        checksum += frame[i];
    }

    // 验证帧头和校验和
    if (frame[0] != 0x68 || frame[1] != 0x0D || frame[2] != 0x00 || frame[3] != 0x84 || checksum != frame[13]) {
        return -1;  // 如果帧头或校验和不匹配，则返回上次的有效数据
    }

    // 解析X轴角度
    int sign = (frame[4] & 0x10) ? -1 : 1; // 提取符号位
    data->Xaxis = ((bcdToDec(frame[4] & (0 << 15))) * 256 + bcdToDec(frame[5])) + bcdToDec(frame[6]) / 100.0;
    data->Xaxis *= sign; // 应用符号

    // 解析Y轴角度
    sign = (frame[7] & 0x10) ? -1 : 1; // 提取符号位
    data->Yaxis = ((bcdToDec(frame[7] & (0 << 15))) * 256 + bcdToDec(frame[8])) + bcdToDec(frame[9]) / 100.0;
    data->Yaxis *= sign; // 应用符号

    // 解析温度
    sign = (frame[10] & 0x10) ? -1 : 1;
    data->temp = ((bcdToDec(frame[10] & (0 << 15))) * 256 + bcdToDec(frame[11])) + bcdToDec(frame[12]) / 100.0;
    data->temp *= sign;
    // printf("-----------tile:");
    // printf("%.2f, %.2f, %.2f\n",data->Xaxis,data->Yaxis,data->temp);
    return 0;
}

// int main() 
// {
//     uint8_t responseFrame[] = {0x68, 0x0D, 0x00, 0x84, 0x00, 0x20, 0x10, 0x10, 0x05, 0x25, 0x00, 0x50, 0x50, 0x9B};
//     SensorData data = parseFrame(responseFrame);
//     printf("X轴角度: %.2f°\n", data.x_angle);
//     printf("Y轴角度: %.2f°\n", data.y_angle);
//     printf("温度: %.1f°C\n", data.temperature);
//     return 0;
// }

