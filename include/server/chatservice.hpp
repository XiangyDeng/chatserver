#pragma once

#include <unordered_map>
#include <functional>
#include <mutex>
#include "friendmodel.hpp"
#include "json.hpp"
#include "muduo/net/TcpConnection.h"

#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "groupmodel.hpp"

using json = nlohmann::json;
using MsgHandler = std::function<void(const muduo::net::TcpConnectionPtr&, json&, muduo::Timestamp)>;

/**
 * @brief 聊天服务器业务类
 * 
 */
class ChatService {
public:
  // 单例构造
  static ChatService *getInstance() {
    static ChatService instance;
    return &instance;
  }

  // 处理登录业务
  void login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp timestamp);
  // 处理注册业务
  void reg(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp timestamp);
  // 一对一聊天业务
  void oneChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp timestamp);
  // 添加好友业务
  void addFriend(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp timestamp);
  // 创建组业务
  void createGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp timestamp);
  // 加入组业务
  void addGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp timestamp);
  // 群组聊天业务
  void groupChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp timestamp);
  // 用户登出业务
  void loginOut(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp timestamp);


  // 处理客户的异常退出
  void clientCloseException(const muduo::net::TcpConnectionPtr &conn);
  // 服务器异常，业务重置方法
  void reset();

  // 获取消息id对应的处理器Handler
  MsgHandler getHandle(int msgid);

private:
  ChatService();

  // 存储消息id和其对应的业务处理方法
  std::unordered_map<int, MsgHandler> m_MsgHandlerrMap;
  std::mutex m_connMutex;

  // 存储在线用户的通信连接
  std::unordered_map<int, const muduo::net::TcpConnectionPtr &> m_userConnMap;

  // 数据操作类对象
  UserModel m_userModel;
  OfflineMsgModel m_offlineMsgModel;
  FriendModel m_friendModel;
  GroupModel m_groupModel;

};