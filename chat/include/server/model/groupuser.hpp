#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"
//群组用户，比普通user多了role信息，从user直接继承就好
 class GroupUser: public User
 {
 private:
     string role;

 public:
     void setRole(string role) { this->role = role; }
     string getRole() { return this->role; }
 };
 

#endif