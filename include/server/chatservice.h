#ifndef CHARTSERVICE_H
#define CHARTSERVICE_H
#include <unordered_map>
#include <functional>
#include <mutex>
#include <muduo/net/TcpConnection.h>
#include "json.hpp"
#include"usermodel.h"
#include<mutex>
#include"offlinemessagemodel.h"
#include"friendmodel.h"
#include"groupmodel.h"

// #include "UserModel.hpp"
// #include "OfflineMessageModel.hpp"
// #include "FriendModel.hpp"
//  #include "GroupModel.hpp"
#include "redis.h"
using namespace muduo;
using namespace muduo::net;
using json =  nlohmann::json;
using namespace std;
using  MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;
class ChatService
{
public:
    //获取单例对象
    static ChatService *instance();

    //登录
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);

    //注册
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // //一对一聊天业务
    void one_chat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // //添加好友业务
    void add_friend(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // //创建 群组
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // //加入群组
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // //群聊业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // //注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // //redis订阅消息触发的回调函数
    // void redis_subscribe_message_handler(int channel, string message);

    // //redis发布消息触发的回调函数
    void handleRedisSubscribeMessage(int userid, string msg);
   
    // //获取消息对应的处理器
    MsgHandler get_handler(int msgid);

    // //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    // //服务器异常，业务重置
    void reset();
private:
    //注册消息以及对应的回调操作
    ChatService();

private:
    //存储事件触发的回调函数
    unordered_map<int, MsgHandler> msg_handler_map_;

    //存储在线用户的连接情况，便于服务器给用户发消息，注意线程安全
    unordered_map<int, TcpConnectionPtr> user_connection_map_;
    mutex conn_mutex_;

    //存储消息id及其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;


    // 存储在线用户的通信id
    unordered_map<int, TcpConnectionPtr> _userConnMap;


    //数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;
    // Redis _redis;
    // GroupModel _groupModel;
    //定义互斥锁 保证_userConnMap的线程安全
    mutex _connMutex;

    // redis 相关操作
    Redis _redis;
    
};



#endif