#ifndef OFFLINEMSGMODEL_H
#define OFFLINEMSGMODEL_H
#include <vector>
#include <string>
using namespace std;

// 对离线消息进行操作 这是对数据库的操作  ORM的model层
class offlineMsgModel
{
public:
    // 插入用户消息到 offlinemsg数据库
    bool insert(int userid, string msg);

    // 从数据库删除 用户离线储存的消息
    void remove(int userid);

    // 查询用户离线消息
    vector<string> query(int userid);
};

#endif