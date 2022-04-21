#pragma once

#include <string>
#include <vector>

/**
 * @brief 提供离线消息表的操作接口方法
 * 
 */
class OfflineMsgModel {
public:
  // 存储用户的离线消息
  void insert(uint32_t userid, std::string message);

  // 删除用户的离线消息
  void remove(uint32_t userid);

  // 查询用户的离线消息
  std::vector<std::string> query(uint32_t userid);

private:
}; 