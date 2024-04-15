#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H
#include <vector>
#include "user.hpp"
using namespace std;

// 维护好友列表的 操作类
class friendModel
{
public:
    // 添加好友
    void insert(int userid, int friendid);
    // 返回好友的user列表
    vector<User> query(int userid);
};

#endif