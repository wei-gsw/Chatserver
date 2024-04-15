#ifndef USER_H
#define USER_H
#include <string>
#include <iostream>
using namespace std;

// 匹配user表达ORM类
//user对象类 数据库 和 服务模块分离=》 user表
class User
{
public:
    User(int id = -1, string name = " ", string password = " ", string state = "offline")
    {
        this->id = id;
        this->name = name;
        this->password = password;
        this->state = state;
    }
    // 设置各项参数函数
    void setid(int id) { this->id = id; }
    void setname(string name) { this->name = name; }
    void setpassword(string password) { this->password = password; }
    void setstate(string state) { this->state = state; }

    // 查看各项参数函数
    int getid() { return this->id; }
    string getname() { return this->name; }
    string getpassword() { return this->password; }
    string getstate() { return this->state; }

protected:
    int id;
    string name;
    string password;
    string state;
};

#endif