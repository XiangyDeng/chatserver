#pragma once

#include <mysql/mysql.h>
#include <string> 

class MySql {
public:
  // 建立数据库连接 mysql_init
  MySql(std::string serverName, std::string username, std::string password, uint32_t m_port, std::string m_dbName);
  // 释放数据库连接 mysql_close
  ~MySql();
  // 连接数据库
  bool connect();
  // 更新操作
  bool update(std::string sql);
  // 查询操作
  MYSQL_RES *query(std::string sql);
  // 获取连接
  MYSQL* getConnection() const;

private:
  MYSQL *m_conn;

  std::string m_serverName;
  uint32_t m_port;
  std::string m_userName;
  std::string m_password;
  std::string m_dbName;
};