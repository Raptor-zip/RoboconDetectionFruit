#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyUSB1"
#define _POSIX_SOURCE 1 /* POSIX 準拠のソース */
#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE;

main()
{
    int fd, c, res;
    struct termios oldtio, newtio;
    char buf[255];

    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(MODEMDEVICE);
        exit(-1);
    }

    tcgetattr(fd, &oldtio); /* 現在のポート設定を待避 */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0; /* キャラクタ間タイマは未使用 */
    newtio.c_cc[VMIN] = 0;  /* 5文字受け取るまでブロックする */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

    while (STOP == FALSE)
    {                            /* 入力ループ */
        res = read(fd, buf, 18); /* 5 文字入力されたら戻る */
        if (res > 0)
            printf("%d,%d,%d,%d  res::%d\n", buf[0], buf[1], buf[2], buf[3], res);
        // buf[res] = 0;             /* printf を使うために文字列終端をセット */
        // printf(":%s:%d\n", buf, res);
        // if (buf[0] == 'z')
        //     STOP = TRUE;
    }
    tcsetattr(fd, TCSANOW, &oldtio);
}