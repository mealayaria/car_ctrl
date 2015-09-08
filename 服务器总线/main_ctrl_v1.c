#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <sys/socket.h>

#define UNIX_ADDR "/tmp/UNIX.domain"

int car_ctrl(char c);

int main(int argc, const char *argv[])
{
    int listenfd, clifd;
    struct sockaddr_un serv_addr, cli_addr;
    socklen_t len = sizeof(serv_addr);
    char buf = 0;

    //更改权限防止php用户组无法读写UNIX.domain
    mode_t pre_umask = umask(0);

    listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket");
        unlink(UNIX_ADDR);
        exit(1);
    }

    unlink(UNIX_ADDR);
    bzero(&serv_addr, len);
    bzero(&cli_addr, len);
    serv_addr.sun_family = AF_LOCAL;
    strcpy(serv_addr.sun_path, UNIX_ADDR);

    if (bind(listenfd, (struct sockaddr *)&serv_addr, len) == -1) {
        perror("bind");
        unlink(UNIX_ADDR);
        exit(1);
    }
    umask(pre_umask);
    
    if (listen(listenfd, 2) == -1) {
        perror("listen");
        unlink(UNIX_ADDR);
        exit(1);
    }
    
    int epfd = epoll_create(2);
    struct epoll_event ev, events;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("epoll_add");
        unlink(UNIX_ADDR);
        exit(1);
    }

	while (1) {
        int nfds = epoll_wait(epfd, &events, 2, 120000);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(1);
        }

        //2分钟超时退出
        if (nfds == 0) 
            break;

        //与网页端建立通信
        if ((events.data.fd == listenfd)) {
            clifd = accept(listenfd, (struct sockaddr *)&cli_addr, &len);
            if (clifd == -1) {
                perror("accept");
                exit(1);
            }
            ev.data.fd = clifd;
            ev.events = EPOLLIN | EPOLLRDHUP;
            if (epoll_ctl(epfd, EPOLL_CTL_ADD, clifd, &ev) == -1) {
                perror("epoll_ctl");
                exit(1);
            }
        }

        //读取网页端数据
        if (((events.events & EPOLLIN) == EPOLLIN) && 
                (events.data.fd == clifd)) {
            recv(clifd, &buf, 1, 0);
            //printf("%s\n", buf);
            int flg = car_ctrl(buf);
            buf = 0;
            if (flg <= 0) 
                continue;
            else {
                if (epoll_ctl(epfd, EPOLL_CTL_DEL, clifd, &ev) == -1) {
                    perror("epoll_del");
                    exit(1);
                }
                shutdown(listenfd, SHUT_RDWR);
                return 0;
            }
        }
	}
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, listenfd, &ev) == -1) {
        perror("epoll_del");
        exit(1);
    }
    close(listenfd);

	return 0;
}

int car_ctrl(char c)
{
    switch(c) {
        //左转
		case '4':system("./gpio 4"); break;
        //右转
        case '6':system("./gpio 6"); break;
        //前进
        case '8':system("./gpio 8"); break;
        //后退
        case '2':system("./gpio 2"); break;
        //减速
        case '0':system("./pwm 200 20"); break;
        //加速
        case '1':system("./pwm 200 80"); break;
        //启动
        case '5':system("./gpio 8"); 
                system("./pwm 200 50"); break;
        //退出
        case 'q':system("./gpio q");
                system("./pwm 200 0");  return 1;
        default:return -1;
    }
    return 0;

}
