#include "chatserver.hpp"

#include <muduo/base/Logging.h>
#include <stdio.h>
#include <functional>
#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Channel.h>
#include <chatservice.hpp>
#include "json.hpp"

ChatServer::ChatServer(muduo::net::EventLoop *loop,
                       const muduo::net::InetAddress &listenAddr,
                       const std::string &nameArg)
    : m_loop(loop), m_nameArg(nameArg),
      m_server(m_loop, listenAddr, m_nameArg) {

  m_server.setConnectionCallback(
      std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
  
  m_server.setMessageCallback(
      std::bind(&ChatServer::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  // ! 设置线程数量
  m_server.setThreadNum(10);
}

void ChatServer::start() {
  m_server.start();
}

void ChatServer::onConnection(const muduo::net::TcpConnectionPtr &conn) {

  if (!conn->connected()) {

    ChatService::getInstance()->clientCloseException(conn);
    conn->shutdown();
  } 
}

// ! 目标：完全解耦网络模块框架代码与业务模块代码
// * 方式：js["msgid"] -> 获取业务handler -> conn,js,timestamp
void ChatServer::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer,
               muduo::Timestamp timestamp) {
  LOG_INFO << "onMessage called. ";
  std::string buf = buffer->retrieveAllAsString();

  // 接收数据反序列化
  json js = json::parse(buf);
  // 注意json类型 -> int类型
  auto msgHandler = ChatService::getInstance()->getHandle(js["msgid"].get<int>());
  // 回调消息绑定好的事件处理器，来进行对应的业务处理（框架与业务分离）
  msgHandler(conn, js, timestamp);
}

