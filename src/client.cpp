#include <topgg/topgg.h>

using topgg::client;

client::client(dpp::cluster& cluster, const std::string& token): m_token(token), m_cluster(cluster), m_autoposter_timer(0) {
  m_headers.insert(std::pair("Authorization", "Bearer " + token));
  m_headers.insert(std::pair("Connection", "close"));
  m_headers.insert(std::pair("Content-Type", "application/json"));
  m_headers.insert(std::pair("User-Agent", "topgg (https://github.com/top-gg-community/cpp-sdk) D++"));
}

void client::get_bot(const dpp::snowflake bot_id, const topgg::get_bot_completion_t& callback) {
  basic_request<topgg::bot>("/bots/" + std::to_string(bot_id), callback, [](const auto& j) {
    return topgg::bot{j};
  });
}

#ifdef DPP_CORO
topgg::async_result<topgg::bot> client::co_get_bot(const dpp::snowflake bot_id) {
  return topgg::async_result<topgg::bot>{ [this, bot_id] <typename C> (C&& cc) { return get_bot(bot_id, std::forward<C>(cc)); }};
}
#endif

void client::get_user(const dpp::snowflake user_id, const topgg::get_user_completion_t& callback) {
  basic_request<topgg::user>("/users/" + std::to_string(user_id), callback, [](const auto& j) {
    return topgg::user{j};
  });
}

#ifdef DPP_CORO
topgg::async_result<topgg::user> client::co_get_user(const dpp::snowflake user_id) {
  return topgg::async_result<topgg::user>{ [this, user_id] <typename C> (C&& cc) { return get_user(user_id, std::forward<C>(cc)); }};
}
#endif

void client::post_stats(const topgg::post_stats_completion_t& callback)  {
  post_stats(stats{m_cluster}, callback);
}

#ifdef DPP_CORO
dpp::async<bool> client::co_post_stats() {
  return dpp::async<bool>{ [this] <typename C> (C&& cc) { return post_stats(stats{m_cluster}, std::forward<C>(cc)); }};
}
#endif

void client::post_stats(const stats& s, const topgg::post_stats_completion_t& callback)  {
  auto headers = std::multimap<std::string, std::string>{m_headers};
  const auto s_json = s.to_json();

  headers.insert(std::pair("Content-Length", std::to_string(s_json.size())));

  m_cluster.request("https://top.gg/api/bots/stats", dpp::m_post, [callback](const auto& response) { callback(response.error == dpp::h_success && response.status < 400); }, s_json, "application/json", headers);
}

#ifdef DPP_CORO
dpp::async<bool> client::co_post_stats(const stats& s) {
  return dpp::async<bool>{ [this, s] <typename C> (C&& cc) { return post_stats(s, std::forward<C>(cc)); }};
}
#endif

void client::get_stats(const topgg::get_stats_completion_t& callback) {
  basic_request<topgg::stats>("/bots/stats", callback, [](const auto& j) {
    return topgg::stats{j};
  });
}

#ifdef DPP_CORO
topgg::async_result<topgg::stats> client::co_get_stats() {
  return topgg::async_result<topgg::stats>{ [this] <typename C> (C&& cc) { return get_stats(std::forward<C>(cc)); }};
}
#endif

void client::get_voters(const topgg::get_voters_completion_t& callback) {
  basic_request<std::vector<topgg::voter>>("/bots/votes", callback, [](const auto& j) {
    std::vector<topgg::voter> voters;

    for (const auto& part: j) {
      voters.push_back(topgg::voter{part});
    }

    return voters;
  });
}

#ifdef DPP_CORO
topgg::async_result<std::vector<topgg::voter>> client::co_get_voters() {
  return topgg::async_result<std::vector<topgg::voter>>{ [this] <typename C> (C&& cc) { return get_voters(std::forward<C>(cc)); }};
}
#endif


void client::has_voted(const dpp::snowflake user_id, const topgg::has_voted_completion_t& callback) {
  basic_request<bool>("/bots/votes?userId=" + std::to_string(user_id), callback, [](const auto& j) {
    return j["voted"].template get<uint8_t>() != 0;
  });
}

#ifdef DPP_CORO
topgg::async_result<bool> client::co_has_voted(const dpp::snowflake user_id) {
  return topgg::async_result<bool>{ [user_id, this] <typename C> (C&& cc) { return has_voted(user_id, std::forward<C>(cc)); }};
}
#endif

void client::is_weekend(const topgg::is_weekend_completion_t& callback) {
  basic_request<bool>("/weekend", callback, [](const auto& j) {
    return j["is_weekend"].template get<bool>();
  });
}

#ifdef DPP_CORO
topgg::async_result<bool> client::co_is_weekend() {
  return topgg::async_result<bool>{ [this] <typename C> (C&& cc) { return is_weekend(std::forward<C>(cc)); }};
}
#endif

void client::start_autoposter(const time_t delay) {
  start_autoposter([](dpp::cluster& bot) {
    return stats{bot};
  }, delay);
}

void client::start_autoposter(const topgg::custom_autopost_callback_t& callback, const time_t delay) {
  /**
   * Check the timer duration is not less than 15 minutes
   */
  if (delay < 15 * 60) {
    throw std::invalid_argument{"Delay mustn't be shorter than 15 minutes."};
  }
  
  /**
   * Create a D++ timer, this is managed by the D++ cluster and ticks every n seconds.
   * It can be stopped at any time without blocking, and does not need to create extra threads.
   */
  if (!m_autoposter_timer) {
    m_autoposter_timer = m_cluster.start_timer([this, callback](TOPGG_UNUSED dpp::timer) {
      const auto s = callback(m_cluster);
      const auto s_json = s.to_json();
      std::multimap<std::string, std::string> headers{m_headers};
      headers.insert(std::pair("Content-Length", std::to_string(s_json.length())));
      
      m_cluster.request("https://top.gg/api/bots/stats", dpp::m_post, [](TOPGG_UNUSED const auto&) {}, s_json, "application/json", headers);
    }, delay);
  }
}

void client::stop_autoposter() noexcept {
  if (m_autoposter_timer) {
    m_cluster.stop_timer(m_autoposter_timer);
    m_autoposter_timer = 0;
  }
}

client::~client() {
  stop_autoposter();
}