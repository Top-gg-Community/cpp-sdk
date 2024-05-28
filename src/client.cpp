#include <topgg/topgg.h>

using topgg::client;

void client::setup_headers(std::multimap<std::string, std::string>& headers, const std::string& token) {
  headers.insert(std::pair("Authorization", "Bearer " + token));
  headers.insert(std::pair("Connection", "close"));
  headers.insert(std::pair("Content-Type", "application/json"));
  headers.insert(std::pair("User-Agent", "topgg (https://github.com/top-gg-community/cpp-sdk) D++"));
}

client::client(dpp::cluster* cluster, const std::string& token)
  : m_cluster(cluster) {
  client::setup_headers(m_headers, token);
}

#define TOPGG_API_ENDPOINT(name) \
  void client::name(const topgg::name##_completion_t& callback)

#define TOPGG_API_ENDPOINT_ARGS(name, ...) \
  void client::name(__VA_ARGS__, const topgg::name##_completion_t& callback)

TOPGG_API_ENDPOINT_ARGS(get_bot, const dpp::snowflake& bot_id) {
  basic_request<topgg::bot>("/bots/" + std::to_string(bot_id), callback, [](const auto& j) {
    return topgg::bot{j};
  });
}

TOPGG_API_ENDPOINT_ARGS(get_user, const dpp::snowflake& user_id) {
  basic_request<topgg::user>("/users/" + std::to_string(user_id), callback, [](const auto& j) {
    return topgg::user{j};
  });
}

TOPGG_API_ENDPOINT_ARGS(post_stats, const stats& s) {
  auto headers = std::multimap<std::string, std::string>{m_headers};
  const auto s_json = s.to_json();

  headers.insert(std::pair("Content-Length", std::to_string(s_json.size())));

  m_cluster->request("https://top.gg/api/bots/stats", dpp::m_post, [callback](TOPGG_UNUSED const auto& _) { callback(); }, s_json, "application/json", headers);
}

TOPGG_API_ENDPOINT(get_stats) {
  basic_request<topgg::stats>("/bots/stats", callback, [](const auto& j) {
    return topgg::stats{j};
  });
}

TOPGG_API_ENDPOINT(get_voters) {
  basic_request<std::vector<topgg::voter>>("/bots/votes", callback, [](const auto& j) {
    std::vector<topgg::voter> voters;

    for (const auto& part: j) {
      voters.push_back(topgg::voter{part});
    }

    return voters;
  });
}

TOPGG_API_ENDPOINT_ARGS(has_voted, const dpp::snowflake& user_id) {
  basic_request<bool>("/bots/votes?userId=" + std::to_string(user_id), callback, [](const auto& j) {
    return j["voted"].template get<uint8_t>() != 0;
  });
}

TOPGG_API_ENDPOINT(is_weekend) {
  basic_request<bool>("/weekend", callback, [](const auto& j) {
    return j["is_weekend"].template get<bool>();
  });
}