#ifndef USERMODEL_H
#define USERMODEL_H

#include <string>
#include <iostream>
#include "user.hpp"
using namespace std;

// user表的 数据操作类
class UserModel
{
public:
    // user增加方法
    bool insert(User &user);
    // usermodel查询方法  封装user自己的查询方法
    User query(int id);
    //usermodel更新state方法  上面两个其实是封装的 sql语句 用sql语句对mysql进行操作
    bool updateState(User &user);
// 重置用户的状态信息
    void resetState();
};

#endif