#pragma once

#include <topgg/topgg.h>

#include <functional>
#include <vector>
#include <string>
#include <map>

namespace topgg {
  /**
   * @brief The callback function to call when get_bot completes.
   */
  using get_bot_completion_t = std::function<void(const result<bot>&)>;

  /**
   * @brief The callback function to call when get_user completes.
   */
  using get_user_completion_t = std::function<void(const result<user>&)>;

  /**
   * @brief The callback function to call when get_stats completes.
   */
  using get_stats_completion_t = std::function<void(const result<stats>&)>;

  /**
   * @brief The callback function to call when get_voters completes.
   */
  using get_voters_completion_t = std::function<void(const result<std::vector<voter>>&)>;

  /**
   * @brief The callback function to call when has_voted completes.
   */
  using has_voted_completion_t = std::function<void(const result<bool>&)>;

  /**
   * @brief The callback function to call when is_weekend completes.
   */
  using is_weekend_completion_t = std::function<void(const result<bool>&)>;

  /**
   * @brief The callback function to call when post_stats completes.
   */
  using post_stats_completion_t = std::function<void(void)>;

  /**
   * @brief Base client class for HTTP purposes.
   */
  class TOPGG_EXPORT base_client {
  protected:
    /**
     * @brief HTTP headers to use when making HTTP requests with the Top.gg API.
     */
    std::multimap<std::string, std::string> m_headers;

    /**
     * @brief Constructs the base client class.
     *
     * @param token The Top.gg API token to use.
     */
    base_client(const std::string& token);

  public:
    /**
     * @brief Don't use this class directly. Use its child classes!
     */
    base_client() = delete;
  };

  /**
   * @brief Main client class that lets you make HTTP requests with the Top.gg API.
   */
  class TOPGG_EXPORT client: private base_client {
    dpp::cluster* m_cluster;

    template<typename T>
    void basic_request(const std::string& url, const std::function<void(const result<T>&)>& callback, const std::function<T(const dpp::json&)>& conversion_fn) {
      m_cluster->request("https://top.gg/api" + url, dpp::m_get, [callback, conversion_fn](const auto& response) { callback(result<T>{response, conversion_fn}); }, "", "application/json", m_headers);
    }

  public:
    /**
     * @brief That's not how you initiate the class buddy :)
     */
    client() = delete;

    /**
     * @brief Constructs the client class.
     *
     * @param cluster A pointer to the bot's D++ cluster using this library.
     * @param token The Top.gg API token to use.
     */
    inline client(dpp::cluster* cluster, const std::string& token)
      : base_client(token), m_cluster(cluster) {}

    /**
     * @brief Fetches a listed Discord bot from a Discord ID.
     *
     * @param bot_id The Discord bot ID to fetch from.
     * @param callback The callback function to call when get_bot completes.
     */
    void get_bot(const dpp::snowflake bot_id, const get_bot_completion_t& callback);

    /**
     * @brief Fetches a user from a Discord ID.
     *
     * @param user_id The Discord user ID to fetch from.
     * @param callback The callback function to call when get_user completes.
     */
    void get_user(const dpp::snowflake user_id, const get_user_completion_t& callback);

    /**
     * @brief Fetches your Discord bot’s statistics.
     *
     * @param callback The callback function to call when get_stats completes.
     */
    void get_stats(const get_stats_completion_t& callback);

    /**
     * @brief Fetches your Discord bot’s last 1000 voters.
     *
     * @param callback The callback function to call when get_voters completes.
     */
    void get_voters(const get_voters_completion_t& callback);

    /**
     * @brief Checks if the specified user has voted your Discord bot.
     *
     * @param user_id The Discord user ID to check from.
     * @param callback The callback function to call when has_voted completes.
     */
    void has_voted(const dpp::snowflake user_id, const has_voted_completion_t& callback);

    /**
     * @brief Checks if the weekend multiplier is active.
     *
     * @param callback The callback function to call when is_weekend completes.
     */
    void is_weekend(const is_weekend_completion_t& callback);

    /**
     * @brief Manually posts your Discord bot's statistics.
     *
     * @param s Your Discord bot's statistics.
     * @param callback The callback function to call when post_stats completes.
     */
    void post_stats(const stats& s, const post_stats_completion_t& callback);

    friend class autoposter::base;
  };
}; // namespace topgg