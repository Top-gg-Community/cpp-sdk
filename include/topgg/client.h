/**
 * @module topgg
 * @file client.h
 * @brief The official C++ wrapper for the Top.gg API.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024 Top.gg & null8626
 * @date 2024-07-09
 * @version 2.0.0
 */

#pragma once

#include <topgg/topgg.h>

#include <functional>
#include <vector>
#include <string>
#include <map>

namespace topgg {
  /**
   * @brief The callback function to call when get_bot completes.
   *
   * @see topgg::client::get_bot
   * @since 2.0.0
   */
  using get_bot_completion_t = std::function<void(const result<bot>&)>;

  /**
   * @brief The callback function to call when get_user completes.
   *
   * @see topgg::client::get_user
   * @since 2.0.0
   */
  using get_user_completion_t = std::function<void(const result<user>&)>;

  /**
   * @brief The callback function to call when get_stats completes.
   *
   * @see topgg::client::get_stats
   * @since 2.0.0
   */
  using get_stats_completion_t = std::function<void(const result<stats>&)>;

  /**
   * @brief The callback function to call when get_voters completes.
   *
   * @see topgg::client::get_voters
   * @since 2.0.0
   */
  using get_voters_completion_t = std::function<void(const result<std::vector<voter>>&)>;

  /**
   * @brief The callback function to call when has_voted completes.
   *
   * @see topgg::client::has_voted
   * @since 2.0.0
   */
  using has_voted_completion_t = std::function<void(const result<bool>&)>;

  /**
   * @brief The callback function to call when is_weekend completes.
   *
   * @see topgg::client::is_weekend
   * @since 2.0.0
   */
  using is_weekend_completion_t = std::function<void(const result<bool>&)>;
  
  /**
   * @brief The callback function to call when post_stats completes.
   *
   * @see topgg::client::post_stats
   * @since 2.0.0
   */
  using post_stats_completion_t = std::function<void(const bool)>;
  
  /**
   * @brief The callback function that retrieves the bot's stats.
   *
   * @see topgg::client::start_autoposter
   * @since 2.0.0
   */
  using custom_autopost_callback_t = std::function<::topgg::stats(dpp::cluster&)>;
  
  /**
   * @brief Main client class that lets you make HTTP requests with the Top.gg API.
   *
   * @since 2.0.0
   */
  class TOPGG_EXPORT client {
    std::multimap<std::string, std::string> m_headers;
    std::string m_token;
    dpp::cluster& m_cluster;
    dpp::timer m_autoposter_timer;

    template<typename T>
    void basic_request(const std::string& url, const std::function<void(const result<T>&)>& callback, const std::function<T(const dpp::json&)>& conversion_fn) {
      m_cluster.request("https://top.gg/api" + url, dpp::m_get, [callback, conversion_fn](const auto& response) { callback(result<T>{response, conversion_fn}); }, "", "application/json", m_headers);
    }

  public:
    client() = delete;

    /**
     * @brief Constructs the client class.
     *
     * @param cluster A pointer to the bot's D++ cluster using this library.
     * @param token The Top.gg API token to use.
     * @since 2.0.0
     */
    client(dpp::cluster& cluster, const std::string& token);

    /**
     * @brief Fetches a listed Discord bot from a Discord ID.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * topgg_client.get_bot(264811613708746752, [](const auto& result) {
     *   try {
     *     const auto topgg_bot = result.get();
     *
     *     std::cout << topgg_bot.username << std::endl;
     *   } catch (const std::exception& ext) {
     *     std::cout << "error: " << ext.what() << std::endl;
     *   }
     * });
     * ```
     *
     * @param bot_id The Discord bot ID to fetch from.
     * @param callback The callback function to call when get_bot completes.
     * @note For its C++20 coroutine counterpart, see co_get_bot.
     * @see topgg::result
     * @see topgg::bot
     * @see topgg::client::co_get_bot
     * @since 2.0.0
     */
    void get_bot(const dpp::snowflake bot_id, const get_bot_completion_t& callback);

#ifdef DPP_CORO
    /**
     * @brief Fetches a listed Discord bot from a Discord ID through a C++20 coroutine.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * auto result = co_await topgg_client.get_bot(264811613708746752);
     *
     * try {
     *   const auto topgg_bot = result.get();
     *   std::cout << topgg_bot.username << std::endl;
     * } catch (const std::exception& ext) {
     *   std::cout << "error: " << ext.what() << std::endl;
     * }
     * ```
     *
     * @param bot_id The Discord bot ID to fetch from.
     * @return co_await to retrieve a topgg::result<topgg::bot>
     * @note For its C++17 callback-based counterpart, see get_bot.
     * @see topgg::result
     * @see topgg::bot
     * @see topgg::client::get_bot
     * @since 2.0.0
     */
    dpp::async<topgg::result<topgg::bot>> co_get_bot(const dpp::snowflake bot_id);
#endif

    /**
     * @brief Fetches a user from a Discord ID.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * topgg_client.get_user(661200758510977084, [](const auto& result) {
     *   try {
     *     const auto user = result.get();
     *
     *     std::cout << user.username << std::endl;
     *   } catch (const std::exception& ext) {
     *     std::cout << "error: " << ext.what() << std::endl;
     *   }
     * });
     * ```
     *
     * @param user_id The Discord user ID to fetch from.
     * @param callback The callback function to call when get_user completes.
     * @note For its C++20 coroutine counterpart, see co_get_user.
     * @see topgg::result
     * @see topgg::user
     * @see topgg::co_get_user
     * @since 2.0.0
     */
    void get_user(const dpp::snowflake user_id, const get_user_completion_t& callback);

#ifdef DPP_CORO
    /**
     * @brief Fetches a user from a Discord ID through a C++20 coroutine.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * const auto result = co_await topgg_client.get_user(661200758510977084);
     *
     * try {
     *   const auto user = result.get();
     *   std::cout << user.username << std::endl;
     * } catch (const std::exception& ext) {
     *   std::cout << "error: " << ext.what() << std::endl;
     * }
     * ```
     *
     * @param user_id The Discord user ID to fetch from.
     * @return co_await to retrieve a topgg::result<topgg::user>
     * @note For its C++17 callback-based counterpart, see get_user.
     * @see topgg::result
     * @see topgg::bot
     * @see topgg::client::get_user
     * @since 2.0.0
     */
    dpp::async<topgg::result<topgg::user>> co_get_user(const dpp::snowflake user_id);
#endif

    /**
     * @brief Fetches your Discord bot’s statistics.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * topgg_client.get_stats([](const auto& result) {
     *   try {
     *     auto stats = result.get();
     *
     *     std::cout << stats.server_count().value_or(0) << std::endl;
     *   } catch (const std::exception& ext) {
     *     std::cout << "error: " << ext.what() << std::endl;
     *   }
     * });
     * ```
     *
     * @param callback The callback function to call when get_stats completes.
     * @note For its C++20 coroutine counterpart, see co_get_stats.
     * @see topgg::result
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @see topgg::client::co_get_stats
     * @since 2.0.0
     */
    void get_stats(const get_stats_completion_t& callback);

#ifdef DPP_CORO
    /**
     * @brief Fetches your Discord bot’s statistics through a C++20 coroutine.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * const auto result = co_await topgg_client.get_stats();
     *
     * try {
     *   auto stats = result.get();
     *   std::cout << stats.server_count().value_or(0) << std::endl;
     * } catch (const std::exception& ext) {
     *   std::cout << "error: " << ext.what() << std::endl;
     * }
     * ```
     *
     * @return co_await to retrieve a topgg::result<topgg::stats>
     * @note For its C++17 callback-based counterpart, see get_stats.
     * @see topgg::result
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @see topgg::client::get_stats
     * @since 2.0.0
     */
    dpp::async<topgg::result<topgg::stats>> co_get_stats();
#endif

    /**
     * @brief Fetches your Discord bot’s last 1000 voters.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * topgg_client.get_voters([](const auto& result) {
     *   try {
     *     auto voters = result.get();
     *
     *     for (auto& voter: voters) {
     *       std::cout << voter.username << std::endl;
     *     }
     *   } catch (const std::exception& ext) {
     *     std::cout << "error: " << ext.what() << std::endl;
     *   }
     * });
     * ```
     *
     * @param callback The callback function to call when get_voters completes.
     * @note For its C++20 coroutine counterpart, see co_get_voters.
     * @see topgg::result
     * @see topgg::voter
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @see topgg::client::co_get_voters
     * @since 2.0.0
     */
    void get_voters(const get_voters_completion_t& callback);

#ifdef DPP_CORO
    /**
     * @brief Fetches your Discord bot’s last 1000 voters through a C++20 coroutine.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * const auto result = co_await topgg_client.get_voters();
     *
     * try {
     *   auto voters = result.get();
     *   for (auto& voter: voters) {
     *     std::cout << voter.username << std::endl;
     *   }
     * } catch (const std::exception& ext) {
     *   std::cout << "error: " << ext.what() << std::endl;
     * }
     * ```
     *
     * @return co_await to retrieve a topgg::result<topgg::stats>
     * @note For its C++17 callback-based counterpart, see get_voters.
     * @see topgg::result
     * @see topgg::voter
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @see topgg::client::get_voters
     * @since 2.0.0
     */
    dpp::async<topgg::result<std::vector<voter>>> co_get_voters();
#endif

    /**
     * @brief Checks if the specified user has voted your Discord bot.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * topgg_client.has_voted(661200758510977084, [](const auto& result) {
     *   try {
     *     if (result.get()) {
     *       std::cout << "checks out" << std::endl;
     *     }
     *   } catch (const std::exception& ext) {
     *     std::cout << "error: " << ext.what() << std::endl;
     *   }
     * });
     * ```
     *
     * @param user_id The Discord user ID to check from.
     * @param callback The callback function to call when has_voted completes.
     * @note For its C++20 coroutine counterpart, see co_has_voted.
     * @see topgg::result
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @note For its C++20 coroutine counterpart, see co_has_voted.
     * @since 2.0.0
     */
    void has_voted(const dpp::snowflake user_id, const has_voted_completion_t& callback);

#ifdef DPP_CORO
    /**
     * @brief Checks if the specified user has voted your Discord bot through a C++20 coroutine.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * const auto result = co_await topgg_client.has_voted(661200758510977084);
     *
     * try {
     *   if (result.get()) {
     *     std::cout << "checks out" << std::endl;
     *   }
     * } catch (const std::exception& ext) {
     *   std::cout << "error: " << ext.what() << std::endl;
     * }
     * ```
     *
     * @param user_id The Discord user ID to check from.
     * @return co_await to retrieve a topgg::result<bool>
     * @note For its C++17 callback-based counterpart, see has_voted.
     * @see topgg::result
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @see topgg::client::has_voted
     * @since 2.0.0
     */
    dpp::async<topgg::result<bool>> co_has_voted(const dpp::snowflake user_id);
#endif

    /**
     * @brief Checks if the weekend multiplier is active.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * topgg_client.is_weekend([](const auto& result) {
     *   try {
     *     if (result.get()) {
     *       std::cout << "the weekend multiplier is active" << std::endl;
     *     }
     *   } catch (const std::exception& ext) {
     *     std::cout << "error: " << ext.what() << std::endl;
     *   }
     * });
     * ```
     *
     * @param callback The callback function to call when is_weekend completes.
     * @note For its C++20 coroutine counterpart, see co_is_weekend.
     * @see topgg::result
     * @see topgg::client::co_is_weekend
     * @since 2.0.0
     */
    void is_weekend(const is_weekend_completion_t& callback);

#ifdef DPP_CORO
    /**
     * @brief Checks if the weekend multiplier is active through a C++20 coroutine.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * const auto result = co_await topgg_client.is_weekend();
     *
     * try {
     *   if (result.get()) {
     *     std::cout << "the weekend multiplier is active" << std::endl;
     *   }
     * } catch (const std::exception& ext) {
     *   std::cout << "error: " << ext.what() << std::endl;
     * }
     * ```
     *
     * @return co_await to retrieve a topgg::result<bool>
     * @note For its C++17 callback-based counterpart, see is_weekend.
     * @see topgg::result
     * @see topgg::client::is_weekend
     * @since 2.0.0
     */
    dpp::async<topgg::result<bool>> co_is_weekend();
#endif

    /**
     * @brief Manually posts your Discord bot's statistics using data directly from your D++ cluster instance.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * topgg_client.post_stats([](const auto success) {
     *   if (success) {
     *     std::cout << "stats posted!" << std::endl;
     *   }
     * });
     * ```
     *
     * @param callback The callback function to call when post_stats completes.
     * @note For its C++20 coroutine counterpart, see co_post_stats.
     * @see topgg::result
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @see topgg::client::co_post_stats
     * @since 2.0.0
     */
    void post_stats(const post_stats_completion_t& callback);

#ifdef DPP_CORO
    /**
     * @brief Manually posts your Discord bot's statistics using data directly from your D++ cluster instance through a C++20 coroutine.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * const auto success = co_await topgg_client.post_stats();
     *
     * if (success) {
     *   std::cout << "stats posted!" << std::endl;
     * }
     * ```
     *
     * @return co_await to retrieve a bool
     * @note For its C++17 callback-based counterpart, see post_stats.
     * @see topgg::result
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @see topgg::client::post_stats
     * @since 2.0.0
     */
    dpp::async<bool> co_post_stats();
#endif

    /**
     * @brief Manually posts your Discord bot's statistics.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * const size_t server_count = 12345;
     *
     * topgg_client.post_stats(topgg::stats{server_count}, [](const auto success) {
     *   if (success) {
     *     std::cout << "stats posted!" << std::endl;
     *   }
     * });
     * ```
     *
     * @param s Your Discord bot's statistics.
     * @param callback The callback function to call when post_stats completes.
     * @note For its C++20 coroutine counterpart, see co_post_stats.
     * @see topgg::result
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @see topgg::client::co_post_stats
     * @since 2.0.0
     */
    void post_stats(const stats& s, const post_stats_completion_t& callback);

#ifdef DPP_CORO
    /**
     * @brief Manually posts your Discord bot's statistics through a C++20 coroutine.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * const size_t server_count = 12345;
     * const auto success = co_await topgg_client.post_stats(topgg::stats{server_count});
     *
     * if (success) {
     *   std::cout << "stats posted!" << std::endl;
     * }
     * ```
     *
     * @param s Your Discord bot's statistics.
     * @return co_await to retrieve a bool
     * @note For its C++17 callback-based counterpart, see post_stats.
     * @see topgg::result
     * @see topgg::stats
     * @see topgg::client::start_autoposter
     * @see topgg::client::post_stats
     * @since 2.0.0
     */
    dpp::async<bool> co_post_stats(const stats& s);
#endif

    /**
     * @brief Starts autoposting statistics using data directly from your D++ cluster instance.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * bot.start_autoposter();
     * ```
     *
     * @param delay The minimum delay between post requests in seconds. Defaults to 30 minutes.
     * @throw std::invalid_argument Throws if the delay argument is shorter than 15 minutes.
     * @note This function has no effect if the autoposter is already running.
     * @see topgg::client::post_stats
     * @see topgg::client::stop_autoposter
     * @since 2.0.0
     */
    void start_autoposter(const time_t delay = 1800);
    
    /**
     * @brief Starts autoposting statistics.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * bot.start_autoposter([](dpp::cluster& bot_inner) {
     *   return topgg::stats{...};
     * });
     * ```
     *
     * @param callback The callback function that returns the current stats.
     * @param delay The minimum delay between post requests in seconds. Defaults to 30 minutes.
     * @throw std::invalid_argument Throws if the delay argument is shorter than 15 minutes.
     * @note This function has no effect if the autoposter is already running.
     * @see topgg::stats
     * @see topgg::client::post_stats
     * @see topgg::client::stop_autoposter
     * @since 2.0.0
     */
    void start_autoposter(const custom_autopost_callback_t& callback, const time_t delay = 1800);
    
    /**
     * @brief Prematurely stops the autoposter. Calling this function is usually unnecessary as this function is called later in the destructor.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client topgg_client{bot, "your top.gg token"};
     *
     * bot.start_autoposter();
     *
     * // ...
     *
     * bot.stop_autoposter();
     * ```
     *
     * @note This function has no effect if the autoposter is already stopped.
     * @see topgg::client::post_stats
     * @since 2.0.0
     */
    void stop_autoposter() noexcept;
    
    /**
     * @brief The destructor. Stops the autoposter if it's running.
     */
    ~client();
  };
}; // namespace topgg
