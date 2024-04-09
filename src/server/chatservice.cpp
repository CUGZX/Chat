#include <muduo/base/Logging.h>
#include <vector>
#include <map>
#include <iostream>
#include "chatservice.h"
#include "usermodel.h"
#include "public.h"
#include"offlinemessagemodel.h"
#include"friendmodel.h"
#include"groupmodel.h"
//  #include"groupmodel.h"
using namespace muduo;
using namespace std;

// 获取单例对象
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 注册消息以及对应的回调操作
ChatService::ChatService()
{
    msg_handler_map_.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)});
    msg_handler_map_.insert({LOGINOUT_MSG, bind(&ChatService::loginout, this, _1, _2, _3)});
    msg_handler_map_.insert({REG_MSG, bind(&ChatService::reg, this, _1, _2, _3)});
    msg_handler_map_.insert({ONE_CHAT, bind(&ChatService::one_chat, this, _1, _2, _3)});
    msg_handler_map_.insert({ADD_FRIEND_MSG, bind(&ChatService::add_friend, this, _1, _2, _3)});
    msg_handler_map_.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    msg_handler_map_.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    msg_handler_map_.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
    // 用户断开连接
    if(_redis.connect()){
        _redis.init_notify_handler(bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

// 获取消息对应的处理器
MsgHandler ChatService::get_handler(int msgid)
{
    // 记录错误日志，msgid没有对应的事件处理回调
    auto it = msg_handler_map_.find(msgid);
    // 如果没有对应的msgid
    if (it == msg_handler_map_.end())
    {
        // 返回一个默认处理器，打印错误日志
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid: " << msgid << " can not find handler!";
        };
    }
    else
    {
        return msg_handler_map_[msgid];
    }
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();

    string pwd = js["password"];
    User user = _userModel.query(id);
    if (user.getID() == id && user.getPwd() == pwd)
    {

        if (user.getState() == "online")
        {
            // 该用户已经登录，不允许重复登录
            json response;

            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账号已经登录!";
            conn->send(response.dump());
        }
        else
        {

            {
                lock_guard<mutex> lock(_connMutex);
                // 用户登录成功
                _userConnMap.insert({id, conn});
            }

            // 向redids订阅 channel(id)
            _redis.subscribe(id);


            // 登陆成功  更新用户状态信息， state offline=》online
            user.setState("online");
            _userModel.updateState(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getID();
            response["name"] = user.getName();

            // 查询该用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty()){
                response["offlinemsg"] = vec;
                _offlineMsgModel.remove(id);
            }
           


            // 查询该用户的好友信息，并返回
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty()){
                vector<string> vec2;
                for (auto &user : userVec)
                {
                    json js;        
                    js["id"] = user.getID();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());  
                }
                response["friends"] = vec2;
            }

              //查询该用户的群组信息并返回
            vector<Group> group_vec = _groupModel.queryGroups(id);
            if (!group_vec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : group_vec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getID();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;

                    groupV.push_back(grpjson.dump());
                }

                response["groups"] = groupV;
            }

             conn->send(response.dump());
            
            

        }
    }
    else
    {

        // 登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或者密码错误或者用户不存在!";
        conn->send(response.dump());
    }
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];
    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getID();
        response["name"] = user.getName();
        conn->send(response.dump());
    }
    else
    {

        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;

    {
        lock_guard<mutex> lock(_connMutex);
        // 用户登录成功
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setID(it->first);
                
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 用户注销
    _redis.unsubscribe(user.getID());
    // 更新用户状态信息
    if (user.getID() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
   
}






 void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time){


    int userid = js["id"].get<int>();   
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }
    // 取消订阅
    _redis.unsubscribe(userid);

    // 更新用户的状态信息
    User user;
    user.setID(userid);
    user.setState("offline");
    _userModel.updateState(user);



 }


void ChatService::one_chat(const TcpConnectionPtr &conn, json &js, Timestamp time){

        int toid = js["to"].get<int>();

        // 标识用户是否在线
        bool userState = false;
        {
            lock_guard<mutex> lock(_connMutex);
            auto it = _userConnMap.find(toid);
            if (it != _userConnMap.end())
            {
                // to在线  服务器主动推送消息给toid用户
                it->second->send(js.dump());
                userState = true;
                   
            }
          
        }

        // 查询用户是否在线
        User user = _userModel.query(toid);
        if (user.getState() == "online" && !userState)
        {
            // 向redids发布消息
            _redis.publish(toid, js.dump());
            return;
        }

        // 不在线  存储离线消息

        _offlineMsgModel.insert(toid, js.dump());

    

}


void ChatService::reset(){

    // 把online用户的状态设置为offline

    _userModel.resetState();
}




// 添加好友   
 void ChatService::add_friend(const TcpConnectionPtr &conn, json &js, Timestamp time){


    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    
    // 存储好友信息

    _friendModel.insert(userid, friendid);




 }





 // 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userId = js["id"].get<int>();
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];

    // 存储新创建的群组消息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        // 存储群组创建人信息
       
        _groupModel.addGroup(userId, group.getId(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userId = js["id"].get<int>();
    int groupId = js["groupid"].get<int>();
    _groupModel.addGroup(userId, groupId, "normal");
}



// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userId = js["id"].get<int>();
    int groupId = js["groupid"].get<int>();
    std::vector<int> userIdVec = _groupModel.queryGroupUsers(userId, groupId);

    lock_guard<mutex> lock(_connMutex);
    for (int id : userIdVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 转发群消息
            it->second->send(js.dump());
        }
        else
        {
            // 查询toid是否在线
            User user = _userModel.query(id);
            if (user.getState() == "online")
            {
                // 向群组成员publish信息
                _redis.publish(id, js.dump());
            }
            else
            {
                //转储离线消息
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);    
    if(it != _userConnMap.end()){
        it->second->send(msg);
        return;
    }

    // 存储用户离线消息
    _offlineMsgModel.insert(userid, msg);
}