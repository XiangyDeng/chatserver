#include "chatserver.hpp"
#include "chatservice.hpp"

#include <iostream>
#include <signal.h>

// 处理 ctrl c 中断信号
void resetHandler(int) {
  ChatService::getInstance()->reset();
  printf("CTRL C occur. \n");
  exit(0);
}

int main(int argc, const char** argv) {
  signal(SIGINT, resetHandler);

  if (argc != 3) {
    std::cerr << "command invalid! examples: ./ChatServer 127.0.0.1 6000" << std::endl; 
    exit(-1);
  }

  const char *ip = argv[1];
  uint16_t port = atoi(argv[2]);

  // ! loop不可为空
  muduo::net::EventLoop *mainLoop = new muduo::net::EventLoop();
  muduo::net::InetAddress servAddr(ip, port, false);
  ChatServer server(mainLoop, servAddr, "ChatServer");
  server.start();
  mainLoop->loop();
                                         
  return 0;
}


