#include "offlinemessagemodel.hpp"

#include "db.h"
#include <mysql/mysql.h>

// 存储用户的离线消息:insert to 到数据库的表中
void OfflineMsgModel::insert(uint32_t userid, std::string message) {
  char sql[1024] = {0};
  sprintf(sql, "INSERT INTO OfflineMessage VALUES (%d,'%s')", userid,
          message.c_str());

  MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");

  if (mysql.connect()) {
    mysql.update(sql);
  }
}

// 删除用户的离线消息
void OfflineMsgModel::remove(uint32_t userid) {
  char sql[1024] = {0};
  sprintf(sql, "DELETE FROM OfflineMessage WHERE userid=%d", userid);

  MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");

  if (mysql.connect()) {
    mysql.update(sql);
  }
}

// 查询用户的离线消息
std::vector<std::string> OfflineMsgModel::query(uint32_t userid) {
  char sql[1024] = {0};
  sprintf(sql, "SELECT * FROM OfflineMessage WHERE userid=%d", userid);

  MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");

  std::vector<std::string> msg;
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr) {
      // * 把从数据库中查询到的userid用户的所有离线消息放入vector中返回
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != nullptr) {
        msg.emplace_back(row[0]);
        msg.emplace_back(row[1]);
      }
    }

    mysql_free_result(res);
  }

  return msg;
}
