#include "chatserver.hpp"
#include <iostream>
#include"chatservice.hpp"
#include<signal.h>
using namespace std;

// 处理服务器ctrl+c 异常结束后 ，业务重置
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGINT, resetHandler);
    if (argc < 3)
    {
        cerr << "command invalid! example: ./Server 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    EventLoop loop; // epoll
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    // 用阻塞方式等待新用户连接，等待读写等
    loop.loop();

    return 0;
}
