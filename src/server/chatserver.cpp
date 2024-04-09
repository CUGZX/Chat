#include"chatserver.h"
#include<functional>
#include<iostream>
#include "json.hpp"
#include "chatservice.h"
using json =  nlohmann::json;
using namespace std;
using namespace placeholders;

using json =  nlohmann::json;
ChatServer::ChatServer(EventLoop* loop,const InetAddress& listenAddr,const string &nameArg):
    _server(loop,listenAddr,nameArg),
    _loop(loop)
{
        
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));
    _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));
    _server.setThreadNum(4);

}

void ChatServer::start()
    {
        _server.start();
    }

void ChatServer::onConnection(const TcpConnectionPtr& conn)
    {   
        // 用户断开连接
        if(!conn->connected()){
            ChatService::instance()->clientCloseException(conn);
            conn->shutdown();
            // conn->shutdown();
        }
    }   

void ChatServer::onMessage(const TcpConnectionPtr& conn,Buffer* buffer,Timestamp time)
    {
       string buf = buffer->retrieveAllAsString();
        cout<<"exute: "<<buf<<endl;
        //数据反序列化
        json js = json::parse(buf);
        //解耦网络和业务模块的代码
        //通过js里面的msgid，绑定msgid的回调函数，获取业务处理器handler
        auto msg_handler = ChatService::instance()->get_handler(js["msgid"].get<int>());

        //其实这个msg_handler就是login或者regist
        msg_handler(conn, js, time);
}