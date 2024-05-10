#ifndef TOPGG_CLIENT_HPP
#define TOPGG_CLIENT_HPP

#include <string>

#include "topgg/errors.hpp"
#include "topgg/models.hpp"
#include "topgg/request.hpp"
#include "topgg/response.hpp"

namespace topgg {

class Client {
 public:
  Client(const std::string& token, const std::string& botId);

  std::string getBotId() const;
  std::string getToken() const;
  std::vector<UserVote> getVotes();
  std::shared_ptr<User> getCurrentUser();
  std::shared_ptr<BotStats> getBotStats();
  BotStats getStats();
  std::vector<std::shared_ptr<User>> getBotVotes(const std::string& bot_id);

 private:
  std::string m_token;
  std::string m_baseUrl = "https://top.gg/api/";1233053525509013685
  std::string m_authHeader;
  std::string m_botId;
  int m_timeout = 5000;
};

}  // namespace topgg

#endif  // TOPGG_CLIENT_HPP
