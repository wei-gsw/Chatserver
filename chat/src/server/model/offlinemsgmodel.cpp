#include "offlinemsgmodel.hpp"
#include <string>
#include "db.h"

// 插入用户消息到 offlinemsg数据库
bool offlineMsgModel::insert(int toid, string msg)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d,'%s')", toid, msg.c_str());

    // 2.对mysql进行操作
    MySQL mysql;
    // 连接
    if (mysql.connect())
    {
        mysql.update(sql);
        return true;
    }else
    {
       return false; 
    }
    
}

// 从数据库删除 用户离线储存的消息
void offlineMsgModel::remove(int userid)
{ // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid=%d", userid);

    // 2.对mysql进行操作
    MySQL mysql;
    // 连接
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}


// 查询用户离线消息
vector<string> offlineMsgModel::query(int userid)
{ 
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid=%d", userid);

    // 2.对mysql进行操作
    MySQL mysql;
    vector<string> vec;
    // 连接
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
       if(res!=nullptr)
       {
           MYSQL_ROW row;
           while((row=mysql_fetch_row(res))!=NULL)
           {
               vec.push_back(row[0]);
           }
           mysql_free_result(res);
           return vec;
       }
    }
    return vec;
}