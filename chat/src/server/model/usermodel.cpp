#include "user.hpp"
#include "usermodel.hpp"
#include "db.h"
#include <iostream>
using namespace std;

// user增加方法
bool UserModel::insert(User &user)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name,password,state) values('%s','%s','%s')", user.getname().c_str(), user.getpassword().c_str(), user.getstate().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取插入成功得到的用户数据生成的主键id
            user.setid(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

// user查询方法 根据id进行查询 如果查找到了返回 user包含id，name，pwd，state
User UserModel::query(int id)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id=%d", id);

    // 对mysql进行操作
    MySQL mysql;
    // 连接
    if (mysql.connect())
    {
        // query(mysql, sql)获取结果集  mysql_fetch_row函数逐行获取查询结果
        MYSQL_RES *result = mysql.query(sql);
        if (result != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row != nullptr)
            {
                User user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setpassword(row[2]);
                user.setstate(row[3]);
                mysql_free_result(result);

                return user;
            }
        }
    }
    return User();
}

// 数据库操作类的更新函数
bool UserModel::updateState(User &user)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state='%s' where id=%d", user.getstate().c_str(),user.getid());

    // 2.对mysql进行操作
    MySQL mysql;
    // 连接
    if (mysql.connect())
    {   
        if(mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

// 重置用户的状态信息
void UserModel::resetState()
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state='offline' where state='online'");

    // 2.对mysql进行操作
    MySQL mysql;
    // 连接
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}