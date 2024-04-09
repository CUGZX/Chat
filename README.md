# Chat
集群聊天服务器
通过nginx tcp负载均衡连接处理的集群聊天服务器和客户端源码，基于moduo网络库实现

编译方式
cd build
rm -rf *
cmake ..
make

requires
nginx redis muduo
