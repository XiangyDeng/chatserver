#include "chatserver.hpp"
#include "chatservice.hpp"

#include <signal.h>

// 处理 ctrl c 中断信号
void resetHandler(int) {
  ChatService::getInstance()->reset();
  printf("CTRL C occur. \n");
  exit(0);
}

int main() {
  signal(SIGINT, resetHandler);

  // ! loop不可为空
  muduo::net::EventLoop *mainLoop = new muduo::net::EventLoop();
  muduo::net::InetAddress servAddr("127.0.0.1",8002,false);
  ChatServer server(mainLoop, servAddr, "ChatServer");
  server.start();
  mainLoop->loop();

  return 0;
}

