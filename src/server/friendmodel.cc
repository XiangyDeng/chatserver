#include "friendmodel.hpp"
#include "db.h"

//   
void FriendModel::insert(uint32_t userid, uint32_t friendid) {
  char sql[1024] = {0};
  sprintf(sql, "INSERT INTO Friend VALUES(%d,%d)", userid, friendid);

    MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");
    if (mysql.connect()) {
      mysql.update(sql);
    }
} 

// * 返回用户好友列表 friendid 返回两个表的联合查询
std::vector<User> FriendModel::query(uint32_t userid) {
  char sql[1024] = {0};
  sprintf(sql, "SELECT a.id, a.name, a.state FROM User a INNER JOIN Friend b ON b.friendid = a.id WHERE b.userid=%d", userid);
  MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");

  std::vector<User> userVec;
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr) {
      MYSQL_ROW row;
      // ! 此处是一行行获取
      // * 每行的row[0] row[1] row[2] 分别表示不同的字段
      while ((row = mysql_fetch_row(res)) != nullptr) {
        User user;
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setState(row[2]);

        userVec.emplace_back(user);
      }
    }

    mysql_free_result(res);
  }

  return userVec;
}