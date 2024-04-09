#include"chatserver.h"
#include"chatservice.h"
#include<iostream>
#include<signal.h>
using namespace std;

void reserHandler(int sig)
{
    ChatService::instance()->reset();
    cout<<"Catch the signal"<<endl;
    exit(0);
}
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "command invaild example: ./ExeNAME  IpAddress  port" << endl;
        exit(-1);
    }

    //解析IP地址和端口号
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT, reserHandler);

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}