#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <vector>
#include "groupuser.hpp"
using namespace std;

// user的ORM类
class Group
{
public:
    Group(int id = -1, string name = " ", string desc = " ")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setid(int id) { this->id = id; }
    void setname(string name) { this->name = name; }
    void setdesc(string desc) { this->desc = desc; }

    int getid() { return this->id; }
    string getname() { return this->name; }
    string getdesc() { return this->desc; }
    vector<GroupUser> &getUsers() { return this->users; }

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};

#endif