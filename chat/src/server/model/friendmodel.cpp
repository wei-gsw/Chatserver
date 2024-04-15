#include"friendmodel.hpp"
#include<vector>
#include"db.h"
using namespace std;


// 添加好友
void friendModel::insert(int userid, int friendid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d,%d)", userid, friendid);

    // 2.对mysql进行操作
    MySQL mysql;
    // 连接
    if (mysql.connect())
    {
        mysql.update(sql);
        return;
    }
}

// 返回好友的 user列表
vector<User> friendModel::query(int userid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid=a.id where b.userid=%d", userid);

    // 2.对mysql进行操作
    MySQL mysql;
    vector<User> vec;
    // 连接
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != NULL)
            {
                User user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setstate(row[2]);

                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}