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
  class TOPGG_EXPORT account {
  protected:
    account(const dpp::json& j);

  public:
    account() = delete;

    dpp::snowflake id;
    std::string avatar;
    std::string username;
    time_t created_at;
  };

  class client;

  class TOPGG_EXPORT voter: public account {
    inline voter(const dpp::json& j)
      : account(j) {}

  public:
    voter() = delete;

    friend class client;
  };

  class TOPGG_EXPORT bot: public account {
    bot(const dpp::json& j);

  public:
    bot() = delete;

    std::string discriminator;
    std::string prefix;
    std::string short_description;
    std::optional<std::string> long_description;
    std::vector<std::string> tags;
    std::optional<std::string> website;
    std::optional<std::string> github;
    std::vector<dpp::snowflake> owners;
    std::vector<size_t> guilds;
    std::optional<std::string> banner;
    time_t approved_at;
    bool is_certified;
    std::vector<size_t> shards;
    size_t votes;
    size_t monthly_votes;
    std::optional<std::string> support;
    size_t shard_count;
    std::string invite;
    std::string url;

    friend class client;
  };

  namespace autoposter {
    class base;
  }; // namespace autoposter

  class TOPGG_EXPORT stats {
    stats(const dpp::json& j);

    std::optional<size_t> m_shard_count;
    std::optional<std::vector<size_t>> m_shards;
    std::optional<size_t> m_shard_id;
    std::optional<size_t> m_server_count;

    std::string to_json() const;

  public:
    stats() = delete;

    inline stats(const size_t server_count)
      : m_server_count(std::optional{server_count}) {}

    inline stats(const size_t server_count, const size_t shard_count)
      : m_shard_count(std::optional{shard_count}), m_server_count(std::optional{server_count}) {}

    stats(const std::vector<size_t>& shards, const size_t shard_index);

    std::vector<size_t> shards() const noexcept;
    size_t shard_count() const noexcept;
    std::optional<size_t> server_count() const noexcept;

    inline void set_server_count(const size_t new_server_count) noexcept {
      m_server_count = std::optional{new_server_count};
    }

    friend class client;
    friend class autoposter::base;
  };

  class user;

  class TOPGG_EXPORT user_socials {
    user_socials(const dpp::json& j);

  public:
    user_socials() = delete;

    std::optional<std::string> github;
    std::optional<std::string> instagram;
    std::optional<std::string> reddit;
    std::optional<std::string> twitter;
    std::optional<std::string> youtube;

    friend class user;
  };

  class TOPGG_EXPORT user: public account {
    user(const dpp::json& j);

  public:
    user() = delete;

    std::optional<std::string> bio;
    std::optional<std::string> banner;
    std::optional<user_socials> socials;
    bool is_supporter;
    bool is_certified_dev;
    bool is_moderator;
    bool is_web_moderator;
    bool is_admin;

    friend class client;
  };
}; // namespace topgg