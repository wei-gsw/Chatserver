#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <functional>
#include <string>


using namespace std;
using namespace muduo;
using namespace muduo::net;

/*
1.组合TcpServer对象
2.创建EvetLoop事件循环对象的 指针
3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
4.在当前服务器类的构造函数里，注册处理连接和 处理读写的 事件回调后函数
5.设置合适的服务器线程数量，用已经封装好的函数，muduo会自己分配线程

*/

class ChatServer
{
public:
    // 初始化聊天对象
    ChatServer(EventLoop *loop,               // 事件循环 ==epoll
               const InetAddress &listenaddr, // ip+port
               const string &name);           // server名字

    // 启动服务
    void start();

private:
    // 专门处理用户的连接创建和断开
    void onConnection(const TcpConnectionPtr &conn);

    // 读写时 调用
    void onMassage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buffer,               // 缓冲区
                   Timestamp time);              // 接受时间

    TcpServer _server; // #1 组合muduo网络库，实现服务器功能的类对象
    EventLoop *_loop;  // #2 epoll 事件循环的指针
};

#endif