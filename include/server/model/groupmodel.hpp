#pragma once

#include "group.hpp"

/**
 * @brief 将 数据库操作 与 业务代码 区分开
 * 
 */
class GroupModel {
public:
  // * 创建群组 : 需要setId(数据库自动生成)，所以不能使用const
  bool createGroup(Group &group);

  // 加入群组
  void addGroup(int userid, int groupid, std::string role);

  // 查询用户所在群组信息
  std::vector<Group> queryGroups(int userid);

  // 根据指定的groupid查询群组用户的id列表，除userid自己，主要用户群聊业务给群组其他成员群发消息
  std::vector<int> queryGroupUsers(int userid, int groupid);
};