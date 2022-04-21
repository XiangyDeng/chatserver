#include "db.h"
#include <muduo/base/Logging.h>
#include <mysql/mysql.h>

// 建立数据库连接 mysql_init
MySql::MySql(std::string serverName, std::string username, std::string password,
             uint32_t m_port, std::string m_dbName)
    : m_serverName(serverName), m_userName(username), m_password(password),
      m_port(m_port), m_dbName(m_dbName) {
  
  m_conn = mysql_init(nullptr);
  if (m_conn == nullptr) {
    LOG_ERROR << "MySQL init failed!";
  }
}
// 释放数据库连接 mysql_close
MySql::~MySql() {
  if (m_conn != nullptr) {
    mysql_close(m_conn);
  }
}
// 连接数据库
bool MySql::connect() {
  MYSQL* p = mysql_real_connect(m_conn, m_serverName.c_str(), m_userName.c_str(),
                     m_password.c_str(), m_dbName.c_str(), m_port,
                     nullptr, 0);
  if (p == nullptr) {
    LOG_ERROR << "mysql connect fail!";
    return false;

  } else {
    LOG_INFO << "mysql connect seccuss!";
    // ! C和C++代码默认的编码字符是ASCII，如果不设置，从MySQL上拉下来的中文显示？
    mysql_query(m_conn, "set names gbk");
    return true;
  }
}

// 更新操作
bool MySql::update(std::string sql) {
  if (mysql_query(m_conn, sql.c_str())) {
    LOG_ERROR << "mysql update fail: " << sql;
    return false;

  } else {
    LOG_INFO << "mysql update success!";
    return true;
  }
}

// 查询操作
MYSQL_RES *MySql::query(std::string sql) {
  if (mysql_query(m_conn, sql.c_str())) {
    LOG_ERROR << "mysql query fail: " << sql;
    return nullptr;

  } else {
    LOG_INFO << "mysql query success!";
    return mysql_use_result(m_conn);

  }
}
// 获取连接
MYSQL *MySql::getConnection() const {
  return m_conn;
}

// int main() {
//   MySql mysql("localhost","melody","yuyu9633",3306,"chat");
//   mysql.connect();
//   mysql.update("INSERT INTO Friend (userid,friendid) VALUES (0,0)");
  
//   return 0;
// }