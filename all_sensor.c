#include "all_sensor.h"

int uart_init_all(int fd, struct termios options)
{
	// 配置串口参数
	tcgetattr(fd, &options);
	cfsetispeed(&options, B9600); // 设置输入波特率
	cfsetospeed(&options, B9600); // 设置输出波特率
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

int parseAllSensor(DATA *data)
{
    struct termios options_all;
    int fd_all = open("/dev/ttymxc2", O_RDWR | O_NOCTTY | O_NONBLOCK); // 添加 O_NONBLOCK 使文件描述符非阻塞
    if (fd_all < 0)
    {
        perror("open failed");
        return 0;
    }
    uart_init_all(fd_all, options_all); // 假设这个函数设置了termios结构体

    char tilt_buf[5] = {0x68, 0x04, 0x00, 0x04, 0x08};
    char linear_buf[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
    char encoder_buf[8] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x38};

    char recv_buf[100] = {0};
    int num = write(fd_all, tilt_buf, sizeof(tilt_buf));
    // printf("tilt write %d\n", num);
    usleep(100000);  // 使用 usleep 替换 sleep，使用更短的等待时间

    num = read(fd_all, recv_buf, sizeof(recv_buf));
    if (num > 0)
    {
        parseTiltFrame(data, recv_buf,14);
    }

    num = write(fd_all, linear_buf, sizeof(linear_buf));
    // printf("linear write %d\n", num);
    usleep(100000);

    num = read(fd_all, recv_buf, sizeof(recv_buf));
    if (num > 0)
    {
        LinearFrame(data, recv_buf, 9);
    }

    num = write(fd_all, encoder_buf, sizeof(encoder_buf));
    // printf("encoder write %d\n", num);
    usleep(100000);

    num = read(fd_all, recv_buf, sizeof(recv_buf));
    if (num > 0)
    {
        parseEncoderPacket(data, recv_buf, 9);
    }

    close(fd_all);
    return 0;
}