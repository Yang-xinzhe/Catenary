#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "list.h"
#include "mailbox.h"
#include "queue.h"
#include "laser.h"
#include "all_sensor.h"
MBS *mbs;


void *uart2_th(void *arg)
{
    static DATA data = {0,0,0,0,0,0};
    while(1)
    {
        laser_data(&data);
        sleep(1);
        send_msg(mbs,"data",&data);
    }
}

void *uart3_th(void *arg)
{
    static DATA data = {0,0,0,0,0,0};
    while(1)
    {
        parseAllSensor(&data);
        sleep(1);
        send_msg(mbs,"data",&data);
    }
}

void *data_th(void *arg)
{
    static DATA data_get  = {0,0,0,0,0,0};
    static DATA data_save = {0,0,0,0,0,0};
    while(1)
    {
        int uart2 = 0;
        int uart3 = 0;
        char sendname[128] = {0};
        while(1)
        {
            recv_msg(mbs,sendname,&data_get);
            if(strcmp(sendname,"uart2") == 0)
            {
                uart2 = 1;
                data_save.distance = data_get.distance;
            }
            else if(strcmp(sendname,"uart3") == 0)
            {
                uart3 = 1;
                data_save.angle = data_get.angle;
                data_save.lenth = data_get.lenth;
                data_save.Xaxis = data_get.Xaxis;
                data_save.Yaxis = data_get.Yaxis;
                data_save.temp = data_get.temp;
            }
            if(uart2 == 1 && uart3 == 1)
            {
                send_msg(mbs,"save",&data_save);
                printf("距离:%.4f\n,激光传感器角度:%.2f\r直线:%.3f\nX轴:%.2f Y轴:%.2f 传感器温度:%.2f\n",
                data_save.distance,data_save.angle,data_save.lenth,data_save.Xaxis,data_save.Yaxis,data_save.temp);
                break;
            }
        }
    }
}

void *save_data(void *arg)
{
    // 用于存储当前操作的日期，以便检查是否是新的一天
    static char current_date[11] = {0};
    static DATA data;
    while (1)
    { // 持续运行，模拟持续的数据接收和写入
        time_t now = time(NULL);
        struct tm *tm_now = localtime(&now);
        char time_str[100] = {0};
        char date_str[11] = {0};
        char filename[128] = {0};

        // 获取当前时间和日期
        strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_now);
        strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_now);

        // 如果日期改变了，更新当前日期并检查是否需要创建新文件
        if (strcmp(current_date, date_str) != 0)
        {
            strcpy(current_date, date_str);
            sprintf(filename, "./data_%s.csv", date_str);
            // 创建新文件
            int fd_csv = open(filename, O_RDWR | O_CREAT | O_APPEND, 0666);
            if (fd_csv < 0)
            {
                perror("Failed to open/create csv file");
                exit(1);
            }

            // 写入列标题
            char header[] = "时间,激光距离,激光倾角,直线长度,X轴倾角,Y轴倾角,设备温度\n";
            write(fd_csv, header, strlen(header));
            close(fd_csv);
        }

        // 打开文件以追加数据
        sprintf(filename, "./data_%s.csv", date_str);
        int fd_csv = open(filename, O_RDWR | O_CREAT | O_APPEND, 0666);
        if (fd_csv < 0)
        {
            perror("Failed to open/create csv file");
            exit(1);
        }

        char sendname[128] = {0};
        recv_msg(mbs,sendname,&data);

        // 构建带时间的数据行
        char buf[128] = {0};
        sprintf(buf, "%s,%.4f,%.2f°,%.3f毫米,%.2f,%.2f,%.2f\n", time_str, data.distance, data.angle, data.lenth, data.Xaxis, data.Yaxis, data.temp);
        write(fd_csv, buf, strlen(buf));

        close(fd_csv);

        // 稍作延迟以模拟现实世界的数据接收速度
        sleep(1);
    }
}

int main(int argc, char const *argv[])
{
    mbs = create_mail_box_system();
    register_to_mail_system(mbs, "uart2", uart2_th);
    register_to_mail_system(mbs, "uart3", uart3_th);
    register_to_mail_system(mbs, "data", data_th);
    register_to_mail_system(mbs, "save", save_data);

    wait_all_end(mbs);
    destroy_mail_box_system(mbs);
    return 0;
}
