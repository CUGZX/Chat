#ifndef __GROUP_USER_H__
#define __GROUP_USER_H__

#include "user.h"
#include <string>

// TODO:别继承了，分开比较好
class GroupUser : public User
{
public:
    GroupUser() = default;
    void setRole(const std::string& role) { this->role = role; }
    std::string getRole() { return role; }

private:
    std::string role;
};


#endif // __GROUP_USER_H__