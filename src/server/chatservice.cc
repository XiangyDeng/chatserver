#include "chatservice.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <muduo/base/Logging.h>
#include "user.hpp"
#include "public.hpp"


// 注册消息以及堆顶的Handler回调操作
ChatService::ChatService() {
  m_MsgHandlerrMap.insert(
      {LOGIN_MSG, std::bind(&ChatService::login, this, std::placeholders::_1,
                            std::placeholders::_2, std::placeholders::_3)});

  m_MsgHandlerrMap.insert({
      REG_MSG, std::bind(&ChatService::reg, this, std::placeholders::_1,
                           std::placeholders::_2, std::placeholders::_3)});

  m_MsgHandlerrMap.insert(
      {ONE_CHAT_MSG,std::bind(&ChatService::oneChat, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3)});

    m_MsgHandlerrMap.insert(
      {ADD_FRIEND_MSG,std::bind(&ChatService::addFriend, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3)});
  
}

// 处理登录业务 id name password
void ChatService::login(const muduo::net::TcpConnectionPtr &conn, json &js,
           muduo::Timestamp Timestamp) {
  // step 1. 反序列化得到id 与 password
  uint32_t id = js["id"];
  std::string pwd = js["password"];

  // step 2. 查询数据库验证id对应的password是否真正确
  User user = m_userModel.query(id);
  json response;

  if (user.getId() == id && user.getPassword() == pwd) {
    if (user.getState() == "online") {
      response["msgid"] = LOGIN_MSG_ACK;
      response["errno"] = 1;
      response["errmsg"] = "用户重复登陆！请重新输入账号";
      conn->send(response.dump());
    } else {
      // 登录成功
      {
        // 多个work线程可能同时登录添加用户信息
        std::unique_lock<std::mutex> lock(m_connMutex);
        // 记录用户登录信息
        m_userConnMap.insert({id, conn});
      }
      
      // 数据库状态更新
      user.setState("online");
      m_userModel.updateState(user);

      response["msgid"] = LOGIN_MSG_ACK;
      response["errno"] = 0;
      response["id"] = user.getId();
      response["name"] = user.getName();

      // * 查询用户是否有离线消息
      std::vector<std::string> offlineMsg = m_offlineMsgModel.query(id);
      if (!offlineMsg.empty()) {
        response["offlineMsg"] = offlineMsg;
        // 读取该用户的离线消息后，清空数据库表中存储的离线消息
        m_offlineMsgModel.remove(id);
      } 

      // * 查询该用户的好友信息，并返回
      std::vector<User> userVec = m_friendModel.query(id);
      if (!userVec.empty()) {
        std::vector<std::string> friendMsg;

        for (auto &e : userVec) {
          json js;
          js["id"] = e.getId();
          js["name"] = e.getName();
          js["state"] = e.getState();

          friendMsg.emplace_back(js.dump());
        }

        response["friends"] = friendMsg;
      }

      conn->send(response.dump());
    }
    
  } else {
    // 该用户不存在，登陆失败
    response["msgid"] = LOGIN_MSG_ACK;
    response["errno"] = 1;
    response["errmsg"] = "用户名或者密码错误！";
    conn->send(response.dump());
   }
}

// 处理注册业务
void ChatService::reg(const muduo::net::TcpConnectionPtr &conn, json &js,
                      muduo::Timestamp Timestamp) {
  // step 1. 读取客户端注册信息：用户名 与 密码
  std::string name = js["name"];
  std::string pwd = js["password"];

  // step 2. 创建用户对象：与数据库的User表数据类型相同，只包含类型不包含操作
  User user;
  user.setName(name);
  user.setPassword(pwd);

  // step 3. 使用UserModel操作类执行数据插入操作
  bool state = m_userModel.insert(user);
  json response;
  if (state == true) {
    // 注册成功
    // step 3.1 生成序列化数据
    response["msgid"] = REG_MSG_ACK;
    response["errno"] = 0;
    response["id"] = user.getId();
    // step 3.2 网络库发送数据
    conn->send(response.dump());

  } else {
    // 注册失败
    response["msgid"] = REG_MSG_ACK;
    response["errno"] = 1;
    response["errmsg"] = "MySql insert fail!";

    conn->send(response.dump());
  }
}

void ChatService::clientCloseException(const muduo::net::TcpConnectionPtr &conn) {
  User user;
  {
    std::unique_lock<std::mutex> lock(m_connMutex);
    // step 1.删除表中的存储信息
    for (auto &e : m_userConnMap) {
      if (e.second == conn) {
        user.setId(e.first);
        m_userConnMap.erase(e.first);
        break;
      }
    }
  }

  // step 2.将操作更新到MySql数据库中（State信息更新为'offline'）
  // * 通过user类操作
  if (user.getId() != -1) {
    user.setState("offline");
    m_userModel.updateState(user);
  }
}

// 一对一聊天业务
void ChatService::oneChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp Timestamp){
  int toid = js["to"].get<int>();

  bool userState = false;
  {
    std::unique_lock<std::mutex> lock(m_connMutex);
    auto it = m_userConnMap.find(toid);
    if (it != m_userConnMap.end()) {
      // toid在线，转发消息(服务器的作用为消息中转)
      it->second->send(js.dump());
      return;
    }
  }

  // toid不在线，存储离线信息(OfflineMessage表)
  m_offlineMsgModel.insert(toid,js.dump());
}

  // 添加好友业务: MySql表，msgid id字段
void ChatService::addFriend(const muduo::net::TcpConnectionPtr &conn, json &js,
                            muduo::Timestamp Timestamp) {
  // 反序列化获得数据:注意使用get转为整型
  uint32_t userid = js["id"].get<int>();
  uint32_t friendid = js["friendid"].get<int>();

  // 添加好友信息
  m_friendModel.insert(userid, friendid);
}

// 服务器异常，业务重置方法
void ChatService::reset() {
  // 把online状态的用户，设置成offline
  m_userModel.resetState();
}

// 创建群组业务
void ChatService::createGroup(const muduo::net::TcpConnectionPtr &conn, json &js,
                            muduo::Timestamp Timestamp) {
    int userid = js["id"].get<int>();
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (m_groupModel.createGroup(group))
    {
        // 存储群组创建人信息
        m_groupModel.addGroup(userid, group.getId(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    m_groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    std::vector<int> useridVec = m_groupModel.queryGroupUsers(userid, groupid);

    {
      std::unique_lock<std::mutex> lock(m_connMutex);
      for (int id : useridVec)
      {
          auto it = m_userConnMap.find(id);
          if (it != m_userConnMap.end())
          {
              // 转发群消息
              it->second->send(js.dump());
          }
          else
          {
              // 存储离线群消息
              m_offlineMsgModel.insert(id, js.dump());
          }
      }
      
    }
}

// 获取消息id对应的处理器Handler
MsgHandler ChatService::getHandle(int msgid) {
  if (!m_MsgHandlerrMap.count(msgid)) {
    // 返回空操作
    return [=](const muduo::net::TcpConnectionPtr &conn, json &js,
               muduo::Timestamp Timestamp) {
        LOG_ERROR << "msgid: " << msgid << " cannot find Handler! ";         
      };
  } else {
    return m_MsgHandlerrMap[msgid];
  }
}

