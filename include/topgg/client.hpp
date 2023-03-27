#ifndef TOPGG_CLIENT_HPP
#define TOPGG_CLIENT_HPP

#include <string>
#include <memory>

#include "http/request.hpp"
#include "http/response.hpp"
#include "errors.hpp"
#include "models.hpp"

namespace topgg {

class Client {
public:
    Client(const std::string& token);

    std::shared_ptr<User> getCurrentUser();
    std::shared_ptr<BotStats> getBotStats();
    std::vector<std::shared_ptr<User>> getBotVotes(const std::string& bot_id);

private:
    std::string m_token;
    std::string m_baseUrl = "https://top.gg/api/";
    std::string m_authHeader;
    int m_timeout = 5000;
};

} // namespace topgg

#endif // TOPGG_CLIENT_HPP
