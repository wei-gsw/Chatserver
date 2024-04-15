#ifndef SOCKETCONN_H
#define SOCKETCONN_H
#include <iostream>
#include <string>
// socket网络编程
#include<string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <atomic>
using namespace std;

// socket连接函数，返回socketfd
int socketconn(char *ip, uint16_t port)
{
    // 创建client端的socket
    // 返回套接字描述符用于，建立连接，数据传输，关闭连接
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        perror("socket error");
        exit(-1);
    }

    // 填写client需要连接的server信息ip+port
    sockaddr_in serveraddr;
    // 先清空地址
    memset(&serveraddr, 0, sizeof(sockaddr_in));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(ip);

    // client连接服务器
    int cfd = connect(clientfd, (sockaddr *)&serveraddr, sizeof(serveraddr));
    if (cfd == -1)
    {
        perror("connect error");
        close(clientfd);
        exit(-1);
    }
   
    return clientfd;
}

#endif