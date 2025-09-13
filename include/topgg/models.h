/**
 * @module topgg
 * @file models.h
 * @brief A simple API wrapper for Top.gg written in C++.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024-2025 Top.gg & null8626
 * @date 2025-09-13
 * @version 3.0.0
 */

#pragma once

#include <topgg/topgg.h>

#include <unordered_map>
#include <optional>
#include <string>
#include <vector>

#if !defined(_WIN32) && !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE
#endif

#include <ctime>

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#ifdef __TOPGG_BUILDING__
#define _TOPGG_SNOWFLAKE_FROM_JSON(j, name) \
  dpp::snowflake{j[#name].template get<std::string>()}
#endif

#define TOPGG_BOT_QUERY_SORT(lib_name, api_name)               \
  inline bot_query& sort_by_##lib_name() noexcept {            \
    m_sort = #api_name;                                        \
    return *this;                                              \
  }

#define TOPGG_BOT_QUERY_QUERY(type, lib_name, api_name, ...)   \
  inline bot_query& lib_name(const type lib_name) {            \
    add_query(#api_name, lib_name, __VA_ARGS__);               \
    return *this;                                              \
  }

namespace topgg {
  class bot_query;
  class client;

  /**
   * @brief A Top.gg voter.
   *
   * @see topgg::vote
   * @see topgg::client::get_vote
   * @see topgg::client::get_voters
   * @since 2.0.0
   */
  class TOPGG_EXPORT voter {
    voter(const dpp::json& j);

  public:
    voter() = delete;

    /**
     * @brief This voter's ID.
     *
     * @since 2.0.0
     */
    dpp::snowflake id;

    /**
     * @brief This voter's avatar URL.
     *
     * @since 2.0.0
     */
    std::string avatar;

    /**
     * @brief This voter's username.
     *
     * @since 3.0.0
     */
    std::string username;

    /**
     * @brief This voter's creation date.
     *
     * @since 2.0.0
     */
    time_t created_at;

    friend class client;
  };

  /**
   * @brief A Discord bot listed on Top.gg.
   *
   * @see topgg::client::get_bot
   * @since 2.0.0
   */
  class TOPGG_EXPORT bot {
    bot(const dpp::json& j);

  public:
    bot() = delete;

    /**
     * @brief This bot's Discord ID.
     *
     * @since 2.0.0
     */
    dpp::snowflake id;

    /**
     * @brief This bot's Top.gg ID.
     *
     * @since 3.0.0
     */
    dpp::snowflake topgg_id;

    /**
     * @brief This bot's avatar URL.
     *
     * @since 2.0.0
     */
    std::string avatar;

    /**
     * @brief This bot's username.
     *
     * @since 3.0.0
     */
    std::string username;

    /**
     * @brief This bot's creation date.
     *
     * @since 2.0.0
     */
    time_t created_at;

    /**
     * @brief This bot's prefix.
     *
     * @since 2.0.0
     */
    std::string prefix;

    /**
     * @brief This bot's short description.
     *
     * @since 2.0.0
     */
    std::string short_description;

    /**
     * @brief This bot's long description.
     *
     * @note This long description can contain Markdown and/or HTML.
     * @since 2.0.0
     */
    std::optional<std::string> long_description;

    /**
     * @brief This bot's tags.
     *
     * @since 2.0.0
     */
    std::vector<std::string> tags;

    /**
     * @brief This bot's website URL.
     *
     * @since 2.0.0
     */
    std::optional<std::string> website;

    /**
     * @brief This bot's GitHub repository URL.
     *
     * @since 2.0.0
     */
    std::optional<std::string> github;

    /**
     * @brief This bot's owner IDs.
     *
     * @since 2.0.0
     */
    std::vector<dpp::snowflake> owners;

    /**
     * @brief This bot's submission date.
     *
     * @since 3.0.0
     */
    time_t submitted_at;

    /**
     * @brief The amount of votes this bot has.
     *
     * @since 2.0.0
     */
    size_t votes;

    /**
     * @brief The amount of votes this bot has this month.
     *
     * @since 2.0.0
     */
    size_t monthly_votes;

    /**
     * @brief This bot's support URL.
     *
     * @since 2.0.0
     */
    std::optional<std::string> support;

    /**
     * @brief This bot's invite URL.
     *
     * @since 2.0.0
     */
    std::optional<std::string> invite;

    /**
     * @brief This bot's Top.gg vanity code.
     *
     * @since 3.0.0
     */
    std::optional<std::string> vanity;

    /**
     * @brief This bot's posted server count.
     *
     * @since 3.0.0
     */
    std::optional<size_t> server_count;

    /**
     * @brief This bot's average review score out of 5.
     *
     * @since 3.0.0
     */
    double review_score;

    /**
     * @brief This bot's review count.
     *
     * @since 3.0.0
     */
    size_t review_count;

    friend class bot_query;
    friend class client;
  };

  /**
   * @brief The callback function to call when get_bots completes.
   *
   * @see topgg::client::get_bots
   * @see topgg::bot_query
   * @since 2.0.1
   */
  using get_bots_completion_event = std::function<void(const result<std::vector<bot>>&)>;

  /**
   * @brief Configure a Discord bot query before sending it to the API.
   *
   * @see topgg::client::get_bots
   * @since 2.0.1
   */
  class TOPGG_EXPORT bot_query {
    client* m_client;
    std::unordered_map<const char*, std::string> m_query;
    const char* m_sort;

    inline bot_query(client* c)
      : m_client(c), m_sort(nullptr) {}

    void add_query(const char* key, const uint16_t value, const uint16_t max);

  public:
    bot_query() = delete;

    /**
     * @brief Sorts results based on each bot's ID.
     *
     * @return bot_query The current modified object.
     * @see topgg::client::get_bots
     * @since 2.0.1
     */
    TOPGG_BOT_QUERY_SORT(id, id);

    /**
     * @brief Sorts results based on each bot's submission date.
     *
     * @return bot_query The current modified object.
     * @see topgg::client::get_bots
     * @since 3.0.0
     */
    TOPGG_BOT_QUERY_SORT(submission_date, date);

    /**
     * @brief Sorts results based on each bot's monthly vote count.
     *
     * @return bot_query The current modified object.
     * @see topgg::client::get_bots
     * @since 2.0.1
     */
    TOPGG_BOT_QUERY_SORT(monthly_votes, monthlyPoints);

    /**
     * @brief Sets the maximum amount of bots to be queried.
     *
     * @param limit The maximum amount of bots to be queried. This cannot be more than 500.
     * @return bot_query The current modified object.
     * @see topgg::client::get_bots
     * @since 2.0.1
     */
    TOPGG_BOT_QUERY_QUERY(uint16_t, limit, limit, 500);

    /**
     * @brief Sets the amount of bots to be skipped.
     *
     * @param skip The amount of bots to be skipped. This cannot be more than 499.
     * @return bot_query The current modified object.
     * @see topgg::client::get_bots
     * @since 2.0.1
     */
    TOPGG_BOT_QUERY_QUERY(uint16_t, skip, offset, 499);

    /**
     * @brief Sends the query to the API.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client client{bot, "your top.gg token"};
     *
     * client
     *   .get_bots()
     *   .limit(250)
     *   .skip(50)
     *   .sort_by_monthly_votes()
     *   .send([](const auto& result) {
     *     try {
     *       const auto bots{result.get()};
     *
     *       for (const auto& bot: bots) {
     *         std::cout << bot.username << std::endl;
     *       }
     *     } catch (const std::exception& exc) {
     *       std::cerr << "error: " << exc.what() << std::endl;
     *     }
     *   });
     * ```
     *
     * @param callback The callback function to call when send() completes.
     * @note For its C++20 coroutine counterpart, see co_send().
     * @see topgg::client::get_bots
     * @see topgg::bot_query::co_send
     * @since 2.0.1
     */
    void send(const get_bots_completion_event& callback);

#ifdef DPP_CORO
    /**
     * @brief Sends the query to the API through a C++20 coroutine.
     *
     * Example:
     *
     * ```cpp
     * dpp::cluster bot{"your bot token"};
     * topgg::client client{bot, "your top.gg token"};
     *
     * try {
     *   const auto bots = co_await client
     *     .get_bots()
     *     .limit(250)
     *     .skip(50)
     *     .sort_by_monthly_votes()
     *     .send();
     *
     *   for (const auto& bot: bots) {
     *     std::cout << topgg_bot.username << std::endl;
     *   }
     * } catch (const std::exception& exc) {
     *   std::cerr << "error: " << exc.what() << std::endl;
     * }
     * ```
     *
     * @throw topgg::internal_server_error Unexpected error from Top.gg's end.
     * @throw topgg::invalid_token Invalid API token.
     * @throw topgg::ratelimited Ratelimited from sending more requests.
     * @throw dpp::http_error An unexpected HTTP exception has occured.
     * @return co_await to retrieve a vector of topgg::bot if successful
     * @note For its C++17 callback-based counterpart, see get_bot.
     * @see topgg::client::get_bots
     * @see topgg::bot_query::send
     * @since 2.0.1
     */
    topgg::async_result<std::vector<topgg::bot>> co_send();
#endif

    friend class client;
  };

  /**
   * @brief An abstract interface for bots that have a custom way of retrieving their server count.
   *
   * @see topgg::start_bot_autoposter
   * @since 3.0.0
   */
  class bot_autoposter_source {
  public:
    virtual size_t TOPGG_EXPORT get_bot_server_count(dpp::cluster&) = 0;
  };
}; // namespace topgg

#undef TOPGG_BOT_QUERY_SEARCH
#undef TOPGG_BOT_QUERY_QUERY
#undef TOPGG_BOT_QUERY_SORT