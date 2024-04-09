#ifndef __GROUP_H__
#define __GROUP_H__

#include <string>
#include <vector>
#include "groupuser.h"

class Group
{
public:
    Group(int id = -1, std::string name = "", std::string desc = "")
        : id(id),
          desc(desc),
          name(name)
    {}

    void setId(int id) { this->id = id; }
    void setName(std::string name) { this->name = name; }
    void setDesc(std::string desc) { this->desc = desc; }

    int getId() { return id; }
    std::string getName() { return this->name; }
    std::string getDesc() { return this->desc; }
    std::vector<GroupUser> &getUsers() { return users; }

private:
    int id;
    std::string name;
    std::string desc;
    std::vector<GroupUser> users;
};

#endif // __GROUP_H__