#include "groupmodel.hpp"

#include "db.h"

// 创建群组 Allgroup 表
bool GroupModel::createGroup(Group &group) {
  char sql[128] = {0};

  sprintf(sql,
          "INSERT INTO AllGroup (groupname, groupdesc) VALUES ('%s', '%s')",
          group.getName().c_str(), group.getDesc().c_str());

  MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");
  if (mysql.connect()) {
    if (mysql.update(sql)) {
      group.setId(mysql_insert_id(mysql.getConnection()));
      return true;
    }
  }

  return false;
}

// 加入群组 GroupUser 表
void GroupModel::addGroup(int userid, int groupid, std::string role) {
  char sql[128] = {0};

  sprintf(
      sql,
      "INSERT INTO GroupUser (userid, userid, grouprole) VALUES (%d, %d, '%s')",
      groupid, userid, role.c_str());

  MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");
  if (mysql.connect()) {
    mysql.update(sql);
  }
}

// 查询用户所在群组信息: AllGroup   INNER JOIN    GroupUser
// 1. 先根据输入的userid在GroupUser中查找其所在的群组
// 2. 在根据群组信息，查询属于该群组的所有用户的userid，并且和user表进行多表联合查询，查出用户的详细信息
std::vector<Group> GroupModel::queryGroups(int userid) {
  char sql[128] = {0};

  sprintf(sql,
          "SELECT a.id, a.groupname, a.groupdesc FROM AllGroup a INNER JOIN GroupUser b \
          ON a.id=b.groupid WHERE b.userid=%d", userid);

  MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");

  std::vector<Group> groupVec;
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr) {
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != nullptr) {
        Group group;
        group.setId(atoi(row[0]));
        group.setName(row[1]);
        group.setDesc(row[2]);

        groupVec.emplace_back(group);
      }

      // ! 别忘了释放资源
      mysql_free_result(res);
    }
  }

  // 查询对应群组的用户信息 user  groupuser
  for (auto &group : groupVec) {
    char sql[128] = {0};

    sprintf(sql,
            "SELECT a.id, a.name, a.state, b.grouprole FROM User a INNER JOIN GroupUser b \
            ON a.id=b.userid WHERE b.userid=%d", group.getId());

    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr) {
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != nullptr) {
        GroupUser user; // 继承自User类 包含以下四个私有成员
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setState(row[2]);
        user.setRole(row[3]);

        group.getUsers().emplace_back(user);
      }

      mysql_free_result(res);
    }
  }

  return groupVec;
}

// 根据指定的groupid查询群组用户的id列表，除userid自己，主要用户群聊业务给群组其他成员群发消息
std::vector<int> GroupModel::queryGroupUsers(int userid, int groupid) {
  char sql[128] = {0};

  sprintf(sql, "SELECT userid FROM GroupUser WHERE groupid=%d AND userid != %d",
          groupid, userid);

  std::vector<int> idVec;
  MySql mysql("localhost", "melody", "yuyu9633", 3306, "chat");
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);

    if (res != nullptr) {
      MYSQL_ROW row;

      while ((row = mysql_fetch_row(res)) != nullptr) {
        idVec.emplace_back(atoi(row[0]));
      }
      mysql_free_result(res);
    }
  }

  return idVec;
}
