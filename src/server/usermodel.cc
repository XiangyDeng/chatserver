#include "usermodel.hpp"

#include <stdio.h>
#include "db.h"

bool UserModel::insert(User &user) {
  char sql[128] = {0};
  // TODO 业务 框架未分开？
  sprintf(
      sql,
      "INSERT INTO User (id, name, password, state) VALUES (%d, \"%s\", \"%s\", \"%s\")",
      user.getId(), user.getName().c_str(), user.getPassword().c_str(),
      user.getState().c_str());

  // TODO 业务 框架未分开？
  // step 1. 初始化
  MySql mysql("localhost","melody","yuyu9633",3306,"chat");

  // step 2. 连接数据库
  if (mysql.connect()) {
    // step 3. 更新数据sql
    if (mysql.update(sql)) {
      // TODO 获取成功插入数据值的主键id（自增）
      user.setId(mysql_insert_id(mysql.getConnection()));
      // user.setId(mysql_insert_id(mysql.getConnection())
      return true;
    }
  }

  return false;
}

// 根据id查询数据库用户user信息
User UserModel::query(uint32_t id) {
  char sql[128] = {0};
  sprintf(sql, "SELECT * FROM User WHERE id=%d", id);

  // TODO 业务 框架未分开？
  // step 1. 初始化
  MySql mysql("localhost","melody","yuyu9633",3306,"chat");

  // step 2. 连接数据库
  if (mysql.connect()) {
    // step 3. 更新数据sql
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr) {
      MYSQL_ROW row = mysql_fetch_row(res);
      if (row != nullptr) {
        User user;
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setPassword(row[2]);
        user.setState(row[3]);
        // !注意此处需要释放指针
        mysql_free_result(res);
        return user;
      }
    }
  }

  return User();
}

  // 更新用户状态信息
bool UserModel::updateState(User user) {
  char sql[128];
  sprintf(sql,"UPDATE User SET State='%s' WHERE id=%d", user.getState().c_str(), user.getId());
  
  // step 1. 初始化
  MySql mysql("localhost","melody","yuyu9633",3306,"chat");

  // step 2. 连接数据库
  if (mysql.connect()) {
    // step 3. 更新数据sql
    if ( mysql.update(sql)) {
      return true;
    }
  }

  return false;
}

  // 重置所有online用户状态为offline
bool UserModel::resetState() {
  char sql[128];
  sprintf(sql, "UPDATE User SET state = 'offline' WHERE State = 'online'");

  MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");
  if (mysql.connect()) {
    if (mysql.update(sql)) {
      return true;
    }
  }

  return false;
}
