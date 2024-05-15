#include <topgg/dpp.h>

using topgg::client;

client::client(dpp::cluster* cluster, const std::string& token): m_cluster(cluster) {
  m_headers.insert(std::pair("Authorization", "Bearer " + token));
  m_headers.insert(std::pair("Connection", "close"));
  m_headers.insert(std::pair("Content-Type", "application/json"));
  m_headers.insert(std::pair("User-Agent", "topgg (https://github.com/top-gg-community/cpp-sdk) D++"));
}

template<typename T>
void client::basic_request(const std::string& url, std::function<void(const result<T>&)> callback, std::function<T(const dpp::json&)> conversion_fn) {
  m_cluster->request("https://top.gg/api" + url, dpp::m_get, [callback, conversion_fn](const auto& response) {
    callback(result<T>{response, conversion_fn});
  }, "", "application/json", m_headers);
}

void client::get_bot(const dpp::snowflake& bot_id, topgg::get_bot_completion_t callback) {
  basic_request<topgg::bot>("/bots/" + std::to_string(bot_id), callback, [](const auto& j) {
    return topgg::bot{j};
  });
}

void client::get_user(const dpp::snowflake& user_id, topgg::get_user_completion_t callback) {
  basic_request<topgg::user>("/users/" + std::to_string(user_id), callback, [](const auto& j) {
    return topgg::user{j};
  });
}

void client::post_stats(const stats& s, topgg::post_stats_completion_t callback) {
  auto headers = std::multimap<std::string, std::string>{m_headers};
  const auto s_json = s.to_json();
  
  headers.insert(std::pair("Content-Length", std::to_string(s_json.size())));
  
  m_cluster->request("https://top.gg/api/bots/stats", dpp::m_post, [callback](const auto& _) {
    callback();
  }, s_json, "application/json", headers);
}

void client::get_stats(topgg::get_stats_completion_t callback) {
  basic_request<topgg::stats>("/bots/stats", callback, [](const auto& j) {
    return topgg::stats{j};
  });
}

void client::get_voters(topgg::get_voters_completion_t callback) {
  basic_request<std::vector<topgg::voter>>("/bots/votes", callback, [](const auto& j) {
    std::vector<topgg::voter> voters;
    
    for (const auto& part: j) {
      voters.push_back(topgg::voter{part});
    }
    
    return voters;
  });
}

void client::has_voted(const dpp::snowflake& user_id, topgg::has_voted_completion_t callback) {
  basic_request<bool>("/bots/votes?userId=" + std::to_string(user_id), callback, [](const auto& j) {
    return j["voted"].template get<uint8_t>() != 0;
  });
}

void client::is_weekend(topgg::is_weekend_completion_t callback) {
  basic_request<bool>("/weekend", callback, [](const auto& j) {
    return j["is_weekend"].template get<bool>();
  });
}