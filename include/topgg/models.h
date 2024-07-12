/**
 * @module topgg
 * @file models.h
 * @brief The official C++ wrapper for the Top.gg API.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024 Top.gg & null8626
 * @date 2024-07-12
 * @version 2.0.0
 */

#pragma once

#include <topgg/topgg.h>

#include <string>
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

namespace topgg {
  /**
   * @brief Base class of the account data stored in the Top.gg API.
   *
   * @see topgg::bot
   * @see topgg::user
   * @see topgg::voter
   * @since 2.0.0
   */
  class TOPGG_EXPORT account {
  protected:
    account(const dpp::json& j);

  public:
    account() = delete;

    /**
     * @brief The account's Discord ID.
     *
     * @since 2.0.0
     */
    dpp::snowflake id;

    /**
     * @brief The account's entire Discord avatar URL.
     *
     * @note This avatar URL can be animated if possible.
     * @since 2.0.0
     */
    std::string avatar;

    /**
     * @brief The account's username.
     *
     * @since 2.0.0
     */
    std::string username;

    /**
     * @brief The unix timestamp of when this account was created.
     *
     * @since 2.0.0
     */
    time_t created_at;
  };

  class client;

  /**
   * @brief Represents voters of a Discord bot.
   *
   * @see topgg::client::get_voters
   * @see topgg::client::start_autoposter
   * @see topgg::account
   * @since 2.0.0
   */
  class TOPGG_EXPORT voter: public account {
    inline voter(const dpp::json& j)
      : account(j) {}

  public:
    voter() = delete;

    friend class client;
  };

  /**
   * @brief Represents a Discord bot listed on Top.gg.
   *
   * @see topgg::client::get_bot
   * @see topgg::account
   * @since 2.0.0
   */
  class TOPGG_EXPORT bot: public account {
    bot(const dpp::json& j);

  public:
    bot() = delete;

    /**
     * @brief The Discord bot's discriminator.
     *
     * @since 2.0.0
     */
    std::string discriminator;

    /**
     * @brief The Discord bot's command prefix.
     *
     * @since 2.0.0
     */
    std::string prefix;

    /**
     * @brief The Discord bot's short description.
     *
     * @since 2.0.0
     */
    std::string short_description;

    /**
     * @brief The Discord bot's long description, if available.
     *
     * @note This long description can contain Markdown and/or HTML.
     * @since 2.0.0
     */
    std::optional<std::string> long_description;

    /**
     * @brief A list of the Discord bot's tags.
     *
     * @since 2.0.0
     */
    std::vector<std::string> tags;

    /**
     * @brief A link to the Discord bot's website, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> website;

    /**
     * @brief A link to the Discord bot's GitHub repository, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> github;

    /**
     * @brief A list of the Discord bot's owners, represented in Discord user IDs.
     *
     * @since 2.0.0
     */
    std::vector<dpp::snowflake> owners;

    /**
     * @brief A list of IDs of the guilds featured on this Discord bot’s page.
     *
     * @since 2.0.0
     */
    std::vector<size_t> guilds;

    /**
     * @brief The Discord bot's page banner URL, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> banner;

    /**
     * @brief The unix timestamp of when this Discord bot was approved on Top.gg by a Bot Reviewer.
     *
     * @since 2.0.0
     */
    time_t approved_at;

    /**
     * @brief Whether this Discord bot is Top.gg certified or not.
     *
     * @since 2.0.0
     */
    bool is_certified;

    /**
     * @brief A list of this Discord bot’s shards.
     *
     * @since 2.0.0
     */
    std::vector<size_t> shards;

    /**
     * @brief The amount of upvotes this Discord bot has.
     *
     * @since 2.0.0
     */
    size_t votes;

    /**
     * @brief The amount of upvotes this Discord bot has this month.
     *
     * @since 2.0.0
     */
    size_t monthly_votes;

    /**
     * @brief The Discord bot's support server invite URL, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> support;

    /**
     * @brief The amount of shards this Discord bot has according to posted stats.
     *
     * @since 2.0.0
     */
    size_t shard_count;

    /**
     * @brief The invite URL of this Discord bot.
     *
     * @since 2.0.0
     */
    std::string invite;

    /**
     * @brief The URL of this Discord bot’s Top.gg page.
     *
     * @since 2.0.0
     */
    std::string url;

    friend class client;
  };

  /**
   * @brief Represents a Discord bot’s statistics.
   *
   * @see topgg::voter
   * @see topgg::client::get_stats
   * @see topgg::client::post_stats
   * @see topgg::client::start_autoposter
   * @since 2.0.0
   */
  class TOPGG_EXPORT stats {
    stats(const dpp::json& j);

    std::optional<size_t> m_shard_count;
    std::optional<std::vector<size_t>> m_shards;
    std::optional<size_t> m_shard_id;
    std::optional<size_t> m_server_count;

    std::string to_json() const;

  public:
    stats() = delete;

    /**
     * @brief Creates a stats object based on existing data from your D++ cluster instance.
     *
     * @param bot The D++ cluster instance.
     * @since 2.0.0
     */
    stats(dpp::cluster& bot);

    /**
     * @brief Creates a stats object based on the bot's server and shard count.
     *
     * @param server_count The amount of servers this Discord bot has.
     * @param shard_count The amount of shards this Discord bot has. Defaults to one.
     * @since 2.0.0
     */
    inline stats(const size_t server_count, const size_t shard_count = 1)
      : m_shard_count(std::optional{shard_count}), m_server_count(std::optional{server_count}) {}

    /**
     * @brief Creates a stats object based on the bot's shard data.
     *
     * @param shards An array of this bot's server count for each shard.
     * @param shard_index The array index of the shard posting this data, defaults to zero.
     * @throw std::out_of_range If the shard_index argument is out of bounds from the shards argument.
     * @since 2.0.0
     */
    stats(const std::vector<size_t>& shards, const size_t shard_index = 0);

    /**
     * @brief Returns this stats object's server count for each shard.
     * @return std::vector<size_t> This stats object's server count for each shard.
     * @since 2.0.0
     */
    std::vector<size_t> shards() const noexcept;

    /**
     * @brief Returns this stats object's shard count.
     * @return size_t This stats object's shard count.
     * @since 2.0.0
     */
    size_t shard_count() const noexcept;

    /**
     * @brief Returns this stats object's server count, if available.
     * @return std::optional<size_t> This stats object's server count, if available.
     * @since 2.0.0
     */
    std::optional<size_t> server_count() const noexcept;

    /**
     * @brief Sets this stats object's server count.
     *
     * @param new_server_count The new server count.
     * @since 2.0.0
     */
    inline void set_server_count(const size_t new_server_count) noexcept {
      m_server_count = std::optional{new_server_count};
    }

    friend class client;
  };

  class user;

  /**
   * @brief Represents a user's social links, if available.
   *
   * @see topgg::user
   * @since 2.0.0
   */
  class TOPGG_EXPORT user_socials {
    user_socials(const dpp::json& j);

  public:
    user_socials() = delete;

    /**
     * @brief A URL of this user’s GitHub account, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> github;

    /**
     * @brief A URL of this user’s Instagram account, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> instagram;

    /**
     * @brief A URL of this user’s Reddit account, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> reddit;

    /**
     * @brief A URL of this user’s Twitter/X account, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> twitter;

    /**
     * @brief A URL of this user’s YouTube channel, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> youtube;

    friend class user;
  };

  /**
   * @brief Represents a user logged into Top.gg.
   *
   * @see topgg::user_socials
   * @see topgg::client::get_user
   * @see topgg::voter
   * @see topgg::account
   * @since 2.0.0
   */
  class TOPGG_EXPORT user: public account {
    user(const dpp::json& j);

  public:
    user() = delete;

    /**
     * @brief The user's bio, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> bio;

    /**
     * @brief The URL of this user’s profile banner image, if available.
     *
     * @since 2.0.0
     */
    std::optional<std::string> banner;

    /**
     * @brief This user's social links, if available.
     *
     * @since 2.0.0
     */
    std::optional<user_socials> socials;

    /**
     * @brief Whether this user is a Top.gg supporter or not.
     *
     * @since 2.0.0
     */
    bool is_supporter;

    /**
     * @brief Whether this user is a Top.gg certified developer or not.
     *
     * @since 2.0.0
     */
    bool is_certified_dev;

    /**
     * @brief Whether this user is a Top.gg moderator or not.
     *
     * @since 2.0.0
     */
    bool is_moderator;

    /**
     * @brief Whether this user is a Top.gg website moderator or not.
     *
     * @since 2.0.0
     */
    bool is_web_moderator;

    /**
     * @brief Whether this user is a Top.gg website administrator or not.
     *
     * @since 2.0.0
     */
    bool is_admin;

    friend class client;
  };
}; // namespace topgg