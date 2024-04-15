#include "groupmodle.hpp"
#include "db.h"

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname,groupdesc) values('%s','%s')", group.getname().c_str(), group.getdesc().c_str());

    // 2.对mysql进行操作
    MySQL mysql;
    // 连接
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            group.setid(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

// 加入群组,role
void GroupModel::addGroup(int userid, int groupid, string role)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d,%d,'%s')", userid, groupid, role.c_str());

    // 2.对mysql进行操作
    MySQL mysql;
    // 连接
    if (mysql.connect())
    {
        mysql.update(sql);
        return;
    }
}

// 查询用户所在的群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    /*
    1.现根据userid在group表查询 所属群组信息
    2.再根据群组信息，查询所有userid，和user表联合查询，查出用户详细信息
    */
    // 现根据userid在group表查询 所属群组信息
    //  1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id=b.groupid where b.userid=%d", userid);

    // 2.对mysql进行操作
    MySQL mysql;
    vector<Group> vec;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != NULL)
            {
                Group group;
                group.setid(atoi(row[0]));
                group.setname(row[1]);
                group.setdesc(row[2]);

                vec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    // 查询组员， vector<GroupUser> users;
    for (Group &group : vec)
    {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from user a inner join groupuser b on a.id=b.userid where b.groupid=%d", group.getid());

        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != NULL)
            {
                GroupUser user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setstate(row[2]);
                user.setRole(row[3]);

                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}

//  根据指定的groupid查询用户id列表， 除了userid自己，给其他成员发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    //  1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid=%d and userid !=%d", groupid,userid);

    // 2.对mysql进行操作
    MySQL mysql;
    vector<int> vec;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != NULL)
            {
                vec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return vec;
}