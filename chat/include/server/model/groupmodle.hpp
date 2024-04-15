#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <string>
#include <vector>
using namespace std;

// 操作group类
class GroupModel
{
public:
    // 创建群组
    bool createGroup(Group &group);
    // 加入群组,role
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在的群组信息
    vector<Group> queryGroups(int userid);
    // 根据指定的groupid查询用户id列表， 除了userid自己，给其他成员发消息
    vector<int> queryGroupUsers(int userid, int groupid);
};

#endif