#pragma once

#include "user.hpp"

/**
 * @brief User表的操作类
 * 
 */
class UserModel {
public:
  // User表的增加方法
  bool insert(User &user);

  // 根据id号码查询用户信息
  User query(uint32_t id);

  // 更新数据库信息
  bool updateState(User user);

  // 重置所有用户状态
  bool resetState();

private:

};