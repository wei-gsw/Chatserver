#include "chatservice.hpp"
#include "public.hpp"
#include "usermodel.hpp"
#include "user.hpp"
#include <muduo/base/Logging.h>
#include <iostream>
#include"groupmodle.hpp"
#include "friendmodel.hpp"
#include "offlinemsgmodel.hpp"
#include <vector>
#include"redis"
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann ::json;

// 获取单例模式的 接口函数
ChatService *ChatService ::instance()
{
    static ChatService service;
    // 返回一个创建好的对象
    return &service;
}

// 初始化  构造函数   用来注册消息 处理的对应的回调函数
ChatService ::ChatService()
{                                                                                                  // 就是在map容器插入相应的键值对
    _msghandlermap.insert({LOGIN_ACK, std::bind(&ChatService::login, this, _1, _2, _3)});          // 1
    _msghandlermap.insert({REG_ACK, std::bind(&ChatService::reg, this, _1, _2, _3)});              // 2
    _msghandlermap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});     // 5 一对一聊天消息
    _msghandlermap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addfriend, this, _1, _2, _3)}); // 6添加好友业务
    _msghandlermap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});//7创建群组
    _msghandlermap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});//8添加群组
    _msghandlermap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});//9群组聊天
    _msghandlermap.insert({LOGINOUT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});  // 10注销
                                                                                                 // 连接redis服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

// 获取msgid对应的 处理器（处理事件的方法）
MsgHandler ChatService::gethandler(int msgid)
{
    // 用find查找
    auto it = _msghandlermap.find(msgid);
    // 没有找到打印错误日志
    if (it == _msghandlermap.end())
    {
        // 返回一个空操作的处理器  用lamda表达式
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        { LOG_ERROR << "msgid: " << msgid << " can find handler! "; };
    }
    else
    {
        return _msghandlermap[msgid];
    }
}

// 处理登录业务login
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"];
    string password = js["password"];

    User user = _usermodel.query(id);
    // 登录有三种情况
    if (user.getid() == id && user.getpassword() == password)
    {
        // 1.用户已经登录，不允许login
        if (user.getstate() == "online")
        {
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "用户已经登录，不允许重复登录";

            // cout << "用户已经登录，不允许重复登录!" << endl;

            conn->send(response.dump());
        }
        else
        {
            {
                // 登录成功，储存用户连接信息 用map表，要线程安全
                lock_guard<mutex> lock(_connmutex);
                _userconnmap.insert({id, conn});
            }

            // 2.登录成功    还要更新用户state信息
            user.setstate("online");
            _usermodel.updateState(user);

            
            // id用户登录成功后，向redis订阅channel(id)
            _redis.subscribe(id);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getid();
            response["name"] = user.getname();

            // 查询用户离线消息 返回
            vector<string> vec = offlinemsgmodel.query(id);
            if (!vec.empty())
            {
                response["offlinemessage"] = vec;
                offlinemsgmodel.remove(id);
            }

            // 查询好友信息返回
            vector<User> vec2 = _friendmodel.query(id);
            if (!vec2.empty())
            {
                vector<string> vec3;
                for (User &user : vec2)
                {
                    json js;
                    js["id"] = user.getid();
                    js["name"] = user.getname();
                    js["state"] = user.getstate();

                    vec3.push_back(js.dump());
                }
                response["friends"] = vec3;
            }

            conn->send(response.dump());
        }
    }
    else
    {
        // 找不到用户或密码错误
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "找不到用户或密码错误";

        //  cout << "找不到用户或密码错误!" << endl;

        conn->send(response.dump());
    }
}

// 处理注册业务reg
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string password = js["password"];

    User user;
    user.setname(name);
    user.setpassword(password);
    bool state = _usermodel.insert(user);
    if (state)
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["id"] = user.getid();
        response["errno"] = 0;

        LOG_INFO << "注册成功!";

        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;

        LOG_INFO << "注册失败!";

        conn->send(response.dump());
    }
}

// 一对一聊天
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["to"].get<int>();
    // 对连接表操作 要加锁保证线程安全
    {
        lock_guard<mutex> lock(_connmutex);
        auto it = _userconnmap.find(toid);

        if (it != _userconnmap.end())
        {
            // toid在线，服务器 转发消息
            it->second->send(js.dump());
            return;
        }
    }

    // toid不在线 把消息保存至 offlinemessage数据库
    offlinemsgmodel.insert(toid, js.dump());
}

// 添加好友业务
void ChatService::addfriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 添加到好友列表
    _friendmodel.insert(userid, friendid);
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 创建新创建的 群组信息
    Group group(-1, name, desc);
    if (_groupmodel.createGroup(group))
    {
        // 存储群组 创建人信息
        _groupmodel.addGroup(userid, group.getid(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();

    _groupmodel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> uservec = _groupmodel.queryGroupUsers(userid, groupid);
    for (int id : uservec)
    {
        lock_guard<mutex> lock(_connmutex);
        auto it = _userconnmap.find(id);
        if (it != nullptr)
        {
            it->second->send(js.dump());
        }
        else
        {
            offlinemsgmodel.insert(id, js.dump());
        }
    }
}

void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connmutex);
        auto it = _userconnmap.find(userid);
        if(it!=_userconnmap.end())
        {
            _userconnmap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid);
}

// 处理客户端 异常退出
void ChatService::clientclose(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connmutex);
        // 去找这个要退出的 conn
        for (auto it = _userconnmap.begin(); it != _userconnmap.end(); it++)
        {
            if (it->second == conn)
            {
                // 从map删除
                user.setid(it->first);
                _userconnmap.erase(it);
                break;
            }
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(user.getid());

    // 更新用户状态信息
    if (user.getid() != -1)
    {
        user.setstate("offline");
        _usermodel.updateState(user);
    }
}

// 服务器异常，业务重组方法
void ChatService::reset()
{
    // 把online用户全部改成 offline
    _usermodel.resetState();
}

// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(_connmutex);
    auto it = _userconnmap.find(userid);
    if (it != _userconnmap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    
    offlinemsgmodel.insert(userid, msg);
}