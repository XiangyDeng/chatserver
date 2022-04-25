#pragma once

#include <string>

/**
 * @brief User表的ORM类:映射表的字段
 * 
 */
class User {
public:
  User(uint32_t id = 0, const std::string &username = "",
       const std::string &password = "", const std::string &state = "offline")
      : m_id(id), m_name(username), m_password(password), m_state(state) {}

  void setId(uint32_t id) { m_id = id; }
  void setName(const std::string &name) { m_name = name; }
  void setPassword(const std::string &password) { m_password = password; }
  void setState(const std::string &state) { m_state = state; }

  uint32_t getId() const { return m_id; }
  std::string getName() const { return m_name; }
  std::string getPassword() const { return m_password; }
  std::string getState() const { return m_state; }

private :
  uint32_t m_id;
  std::string m_name;
  std::string m_password;
  std::string m_state;
};
