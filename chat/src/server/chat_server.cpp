#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json.hpp"
#include <functional>
#include <string>
#include <boost/bind/bind.hpp>
using json = nlohmann::json;
using namespace std;
using namespace boost;

ChatServer::ChatServer(EventLoop *loop,               // 事件循环 ==epoll
                       const InetAddress &listenaddr, // ip+port
                       const string &name)            // server名字
    : _loop(loop), _server(loop, listenaddr, name)
// 构造函数 传入参数 用来给tcpserver赋值
{
    // 给服务器注册用户连接的 创建和断开回调
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));
    // 给服务器注册用户连接的 读写回调
    _server.setMessageCallback(std::bind(&ChatServer::onMassage, this, _1, _2, _3));
    // 设置线程数量
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

// 专门处理用户的连接创建和断开
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:online " << endl;
    }
    else
    {
        cout << "连接已断开" << endl;
        // 断开连接,从map表里删除，改变状态
        ChatService::instance()->clientclose(conn);

        conn->shutdown();
        //_loop->quit();
    }
}

// 读写时 调用
void ChatServer::onMassage(const TcpConnectionPtr &conn, // 连接
                           Buffer *buffer,               // 缓冲区
                           Timestamp time)               // 接受时间
{
    string buf = buffer->retrieveAllAsString();
    // 答应收到的data 和 时间
    cout << "recv data: " << buf << " time: " << time.toFormattedString() << endl;
    // 数据的反序列化
    json js = json::parse(buf);
    // 目的：达到完全解耦网络模块server和业务模块service
    // 通过js["msgid"]来获取业务模块的 处理器 handler 来 得到conn js time
    auto msgHandler = ChatService ::instance()->gethandler(js["msgid"].get<int>());
    // 回调消息绑定好的 处理器 来执行相应的业务
    msgHandler(conn, js, time);
}



// 编译                                连接网络库
// g++ muduo_server.cpp - o server - lmuduo_net - lmuduo_base - lpthread
// {"msgid":1,"id":24,"password":"gssad23456"}
/*  LOGIN_ACK = 1, // 登录消息1
    LOGIN_MSG_ACK, // 登录响应消息2
    REG_ACK,       // 注册消息3
    REG_MSG_ACK    // 注册相应消息4
    ONE_CHAT_MSG  //聊天消息5
};*/
