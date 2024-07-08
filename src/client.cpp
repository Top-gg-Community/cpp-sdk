#include <topgg/topgg.h>

using topgg::base_client;
using topgg::client;

base_client::base_client(const std::string& token) {
  m_headers.insert(std::pair("Authorization", "Bearer " + token));
  m_headers.insert(std::pair("Connection", "close"));
  m_headers.insert(std::pair("Content-Type", "application/json"));
  m_headers.insert(std::pair("User-Agent", "topgg (https://github.com/top-gg-community/cpp-sdk) D++"));
}

#define TOPGG_API_ENDPOINT(name) \
  void client::name(const topgg::name##_completion_t& callback)

#define TOPGG_API_ENDPOINT_ARGS(name, ...) \
  void client::name(__VA_ARGS__, const topgg::name##_completion_t& callback)

TOPGG_API_ENDPOINT_ARGS(get_bot, const dpp::snowflake bot_id) {
  basic_request<topgg::bot>("/bots/" + std::to_string(bot_id), callback, [](const auto& j) {
    return topgg::bot{j};
  });
}

#ifdef DPP_CORO
dpp::async<topgg::result<topgg::bot>> client::co_get_bot(const dpp::snowflake bot_id) {
	return dpp::async<topgg::result<topgg::bot>>{ [this, bot_id] <typename C> (C &&cc) { return get_bot(bot_id, std::forward<C>(cc)); }};
}
#endif

TOPGG_API_ENDPOINT_ARGS(get_user, const dpp::snowflake user_id) {
  basic_request<topgg::user>("/users/" + std::to_string(user_id), callback, [](const auto& j) {
    return topgg::user{j};
  });
}

#ifdef DPP_CORO
dpp::async<topgg::result<topgg::user>> client::co_get_user(const dpp::snowflake user_id) {
	return dpp::async<topgg::result<topgg::user>>{ [this, user_id] <typename C> (C &&cc) { return get_user(user_id, std::forward<C>(cc)); }};
}
#endif

TOPGG_API_ENDPOINT_ARGS(post_stats, const stats& s) {
  auto headers = std::multimap<std::string, std::string>{m_headers};
  const auto s_json = s.to_json();

  headers.insert(std::pair("Content-Length", std::to_string(s_json.size())));

  m_cluster->request("https://top.gg/api/bots/stats", dpp::m_post, [callback](TOPGG_UNUSED const auto& _) { callback(true); }, s_json, "application/json", headers);
}

#ifdef DPP_CORO
dpp::async<bool> client::co_post_stats(const stats& s) {
	return dpp::async<bool>{ [this, s] <typename C> (C &&cc) { return post_stats(s, std::forward<C>(cc)); }};
}
#endif

TOPGG_API_ENDPOINT(get_stats) {
  basic_request<topgg::stats>("/bots/stats", callback, [](const auto& j) {
    return topgg::stats{j};
  });
}

#ifdef DPP_CORO
dpp::async<topgg::result<topgg::stats>> client::co_get_stats() {
	return dpp::async<topgg::result<topgg::stats>>{ [this] <typename C> (C &&cc) { return get_stats(std::forward<C>(cc)); }};
}
#endif

TOPGG_API_ENDPOINT(get_voters) {
  basic_request<std::vector<topgg::voter>>("/bots/votes", callback, [](const auto& j) {
    std::vector<topgg::voter> voters;

    for (const auto& part: j) {
      voters.push_back(topgg::voter{part});
    }

    return voters;
  });
}

#ifdef DPP_CORO
dpp::async<topgg::result<std::vector<topgg::voter>>> client::co_get_voters() {
	return dpp::async<topgg::result<std::vector<topgg::voter>>>{ [this] <typename C> (C &&cc) { return get_voters(std::forward<C>(cc)); }};
}
#endif


TOPGG_API_ENDPOINT_ARGS(has_voted, const dpp::snowflake user_id) {
  basic_request<bool>("/bots/votes?userId=" + std::to_string(user_id), callback, [](const auto& j) {
    return j["voted"].template get<uint8_t>() != 0;
  });
}

#ifdef DPP_CORO
dpp::async<topgg::result<bool>> client::co_has_voted(const dpp::snowflake user_id) {
	return dpp::async<topgg::result<bool>>{ [user_id, this] <typename C> (C &&cc) { return has_voted(user_id, std::forward<C>(cc)); }};
}
#endif

TOPGG_API_ENDPOINT(is_weekend) {
  basic_request<bool>("/weekend", callback, [](const auto& j) {
    return j["is_weekend"].template get<bool>();
  });
}

#ifdef DPP_CORO
dpp::async<topgg::result<bool>> client::co_is_weekend() {
	return dpp::async<topgg::result<bool>>{ [this] <typename C> (C &&cc) { return is_weekend(std::forward<C>(cc)); }};
}
#endif