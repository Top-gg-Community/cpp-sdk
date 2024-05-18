#pragma once

#include <topgg/dpp.h>

#include <string_view>
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
#define TOPGG_MODEL_PROPERTY_VIS(visibility, type, name) \
  visibility: type m_##name;                             \
  public: inline constexpr type name() const noexcept {  \
    return m_##name;                                     \
  }

#define TOPGG_MODEL_PROPERTY(type, name) \
  TOPGG_MODEL_PROPERTY_VIS(private, type, name)

#define TOPGG_MODEL_PROPERTY_REF_VIS(visibility, type, name) \
  visibility: type m_##name;                                 \
  public: inline type const& name() const noexcept {         \
    return m_##name;                                         \
  }

#define TOPGG_MODEL_PROPERTY_REF(type, name) \
  TOPGG_MODEL_PROPERTY_REF_VIS(private, type, name)
  
  class TOPGG_EXPORT account {
  protected:
    account(const dpp::json& j);

  public:
    dpp::snowflake id;
    std::string avatar;
    std::string_view username;
    time_t created_at;
  };
  
  class client;
  
  class TOPGG_EXPORT voter: public account {
    friend class client;
    
    inline voter(const dpp::json& j): account(j) {}
  };

  class TOPGG_EXPORT bot: public account {
    friend class client;
    
    bot(const dpp::json& j);
    
  public:
    std::string_view discriminator;
    std::string_view prefix;
    std::string_view short_description;
    std::optional<std::string_view> long_description;
    std::vector<std::string_view> tags;
    std::optional<std::string_view> website;
    std::optional<std::string_view> github;
    std::vector<dpp::snowflake> owners;
    std::vector<size_t> guilds;
    std::optional<std::string_view> banner;
    time_t approved_at;
    bool is_certified;
    std::vector<size_t> shards;
    size_t votes;
    size_t monthly_votes;
    std::optional<std::string> support;
    size_t shard_count;
    std::string invite;
    std::string url;
  };
  
  namespace autoposter {
    class base;
  };
  
  class TOPGG_EXPORT stats {
    friend class client;
    friend class autoposter::base;
  
    stats(const dpp::json& j);
  
    std::optional<size_t> m_shard_count;
    std::optional<std::vector<size_t>> m_shards;
    std::optional<size_t> m_shard_id;
    std::optional<size_t> m_server_count;
  
    std::string to_json() const;
  
  public:
    inline stats(const size_t server_count): m_server_count(std::optional{server_count}) {}
    inline stats(const size_t server_count, const size_t shard_count): m_shard_count(std::optional{shard_count}), m_server_count(std::optional{server_count}) {}
    
    stats(const std::vector<size_t>& shards, const size_t shard_index);
    
    std::vector<size_t> shards() const noexcept;
    size_t shard_count() const noexcept;
    std::optional<size_t> server_count() const noexcept;
    
    inline void set_server_count(const size_t new_server_count) noexcept {
      m_server_count = std::optional{new_server_count};
    }
  };
  
  class user;

  class TOPGG_EXPORT user_socials {
    friend class user;
    
    user_socials(const dpp::json& j);
    
  public:
    std::optional<std::string_view> github;
    std::optional<std::string_view> instagram;
    std::optional<std::string_view> reddit;
    std::optional<std::string_view> twitter;
    std::optional<std::string_view> youtube;
  };
  
  class TOPGG_EXPORT user: public account {
    friend class client;
    
    user(const dpp::json& j);
    
  public:
    std::optional<std::string_view> bio;
    std::optional<std::string_view> banner;
    std::optional<user_socials> socials;
    bool is_supporter;
    bool is_certified_dev;
    bool is_moderator;
    bool is_web_moderator;
    bool is_admin;
  };
};

#undef TOPGG_MODEL_PROPERTY_VIS
#undef TOPGG_MODEL_PROPERTY
#undef TOPGG_MODEL_PROPERTY_REF_VIS
#undef TOPGG_MODEL_PROPERTY_REF