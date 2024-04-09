#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include <vector>
#include "user.h"
// 维护好友信息的操作接口方法
class FriendModel {
    
public:
    // 添加好友
    void insert(int userid, int friendid);

    
    // 返回用户好友r好友列表  多表查询
    vector<User> query(int userid); 
};

#endif