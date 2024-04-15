#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include "json.hpp"
#include <unordered_map>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include "usermodel.hpp"
#include <mutex>
#include "friendmodel.hpp"
#include "offlinemsgmodel.hpp"
#include "redis.hpp"
#include "groupmodle.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann ::json;

using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

class ChatService
{

public:
    // 获取单例模式的 接口函数
    static ChatService *instance();
    // 处理登录业务login
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注册业务reg
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 在业务模块输入id得到对应的方法
    MsgHandler gethandler(int msgid);
    // 处理客户端 异常退出
    void clientclose(const TcpConnectionPtr &conn);
    // 服务器异常，业务重组方法
    void reset();
    // 一对一聊天
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 添加好友业务
    void addfriend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //注销
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);

private:
    // 私有化构造函数
    ChatService();
    // 用map容器储存消息id 和 对应的业务处理方法
    unordered_map<int, MsgHandler> _msghandlermap;

    // 储存在线online用户的 通信连接
    unordered_map<int, TcpConnectionPtr> _userconnmap;
   //互斥锁保证userconnmap线程安全
    mutex _connmutex;

    // 数据操作类对象  通过对象对其他cpp文件操纵
    //user数据操作对象
    UserModel _usermodel;
    // 离线消息操作类对象
    offlineMsgModel offlinemsgmodel;
    // friendmodel操作类对象
    friendModel _friendmodel;
    //group操作类对象
    GroupModel _groupmodel;

    // redis操作对象
    Redis _redis;
};

#endif