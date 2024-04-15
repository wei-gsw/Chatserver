#pragma once
#include <iostream>
#include <string>
#include "json.hpp"
#include "public.hpp"
#include <functional>
#include <map>
#include "socketconn.hpp"
using json = nlohmann ::json;
using namespace std;

// login登录业务
bool login(int clientfd, sem_t &rwsem)
{
    int id = 0;
    char pwd[50] = {0};

    cout << "input userid:";
    cin >> id;
    cin.get();
    cout << "input password:";
    cin.getline(pwd, 50);

    json js;
    js["msgid"] = LOGIN_ACK;
    js["id"] = id;
    js["password"] = pwd;
    string request = js.dump();

    g_isLoginSuccess = false;

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send login msg error:" << request << endl;
        return false;
    }

    // 等待信号量，子线程处理完登录线程后通知这里
    sem_wait(&rwsem);
    return g_isLoginSuccess;
    // return true;
}

// register注册业务
void Register(int clientfd, sem_t &rwsem)
{
    char name[20] = {0};
    char pwd[20] = {0};
    cout << "intput name:";
    cin.getline(name, 20);
    cout << "intput password:";
    cin.getline(pwd, 20);

    json js;
    js["msgid"] = REG_ACK;
    js["name"] = name;
    js["password"] = pwd;
    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send reg msg error:" << request << endl;
    }
    sem_wait(&rwsem); // 等待信号量，子线程处理完注册消息会通知
}

// quit 业务
void quit(int clientfd, sem_t &rwsem)
{
    close(clientfd);
    sem_destroy(&rwsem);
    exit(0);
}

// 功能函数commandhandler
void help(int fd = 0, string str = " "); // 显示所有支持的命令
void chat(int, string);                  // 一对一聊天
void addfriend(int, string);             // 添加好友
void creategroup(int, string);           // 创建群组
void addgroup(int, string);              // 加入群组
void groupchat(int, string);             // 群组聊天
void loginout(int, string);              // 注销

// 客户端命令说明列表
map<string, string> commandMap = {
    {"6.help\t", "\t显示所有支持的命令,格式help"},
    {"5.chat\t", "\t一对一聊天,格式chat:frendid:message"},
    {"4.addfriend", "\t添加好友,格式addfriend:frendid"},
    {"3.creategroup", "\t创建群组,格式creategroup:groupname:groupdesc"},
    {"2.addgroup", "\t加入群组,格式addgroup:groupid"},
    {"1.groupchat", "\t群组聊天,格式groupchat:groupid:message"},
    {"0.loginout", "\t注销,格式loginout"}};

// 将命令绑定到相应的函数
unordered_map<string, function<void(int, string)>> commandHandlerMap = {
    {"help", help},
    {"chat", chat},
    {"addfriend", addfriend},
    {"creategroup", creategroup},
    {"addgroup", addgroup},
    {"groupchat", groupchat},
    {"loginout", loginout}};



// 显示所有支持的命令
void help(int fd, string str)
{
    cout << "command list:" << endl;
    for (auto &p : commandMap)
    {
        cout << p.first << ":" << p.second << endl;
    }
    cout << endl;
}

// 一对一聊天
void chat(int clientfd, string str)
{
    int idx = str.find(":"); // friend：message
    if (idx == -1)
    {
        cerr << "chat command invalid!" << endl;
        return;
    }

    int friendid = atoi(str.substr(0, idx).c_str());        // friendid
    string message = str.substr(idx + 1, str.size() - idx); // message

    json js;
    js["msgid"] = ONE_CHAT_MSG;
    js["id"] = g_currentUser.getid();
    js["name"] = g_currentUser.getname();
    js["toid"] = friendid;
    js["msg"] = message;
    js["time"] = getCurrentTime(); // 获取当前时间
    string buf = js.dump();

    int len = send(clientfd, buf.c_str(), strlen(buf.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send chat message error:" << buf << endl;
    }
}

// 添加好友
void addfriend(int clientfd, string str)
{
    int friendid = atoi(str.c_str());

    json js;
    js["msgid"] = ADD_FRIEND_MSG;
    js["id"] = g_currentUser.getid();
    js["friendid"] = friendid;
    string buf = js.dump();

    int len = send(clientfd, buf.c_str(), strlen(str.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send addfriend message error:" << buf << endl;
    }
}

// 创建群组
void creategroup(int clientfd, string str)
{
    int idx = str.find(":"); // groupname:groupdesc
    if (idx == -1)
    {
        cerr << "creategroup command invalid!" << endl;
        return;
    }

    string groupname = str.substr(0, idx).c_str();            // groupname
    string groupdesc = str.substr(idx + 1, str.size() - idx); // groupdesc群组描述

    json js;
    js["msgid"] = CREATE_GROUP_MSG;
    js["id"] = g_currentUser.getid();
    js["time"] = getCurrentTime(); // 获取当前时间
    js["groupname"] = groupname;
    js["groupdesc"] = groupdesc;
    string buf = js.dump();

    int len = send(clientfd, buf.c_str(), strlen(buf.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send creategroup message error:" << buf << endl;
    }
}

// 加入群组
void addgroup(int clientfd, string str)
{
    int groupid = atoi(str.c_str());

    json js;
    js["msgid"] = ADD_FRIEND_MSG;
    js["id"] = g_currentUser.getid();
    js["groupid"] = groupid;
    string buf = js.dump();

    int len = send(clientfd, buf.c_str(), strlen(buf.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send addgroup message error:" << buf << endl;
    }
}

// 群组聊天
void groupchat(int clientfd, string str)
{
    int idx = str.find(":"); // groupid：message
    if (idx == -1)
    {
        cerr << "chat command invalid!" << endl;
        return;
    }

    int groupid = atoi(str.substr(0, idx).c_str());         // friendid
    string message = str.substr(idx + 1, str.size() - idx); // message

    json js;
    js["msgid"] = GROUP_CHAT_MSG;
    js["id"] = g_currentUser.getid();
    js["name"] = g_currentUser.getname();
    js["groupid"] = groupid;
    js["msg"] = message;
    string buf = js.dump();

    int len = send(clientfd, buf.c_str(), strlen(buf.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send groupchat message error:" << buf << endl;
    }
}

// 注销
void loginout(int clientfd, string str)
{
    json js;
    js["msgid"] = LOGINOUT_MSG;
    js["id"] = g_currentUser.getid();
    string buf = js.dump();

    int len = send(clientfd, buf.c_str(), strlen(buf.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send loginout message error:" << buf << endl;
    }else
    {
        isMainMenRunning = false;
    }
}

// 显示登录用户的基本信息
void showCurrentUserDate()
{
    cout << "=====================Login User=====================" << endl;
    cout << "CurrentUser id: " << g_currentUser.getid() << "\tUsername: " << g_currentUser.getname() << endl
         << endl;
    cout << "=====================Friend List=====================" << endl;
    if (!g_currentUserFriendList.empty())
    {
        for (User &user : g_currentUserFriendList)
        {
            cout << "id: " << user.getid() << "\tname: " << user.getname() << "\tstate: " << user.getstate() << endl;
        }
        cout << endl;
    }

    cout << "======================Group List======================" << endl;
    if (!g_currentUserGroupList.empty())
    {
        for (Group &group : g_currentUserGroupList)
        {
            cout << "id: " << group.getid() << "\tname: " << group.getname() << "\tdesc: " << group.getdesc() << endl;
            for (GroupUser &it : group.getUsers())
            {
                cout << "id: " << it.getid() << "\tname: " << it.getname() << "\tstate: " << it.getstate() << endl;
            }
        }
        cout << endl;
    }
}

// 响应登录业务
void loginResponse(json &responsejs)
{ // 失败
    if (responsejs["errno"].get<int>() != 0)
    {
        cerr << responsejs["errmsg"] << endl;
        g_isLoginSuccess = false;
    }
    // 成功
    else
    {
        // 记录登录的user
        g_currentUser.setid(responsejs["id"].get<int>());
        g_currentUser.setname(responsejs["name"]);

        // 记录当前用户好友列表信息
        if (responsejs.contains("friends"))
        {
            // 初始化
            g_currentUserFriendList.clear();
            vector<string> vec = responsejs["friends"];
            for (auto &it : vec)
            {
                json js = json::parse(it);
                User user;
                user.setname(js["name"]);
                user.setid(js["id"].get<int>());
                user.setstate(js["state"]);
                g_currentUserFriendList.push_back(user);
            }
        }

        // 记录当前用户群组列表
        if (responsejs.contains("groups"))
        {
            // 初始化
            g_currentUserGroupList.clear();
            vector<string> vec1 = responsejs["groups"];
            for (auto &it : vec1)
            {
                json groupjs = json::parse(it);
                Group group;
                group.setname(groupjs["groupname"]);
                group.setid(groupjs["id"].get<int>());
                group.setdesc(groupjs["desc"]);

                // 记录group里面的groupuser成员
                vector<string> vec2 = groupjs["users"];
                for (string &userstr : vec2)
                {
                    GroupUser user;
                    json js = json::parse(userstr);
                    Group group;
                    user.setname(js["groupname"]);
                    user.setid(js["id"].get<int>());
                    user.setstate(js["state"]);
                    user.setRole(js["role"]);

                    group.getUsers().push_back(user);
                }

                g_currentUserGroupList.push_back(group);
            }
        }

        // 显示登录用户的基本信息
        showCurrentUserDate();

        // 显示当前用户的offlinemessage，个人聊天信息和群聊信息
        if (responsejs.contains("offlinemessage"))
        {
            vector<string> vec = responsejs["offlinemessage"];
            for (string &str : vec)
            {
                json js = json::parse(str);
                if (js["msgid"].get<int>() == ONE_CHAT_MSG)
                {
                    cout << js["time"].get<string>() << "[" << js["id"] << "]" << js["name"].get<string>() << "said: " << js["msg"].get<string>() << endl;
                }
                else
                {
                    cout << "群聊消息" << js["time"].get<string>() << "[" << js["groupid"] << "]:" << js["groupname"].get<string>() << " said: " << js["msg"].get<string>() << endl;
                }
            }
        }

        g_isLoginSuccess = true;
    }
}

// 响应注册业务
void regResponse(json &js)
{
    if (js["errno"].get<int>()!= 0)
    {
        cerr << "name is already exits,error!" << endl;
    }
    else
    {
        cout << "register success,id: " << js["id"] << ",don't forget it!" << endl;
    }
}


// login进入后菜单
void Menu(int clientfd, bool &isMainMenRunning)
{
    help();
    char buffer[1024] = {0};
    while (isMainMenRunning)
    {
        cin.getline(buffer, 1024);
        string commandbuf(buffer);
        string command;                 // 储存命令
        string str;                     // 储存：后的字段
        int idx = commandbuf.find(":"); // 返回：的下表
        if (idx == -1)
        {
            // 为空or help
            command = commandbuf;
        }
        else
        {
            // 截取：前面的作为命令
            command = commandbuf.substr(0, idx);
            // 后面的字段
            str = commandbuf.substr(idx+1, commandbuf.size()-idx);
        }
        auto it = commandHandlerMap.find(command);
        if (it == commandHandlerMap.end())
        {
            cerr << "invalid input command!" << endl;
            continue;
        }

        // 调用相应的事件处理回调，main对修改封闭，对扩展开放
        it->second(clientfd, str); // 用相应的command方法处理字段data
    }
}

// 获取系统当前时间函数
string getCurrentTime()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}

// 子线程-接收线程
void readTaskHandler(int clientfd)
{
    for (;;)
    {
        char buf[1024] = {0};
        int len = recv(clientfd, buf, 1024, 0);
        if (len == -1 || len == 0)
        {
            close(clientfd);
            exit(-1);
        }

        // 接收Chatserver转发的数据,反序列化js对象
        json js = json::parse(buf);
        int msgtype = js["msgid"].get<int>();
        if (msgtype == ONE_CHAT_MSG)
        {
            cout << js["time"].get<string>() << "[" << js["id"] << "]" << js["name"].get<string>() << "said: " << js["msg"].get<string>() << endl;
            continue;
        }
        if (msgtype == GROUP_CHAT_MSG)
        {
            cout << "群聊消息" << js["time"].get<string>() << "[" << js["groupid"] << "]:" << js["groupname"].get<string>() << " said: " << js["msg"].get<string>() << endl;
            continue;
        }

        // 响应登录业务
        if (msgtype == LOGIN_MSG_ACK)
        {
            loginResponse(js);
            sem_post(&rwsem);
            continue;
        }

        // 响应注册业务
        if (msgtype == REG_MSG_ACK)
        {
            regResponse(js);
            sem_post(&rwsem);
            continue;
        }
    }
}