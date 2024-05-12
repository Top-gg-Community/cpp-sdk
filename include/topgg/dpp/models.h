#ifndef __TOPGG_CPP_SDK_DPP_MODELS_H__
#define __TOPGG_CPP_SDK_DPP_MODELS_H__

#include <nlohmann/json.hpp>
#include <dpp/dpp.h>

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
  
  class account {
  protected:
    account(const nlohmann::json& j);
    
    TOPGG_MODEL_PROPERTY_REF_VIS(protected, dpp::snowflake, id)
    TOPGG_MODEL_PROPERTY_REF(std::string, avatar)
    TOPGG_MODEL_PROPERTY_REF(std::string_view, username)
    
  public:
    inline time_t created_at() const noexcept {
      return static_cast<time_t>(((m_id >> 22) / 1000) + 1420070400);
    }
  };
  
  class dpp_client;
  
  class voter: public account {
    friend class dpp_client;
    
    inline voter(const nlohmann::json& j): account(j) {}
  };

  class bot: public account {
    friend class dpp_client;
    
    bot(const nlohmann::json& j);
    
    TOPGG_MODEL_PROPERTY_REF(std::string_view, discriminator)
    TOPGG_MODEL_PROPERTY_REF(std::string_view, prefix)
    TOPGG_MODEL_PROPERTY_REF(std::string_view, short_description)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, long_description)
    TOPGG_MODEL_PROPERTY_REF(std::vector<std::string_view>, tags)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, website)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, github)
    TOPGG_MODEL_PROPERTY_REF(std::vector<dpp::snowflake>, owners)
    TOPGG_MODEL_PROPERTY_REF(std::vector<size_t>, guilds)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, banner)
    TOPGG_MODEL_PROPERTY(time_t, approved_at)
    TOPGG_MODEL_PROPERTY(bool, is_certified)
    TOPGG_MODEL_PROPERTY_REF(std::vector<size_t>, shards)
    TOPGG_MODEL_PROPERTY(size_t, votes)
    TOPGG_MODEL_PROPERTY(size_t, monthly_votes)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, support)
    TOPGG_MODEL_PROPERTY(size_t, shard_count)
    TOPGG_MODEL_PROPERTY_REF(std::string, invite)
    TOPGG_MODEL_PROPERTY_REF(std::string, url)
  };
  
  class stats {
    friend class dpp_client;
  
    stats(const nlohmann::json& j);
  
    std::optional<size_t> m_shard_count;
    std::optional<std::vector<size_t>> m_shards;
    std::optional<size_t> m_shard_id;
    std::optional<size_t> m_server_count;
  
    std::string to_json() const;
  
  public:
    inline stats(const size_t server_count): m_server_count(std::optional{server_count}) {}
    inline stats(const size_t server_count, const size_t shard_count): m_server_count(std::optional{shard_count}), m_shard_count(std::optional{shard_count}) {}
    
    stats(const std::vector<size_t>& shards, const size_t shard_index);
    
    std::vector<size_t> shards() const noexcept;
    size_t shard_count() const noexcept;
    std::optional<size_t> server_count() const noexcept;
  };
  
  class user;

  class socials {
    friend class user;
    
    socials(const nlohmann::json& j);
    
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, github)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, instagram)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, reddit)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, twitter)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, youtube)
  };
  
  class user: public account {
    friend class dpp_client;
    
    user(const nlohmann::json& j);
    
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, bio)
    TOPGG_MODEL_PROPERTY_REF(std::optional<std::string_view>, banner)
    TOPGG_MODEL_PROPERTY_REF(std::optional<socials>, socials)
    TOPGG_MODEL_PROPERTY(bool, is_supporter)
    TOPGG_MODEL_PROPERTY(bool, is_certified_dev)
    TOPGG_MODEL_PROPERTY(bool, is_moderator)
    TOPGG_MODEL_PROPERTY(bool, is_web_moderator)
    TOPGG_MODEL_PROPERTY(bool, is_admin)
  };
};

#undef TOPGG_MODEL_PROPERTY_VIS
#undef TOPGG_MODEL_PROPERTY
#undef TOPGG_MODEL_PROPERTY_REF_VIS
#undef TOPGG_MODEL_PROPERTY_REF
#endif