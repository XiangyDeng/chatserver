#pragma once

#include <vector>
#include "user.hpp"

/**
 * @brief 维护好友信息的操作接口方法：操作MySql的Friend表
 * 
 */
#include <cstdint>
class FriendModel {
public:
  // 添加好友关系
  void insert(uint32_t userid, uint32_t friendid);

  // 返回用户好友列表 friendid 返回两个表的联合查询
  std::vector<User> query(uint32_t id);

private:
  
};