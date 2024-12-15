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

#include <topgg/export.h>
#include <dpp/dpp.h>
#include <string>
#include <optional>
#include <vector>
#include <ctime>

namespace topgg {

// Forward declarations
class client;

/**
 * @brief Base class of the account data stored in the Top.gg API.
 */
class TOPGG_API account {
protected:
    account(const dpp::json& j);

public:
    account() = delete;
    virtual ~account() = default;

    dpp::snowflake id;
    std::string avatar;
    std::string username;
    time_t created_at;
};

/**
 * @brief Represents voters of a Discord bot.
 */
class TOPGG_API voter : public account {
    voter(const dpp::json& j);

public:
    voter() = delete;
    ~voter() override = default;

    friend class client;
};

/**
 * @brief Represents a Discord bot listed on Top.gg.
 */
class TOPGG_API bot : public account {
    bot(const dpp::json& j);

public:
    bot() = delete;
    ~bot() override = default;

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

/**
 * @brief Represents a Discord bot's statistics.
 */
class TOPGG_API stats {
private:
    std::optional<size_t> m_shard_count;
    std::optional<std::vector<size_t>> m_shards;
    std::optional<size_t> m_shard_id;
    std::optional<size_t> m_server_count;

    stats(const dpp::json& j);
    std::string to_json() const;

public:
    stats() = delete;
    ~stats() = default;

    stats(dpp::cluster& bot);
    stats(const size_t server_count, const size_t shard_count = 1);
    stats(const std::vector<size_t>& shards, const size_t shard_index = 0);

    std::vector<size_t> shards() const noexcept;
    size_t shard_count() const noexcept;
    std::optional<size_t> server_count() const noexcept;
    void set_server_count(const size_t new_server_count) noexcept;

    friend class client;
};

/**
 * @brief Represents a user's social links.
 */
class TOPGG_API user_socials {
    user_socials(const dpp::json& j);

public:
    user_socials() = delete;
    ~user_socials() = default;

    std::optional<std::string> github;
    std::optional<std::string> instagram;
    std::optional<std::string> reddit;
    std::optional<std::string> twitter;
    std::optional<std::string> youtube;

    friend class user;
};

/**
 * @brief Represents a user logged into Top.gg.
 */
class TOPGG_API user : public account {
    user(const dpp::json& j);

public:
    user() = delete;
    ~user() override = default;

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

} // namespace topgg