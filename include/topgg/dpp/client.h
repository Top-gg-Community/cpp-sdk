#ifndef __TOPGG_CPP_SDK_DPP_CLIENT_H__
#define __TOPGG_CPP_SDK_DPP_CLIENT_H__

#include <dpp/dpp.h>
#include <topgg/dpp.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace topgg {
#define TOPGG_API_CALLBACK(return_type, name) \
  typedef std::function<void(const result<return_type>&)> name##_completion_t

TOPGG_API_CALLBACK(bot, get_bot);
TOPGG_API_CALLBACK(user, get_user);
TOPGG_API_CALLBACK(stats, get_stats);
TOPGG_API_CALLBACK(std::vector<voter>, get_voters);
TOPGG_API_CALLBACK(bool, has_voted);
TOPGG_API_CALLBACK(bool, is_weekend);

class dpp_client {
  dpp::cluster* m_cluster;
  std::multimap<std::string, std::string> m_headers;

  template <typename T>
  void basic_request(const std::string& url,
                     std::function<void(const result<T>&)> callback,
                     std::function<T(const nlohmann::json&)> conversion_fn);

 public:
  dpp_client(dpp::cluster* cluster, const std::string& token);

  void get_bot(const dpp::snowflake& bot_id, get_bot_completion_t callback);
  void get_user(const dpp::snowflake& user_id, get_user_completion_t callback);
  void get_stats(get_stats_completion_t callback);
  void get_voters(get_voters_completion_t callback);
  void has_voted(const dpp::snowflake& user_id,
                 has_voted_completion_t callback);
  void is_weekend(is_weekend_completion_t callback);
};
};  // namespace topgg

#undef TOPGG_API_CALLBACK
#endif