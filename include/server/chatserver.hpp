#pragma once


#include <string>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>


/**
 * @brief 聊天服务器的主类，复合TcpServer类（并非继承关系）
 * 
 */
class ChatServer {
public:
  // 构造聊天服务器对象
  ChatServer(muduo::net::EventLoop *loop,
             const muduo::net::InetAddress &listenAddr,
             const std::string &nameArg);
  ~ChatServer() = default;

  // 启动服务:调用TcpServer的start函数
  void start();

private:
  void onConnection(const muduo::net::TcpConnectionPtr &);
  void onMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *,
                 muduo::Timestamp);
  
  muduo::net::EventLoop *m_loop;    // mainloop（指针）
  const muduo::net::InetAddress m_listenAddr;
  const std::string m_nameArg;

  // * 复合类：实现服务器功能
  muduo::net::TcpServer m_server;
};