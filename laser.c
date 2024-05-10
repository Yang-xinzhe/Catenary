#include "laser.h"

// 错误码和对应的错误信息
typedef struct {
    char *errorCode;
    char *errorMessage;
} ErrorLookup;

// 定义错误码和错误消息的数组
ErrorLookup errorLookupTable[] = {
    {"Er01!", "输入电压太低"},
    {"Er02!", "内部错误，重新开机无果后返修"},
    {"Er03!", "环境温度太低(< -20℃)"},
    {"Er04!", "环境温度太高(> +40℃)"},
    {"Er05!", "测量目标超过量程"},
    {"Er06!", "错误的测量结果，再次测量"},
    {"Er07!", "环境光太强，遮光处理"},
    {"Er08!", "反射信号太弱，加强被测目标反光"},
    {"Er09!", "反射信号太强，降低被测目标反光"},
    {"Er10!", "硬件错误 1，重新开机无果后返修"},
    {"Er11!", "硬件错误 2，重新开机无果后返修"},
    {"Er12!", "硬件错误 3，重新开机无果后返修"},
    {"Er13!", "硬件错误 4，重新开机无果后返修"},
    {"Er14!", "硬件错误 5，重新开机无果后返修"},
    {"Er15!", "模块抖动过大，稳定测距模块后重新测量"},
    {"Er16!", "硬件错误 6，重新开机无果后返修"},
    {"Er17!", "硬件错误 7，重新开机无果后返修"},
    {NULL, NULL}  // 终结符
};


int uart_init_laser(int fd, struct termios options)
{
    // 配置串口参数
    tcgetattr(fd, &options);
    cfsetispeed(&options, B19200); // 设置输入波特率
    cfsetospeed(&options, B19200); // 设置输出波特率
    options.c_cflag |= (CLOCAL | CREAD); // 本地连接和接收使能
    options.c_cflag &= ~PARENB; // 无奇偶校验位
    options.c_cflag &= ~CSTOPB; // 停止位为1
    options.c_cflag &= ~CSIZE; // 清除数据位掩码
    options.c_cflag |= CS8; // 数据位为8
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // 非规范模式、关闭回显、关闭信号字符处理
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // 关闭软件流控制
    options.c_oflag &= ~OPOST; // 关闭输出处理
    tcsetattr(fd, TCSANOW, &options); // 应用配置

	return 0;
}

// 解析激光传感器返回的数据
void parseLaserSensorResponse(DATA *data, const char *recv_buf)
{
    if (recv_buf == NULL || recv_buf[0] == '\0')
    {
        printf("Received buffer is empty. Skipping data parsing.\n");
        return;
    }
    int foundError = 0;
    int i;
    for (i = 0; errorLookupTable[i].errorCode != NULL; ++i)
    {
        if (strstr(recv_buf, errorLookupTable[i].errorCode))
        {
            printf("Error Detected: %s - %s\n", errorLookupTable[i].errorCode, errorLookupTable[i].errorMessage);
            foundError = 1;
            break;
        }
    }

    // 如果没有找到错误码，则尝试解析距离和信号质量
    if (!foundError)
    {
        char *distanceStart = strstr(recv_buf, ": ") + 2;  // 定位到距离数据的起始位置
        char *signalQualityStart = strstr(distanceStart, ",") + 1;  // 定位到信号质量数据的起始位置

        // 验证数据格式
        if (distanceStart != NULL && signalQualityStart != NULL)
        {
            char distanceStr[20], signalQualityStr[4];
            strncpy(distanceStr, distanceStart, signalQualityStart - distanceStart - 1);
            distanceStr[signalQualityStart - distanceStart - 1] = '\0';

            strncpy(signalQualityStr, signalQualityStart, 3);
            signalQualityStr[3] = '\0';

            double distance = atof(distanceStr);
            //int quality = atoi(signalQualityStr);

            // lastValidDistance = distance;  // 更新最后一次成功读取的距离
            data->distance = distance;
        }
    }
    else {
        // 打印上一次有效的距离值
        printf("Last valid distance: %.3f meters\n", data->distance);
    }
}

void open_laser(int fd)//开启激光，激光开启后模块返回字符串:",OK!"
{
    char ch = 'O';
    write(fd,&ch,sizeof(ch));
}

void close_laser(int fd) //关闭激光，激光关闭后模块返回字符串",OK!"
{
    char ch = 'C';
    write(fd,&ch,sizeof(ch));
}

void read_state(int fd) //读取模块状态，模块返回状态字符串: "18.0'C, 2.7V"
{
    char ch = 'S';
    write(fd,&ch,sizeof(ch));
}

void auto_messure(int fd) //启动自动测量过程
{
    char ch = 'D';
    write(fd,&ch,sizeof(ch));
}

void slow_messure(int fd) //启动慢速测量过程
{
    char ch = 'M';
    write(fd, &ch, sizeof(ch));
}

void fast_messure(int fd) //启动快速测量过程
{
    char ch = 'F';
    int num = write(fd, &ch, sizeof(ch));
    printf("laser write:%d:%c\n",num,ch);
}

void check_version(int fd) //查询模块版本信息
{
    char ch = 'V';
    write(fd, &ch, sizeof(ch));
}

void close_module(int fd) //关闭模块，关闭模块的同时需要将 PWR_ON 引脚拉低
{
    char ch = 'X';
    write(fd, &ch, sizeof(ch));
}

int laser_data(DATA *data)
{
    struct termios options_laser;
    int fd_laser = open("/dev/ttymxc1",O_RDWR | O_NOCTTY);
    if (fd_laser < 0)
    {
        printf("open failed!\n");
        return 0;
    }
    uart_init_laser(fd_laser, options_laser);
    char recv_buf[100] = {0};
    slow_messure(fd_laser);
    sleep(1);
    int ret;
    if ((ret = read(fd_laser, recv_buf, sizeof(recv_buf) - 1)) > 0)
    {
        recv_buf[ret] = '\0';
        parseLaserSensorResponse(data, recv_buf);
    }

    close(fd_laser);
    return 0;
}