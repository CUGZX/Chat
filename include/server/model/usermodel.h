#ifndef UserModel_H
#define UserModel_H

#include "user.h"
#include <mysql/mysql.h>

class UserModel {
    
public:
    // user 插入操作
    bool insert(User &user);
    User query(int id);
    bool updateState(User &user);
    bool isOnline(int id);
    bool query(string name, User &user);
    void resetState();
};

#endif