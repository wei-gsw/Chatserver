#ifndef PUBLIC_H
#define PUBLIC_H
#include <atomic>
#include <thread>
#include <semaphore.h>
using namespace std;
// server和 service的公共文件
// 枚举类
enum EnMsgType
{
    LOGIN_ACK = 1,  // 登录消息1
    LOGIN_MSG_ACK,  // 登录响应消息2
    REG_ACK,        // 注册消息3
    REG_MSG_ACK,    // 注册相应消息4
    ONE_CHAT_MSG,   // 聊天消息5
    ADD_FRIEND_MSG, // 添加好友信息6

    CREATE_GROUP_MSG, // 创建群组7
    ADD_GROUP_MSG,    // 加入群组8
    GROUP_CHAT_MSG,   // 群聊天9
    LOGINOUT_MSG,//注销10

};

// atomic_bool 是标准库中原子布尔类型,提供了线程安全的操作，避免竞态条件和数据竞争
// 记录登录状态
atomic_bool g_isLoginSuccess{false};

// 记录当前系统登录的用户信息
User g_currentUser;
// 记录当前登录用户的好友列表
vector<User> g_currentUserFriendList;
// 记录当前登录用户的群组列表
vector<Group> g_currentUserGroupList;
// 控制主菜单页面程序
bool isMainMenRunning = false;

// 用于读写程序线程之间的通信 信号量
sem_t rwsem;
//  记录登录状态

// 聊天客户端程序实现，main线程用作发送线程，子线程用作接收线程

// 接收线程
void readTaskHandler(int clientfd);
string getCurrentTime();
#endif