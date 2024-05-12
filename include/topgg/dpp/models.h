#ifndef __TOPGG_CPP_SDK_DPP_MODELS_H__
#define __TOPGG_CPP_SDK_DPP_MODELS_H__

#include <dpp/dpp.h>
#include <topgg/dpp.h>

#include <nlohmann/json.hpp>
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
  visibility:                                            \
  type m_##name;                                         \
                                                         \
 public:                                                 \
  inline constexpr type name() const noexcept {          \
    return m_##name;                                     \
  }

#define TOPGG_MODEL_PROPERTY(type, name) \
  TOPGG_MODEL_PROPERTY_VIS(private, type, name)

#define TOPGG_MODEL_PROPERTY_REF_VIS(visibility, type, name) \
  visibility:                                                \
  type m_##name;                                             \
                                                             \
 public:                                                     \
  inline type const& name() const noexcept {                 \
    return m_##name;                                         \
  }

#define TOPGG_MODEL_PROPERTY_REF(type, name) \
  TOPGG_MODEL_PROPERTY_REF_VIS(private, type, name)

class account {
 protected:
  account(const nlohmann::json& j);

  TOPGG_MODEL_PROPERTY_REF_VIS(protected, dpp::snowflake, id)
  TOPGG_MODEL_PROPERTY_REF(std::string, avatar)
  TOPGG_MODEL_PROPERTY_REF(std::string, username)

 public:
  inline time_t created_at() const noexcept {
    return static_cast<time_t>(((m_id >> 22) / 1000) + 1420070400);
  }
};

class dpp_client;

class voter : public account {
  friend class dpp_client;

  inline voter(const nlohmann::json& j) : account(j) {}
};

class bot : public account {
  friend class dpp_client;

  bot(const nlohmann::json& j);

  TOPGG_MODEL_PROPERTY_REF(std::string, discriminator)
  TOPGG_MODEL_PROPERTY_REF(std::string, prefix)
  TOPGG_MODEL_PROPERTY_REF(std::string, short_description)
  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, long_description)
  TOPGG_MODEL_PROPERTY_REF(std::vector<std::string>, tags)
  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, website)
  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, github)
  TOPGG_MODEL_PROPERTY_REF(std::vector<dpp::snowflake>, owners)
  TOPGG_MODEL_PROPERTY_REF(std::vector<size_t>, guilds)
  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, banner)
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

  TOPGG_MODEL_PROPERTY_REF(std::vector<size_t>, shards)
  TOPGG_MODEL_PROPERTY(size_t, shard_count)
  TOPGG_MODEL_PROPERTY_REF(std::optional<size_t>, server_count)
};

class user;

class socials {
  friend class user;

  socials(const nlohmann::json& j);

  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, github)
  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, instagram)
  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, reddit)
  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, twitter)
  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, youtube)
};

class user : public account {
  friend class dpp_client;

  user(const nlohmann::json& j);

  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, bio)
  TOPGG_MODEL_PROPERTY_REF(std::optional<std::string>, banner)
  TOPGG_MODEL_PROPERTY_REF(std::optional<socials>, socials)
  TOPGG_MODEL_PROPERTY(bool, is_supporter)
  TOPGG_MODEL_PROPERTY(bool, is_certified_dev)
  TOPGG_MODEL_PROPERTY(bool, is_moderator)
  TOPGG_MODEL_PROPERTY(bool, is_web_moderator)
  TOPGG_MODEL_PROPERTY(bool, is_admin)
};
};  // namespace topgg

#undef TOPGG_MODEL_PROPERTY_VIS
#undef TOPGG_MODEL_PROPERTY
#undef TOPGG_MODEL_PROPERTY_REF_VIS
#undef TOPGG_MODEL_PROPERTY_REF
#endif