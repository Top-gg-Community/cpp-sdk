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
   */
  class TOPGG_EXPORT account {
  protected:
    /**
     * @brief Creates this class from a JSON data.
     *
     * @param j The JSON data to use from.
     */
    account(const dpp::json& j);

  public:
    /**
     * @brief No outsiders are allowed to initiate this class, internal use only :)
     */
    account() = delete;

    /**
     * @brief The account's Discord ID.
     */
    dpp::snowflake id;

    /**
     * @brief The account's entire Discord avatar URL.
     * @note This avatar URL can be animated if possible.
     */
    std::string avatar;

    /**
     * @brief The account's username.
     */
    std::string username;

    /**
     * @brief The unix timestamp of when this account was created.
     */
    time_t created_at;
  };

  class client;

  /**
   * @brief Represents voters of a Discord bot.
   */
  class TOPGG_EXPORT voter: public account {
    inline voter(const dpp::json& j)
      : account(j) {}

  public:
    /**
     * @brief No outsiders are allowed to initiate this class, internal use only :)
     */
    voter() = delete;

    friend class client;
  };

  /**
   * @brief Represents a Discord bot listed on Top.gg.
   */
  class TOPGG_EXPORT bot: public account {
    bot(const dpp::json& j);

  public:
    /**
     * @brief No outsiders are allowed to initiate this class, internal use only :)
     */
    bot() = delete;

    /**
     * @brief The Discord bot's discriminator.
     */
    std::string discriminator;

    /**
     * @brief The Discord bot's command prefix.
     */
    std::string prefix;

    /**
     * @brief The Discord bot's short description.
     */
    std::string short_description;

    /**
     * @brief The Discord bot's long description, if available.
     * @note This long description can contain Markdown and/or HTML.
     */
    std::optional<std::string> long_description;

    /**
     * @brief A list of the Discord bot's tags.
     */
    std::vector<std::string> tags;

    /**
     * @brief A link to the Discord bot's website, if available.
     */
    std::optional<std::string> website;

    /**
     * @brief A link to the Discord bot's GitHub repository, if available.
     */
    std::optional<std::string> github;

    /**
     * @brief A list of the Discord bot's owners, represented in Discord user IDs.
     */
    std::vector<dpp::snowflake> owners;

    /**
     * @brief A list of IDs of the guilds featured on this Discord bot’s page.
     */
    std::vector<size_t> guilds;

    /**
     * @brief The Discord bot's page banner URL, if available.
     */
    std::optional<std::string> banner;

    /**
     * @brief The unix timestamp of when this Discord bot was approved on Top.gg by a Bot Reviewer.
     */
    time_t approved_at;

    /**
     * @brief Whether this Discord bot is Top.gg certified or not.
     */
    bool is_certified;

    /**
     * @brief A list of this Discord bot’s shards.
     */
    std::vector<size_t> shards;

    /**
     * @brief The amount of upvotes this Discord bot has.
     */
    size_t votes;

    /**
     * @brief The amount of upvotes this Discord bot has this month.
     */
    size_t monthly_votes;

    /**
     * @brief The Discord bot's support server invite URL, if available.
     */
    std::optional<std::string> support;

    /**
     * @brief The amount of shards this Discord bot has according to posted stats.
     */
    size_t shard_count;

    /**
     * @brief The invite URL of this Discord bot.
     */
    std::string invite;

    /**
     * @brief The URL of this Discord bot’s Top.gg page.
     */
    std::string url;

    friend class client;
  };

  namespace autoposter {
    class base;
  }; // namespace autoposter

  /**
   * @brief Represents a Discord bot’s statistics.
   */
  class TOPGG_EXPORT stats {
    stats(const dpp::json& j);

    std::optional<size_t> m_shard_count;
    std::optional<std::vector<size_t>> m_shards;
    std::optional<size_t> m_shard_id;
    std::optional<size_t> m_server_count;

    std::string to_json() const;

  public:
    /**
     * @brief No outsiders are allowed to initiate this class, internal use only :)
     */
    stats() = delete;

    /**
     * @brief Creates a stats object solely based on the bot's server count.
     *
     * @param server_count The amount of servers this Discord bot has.
     */
    inline stats(const size_t server_count)
      : m_server_count(std::optional{server_count}) {}

    /**
     * @brief Creates a stats object based on the bot's server and shard count.
     *
     * @param server_count The amount of servers this Discord bot has.
     * @param shard_count The amount of shards this Discord bot has.
     */
    inline stats(const size_t server_count, const size_t shard_count)
      : m_shard_count(std::optional{shard_count}), m_server_count(std::optional{server_count}) {}

    /**
     * @brief Creates a stats object based on the bot's shard data.
     *
     * @param shards An array of this bot's server count for each shard.
     * @param shard_index The array index of the shard posting this data.
     * @throw std::out_of_range If the shard_index argument is out of bounds from the shards argument.
     */
    stats(const std::vector<size_t>& shards, const size_t shard_index);

    /**
     * @brief Returns this stats object's server count for each shard.
     * @return std::vector<size_t> This stats object's server count for each shard.
     */
    std::vector<size_t> shards() const noexcept;

    /**
     * @brief Returns this stats object's shard count.
     * @return size_t This stats object's shard count.
     */
    size_t shard_count() const noexcept;

    /**
     * @brief Returns this stats object's server count, if available.
     * @return std::optional<size_t> This stats object's server count, if available.
     */
    std::optional<size_t> server_count() const noexcept;

    /**
     * @brief Sets this stats object's server count.
     *
     * @param new_server_count The new server count.
     */
    inline void set_server_count(const size_t new_server_count) noexcept {
      m_server_count = std::optional{new_server_count};
    }

    friend class client;
    friend class autoposter::base;
  };

  class user;

  /**
   * @brief Represents a user's social links, if available.
   */
  class TOPGG_EXPORT user_socials {
    user_socials(const dpp::json& j);

  public:
    /**
     * @brief No outsiders are allowed to initiate this class, internal use only :)
     */
    user_socials() = delete;

    /**
     * @brief A URL of this user’s GitHub account, if available.
     */
    std::optional<std::string> github;

    /**
     * @brief A URL of this user’s Instagram account, if available.
     */
    std::optional<std::string> instagram;

    /**
     * @brief A URL of this user’s Reddit account, if available.
     */
    std::optional<std::string> reddit;

    /**
     * @brief A URL of this user’s Twitter/X account, if available.
     */
    std::optional<std::string> twitter;

    /**
     * @brief A URL of this user’s YouTube channel, if available.
     */
    std::optional<std::string> youtube;

    friend class user;
  };

  /**
   * @brief Represents a user logged into Top.gg.
   */
  class TOPGG_EXPORT user: public account {
    user(const dpp::json& j);

  public:
    /**
     * @brief No outsiders are allowed to initiate this class, internal use only :)
     */
    user() = delete;

    /**
     * @brief The user's bio, if available.
     */
    std::optional<std::string> bio;

    /**
     * @brief The URL of this user’s profile banner image, if available.
     */
    std::optional<std::string> banner;

    /**
     * @brief This user's social links, if available.
     */
    std::optional<user_socials> socials;

    /**
     * @brief Whether this user is a Top.gg supporter or not.
     */
    bool is_supporter;

    /**
     * @brief Whether this user is a Top.gg certified developer or not.
     */
    bool is_certified_dev;

    /**
     * @brief Whether this user is a Top.gg moderator or not.
     */
    bool is_moderator;

    /**
     * @brief Whether this user is a Top.gg website moderator or not.
     */
    bool is_web_moderator;

    /**
     * @brief Whether this user is a Top.gg website administrator or not.
     */
    bool is_admin;

    friend class client;
  };
}; // namespace topgg