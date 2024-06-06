#pragma once

#include <topgg/topgg.h>

#include <functional>
#include <vector>
#include <string>
#include <map>

#define TOPGG_API_CALLBACK(return_type, name) \
  using name##_completion_t = std::function<void(const result<return_type>&)>

#define TOPGG_API_ENDPOINT(name) \
  void name(const name##_completion_t& callback)

#define TOPGG_API_ENDPOINT_ARGS(name, ...) \
  void name(__VA_ARGS__, const name##_completion_t& callback)

namespace topgg {
  TOPGG_API_CALLBACK(bot, get_bot);
  TOPGG_API_CALLBACK(user, get_user);
  TOPGG_API_CALLBACK(stats, get_stats);
  TOPGG_API_CALLBACK(std::vector<voter>, get_voters);
  TOPGG_API_CALLBACK(bool, has_voted);
  TOPGG_API_CALLBACK(bool, is_weekend);

  using post_stats_completion_t = std::function<void(void)>;

  class TOPGG_EXPORT base_client {
  protected:
    std::multimap<std::string, std::string> m_headers;

    base_client(const std::string& token);

  public:
    base_client() = delete;
  };

  class TOPGG_EXPORT client: private base_client {
    dpp::cluster* m_cluster;

    template<typename T>
    void basic_request(const std::string& url, const std::function<void(const result<T>&)>& callback, const std::function<T(const dpp::json&)>& conversion_fn) {
      m_cluster->request("https://top.gg/api" + url, dpp::m_get, [callback, conversion_fn](const auto& response) { callback(result<T>{response, conversion_fn}); }, "", "application/json", m_headers);
    }

  public:
    client() = delete;

    inline client(dpp::cluster* cluster, const std::string& token)
      : base_client(token), m_cluster(cluster) {}

    TOPGG_API_ENDPOINT_ARGS(get_bot, const dpp::snowflake bot_id);
    TOPGG_API_ENDPOINT_ARGS(get_user, const dpp::snowflake user_id);
    TOPGG_API_ENDPOINT_ARGS(post_stats, const stats& s);
    TOPGG_API_ENDPOINT(get_stats);
    TOPGG_API_ENDPOINT(get_voters);
    TOPGG_API_ENDPOINT_ARGS(has_voted, const dpp::snowflake user_id);
    TOPGG_API_ENDPOINT(is_weekend);

    friend class autoposter::base;
  };
}; // namespace topgg

#undef TOPGG_API_CALLBACK
#undef TOPGG_API_ENDPOINT
#undef TOPGG_API_ENDPOINT_ARGS