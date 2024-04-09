#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<iostream>
#include<functional>
#include<string>
using namespace std;

using namespace std::placeholders;
using namespace muduo::net;
using namespace muduo;

class ChatServer{
public:
    ChatServer(EventLoop* loop,const InetAddress& listenAddr, const string &name)
        :_server(loop,listenAddr,name),
        _loop(loop)
    {
            
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));
        _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));    
        }
    void start()
    {
        _server.start();
    }
private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if(conn->connected())
        {
            cout<<conn->peerAddress().toIpPort()<<"->"
                <<conn->localAddress().toIpPort()<<"state:"
                <<"online"<<endl;
        }
        else{
                cout<<conn->peerAddress().toIpPort()<<"->"
                    <<conn->localAddress().toIpPort()<<"state:"
                    <<"offline"<<endl;
                    conn->shutdown();
                // _loop->quit();
        }
        
    }   

    void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp time)
    {
        string msg=buf->retrieveAllAsString();
        cout<<"recv data:"<<msg<<"time:"<<time.toString()<<endl;
        
        conn->send(msg);                
    }

    TcpServer _server;
    EventLoop* _loop;
};

int main()
{
    EventLoop loop;  // epoll
    InetAddress addr("127.0.0.2",6000);
    ChatServer server(&loop, addr,"ChatServer");
    server.start(); // epoll_wait 
    loop.loop();
    return 0;
}