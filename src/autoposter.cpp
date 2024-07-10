#include <topgg/topgg.h>
#include <stdexcept>

using topgg::autoposter::base;
using topgg::autoposter::cached;
using topgg::stats;

void base::stop() {
  if (m_timer_handle) {
    m_cluster->stop_timer(m_timer_handle);
  }
  m_timer_handle = 0;
}

base::~base() {
  base::stop();
}


base::base(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const time_t delay)
  : ::topgg::base_client(token), m_cluster(std::shared_ptr{cluster}) {
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
  this->m_timer_handle = cluster->start_timer([this, cluster, token](dpp::timer) {
    if (this->before_fetch()) {
      const auto s = get_stats(cluster.get());
      after_fetch();
      const auto s_json = s.to_json();
      std::multimap<std::string, std::string> headers{m_headers};
      headers.insert(std::pair("Content-Length", std::to_string(s_json.size())));
      cluster->request("https://top.gg/api/bots/stats", dpp::m_post, [](TOPGG_UNUSED const auto&) {}, s_json, "application/json", headers);
    }
  }, delay);
}

::topgg::stats cached::get_stats(TOPGG_UNUSED dpp::cluster* bot) {
  /**
   * Count guilds and shards using D++ guild cache and shards map
   */
  size_t servers = 0;
  for (auto & s : bot->get_shards()) {
    servers += s.second->get_guild_count();
  }
  return ::topgg::stats{servers, bot->get_shards().size()};
}

cached::cached(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const time_t delay) : base(cluster, token, delay) {
}
