#include <topgg/dpp.h>

using topgg::account;
using topgg::socials;
using topgg::stats;
using topgg::user;
using topgg::bot;

#ifdef _WIN32
#include <sstream>
#include <iomanip>

static void strptime(const char* s, const char* f, tm* t) {
  std::istringstream input{s};
  input.imbue(std::locale{setlocale(LC_ALL, nullptr)});
  input >> std::get_time(t, f);
}

#ifdef _MSC_VER
#pragma warning(disable: 4101)
#endif
#endif

#define SERIALIZE_PRIVATE_OPTIONAL(j, name) \
  if (m_##name.has_value()) {               \
    j[#name] = m_##name.value();            \
  }

#define DESERIALIZE(j, name, type) \
  name = j[#name].template get<type>()

#define DESERIALIZE_ALIAS(j, name, prop, type) \
  prop = j[#name].template get<type>()

#define IGNORE_EXCEPTION(scope) \
  try scope catch (const std::exception& _) {}

#define DESERIALIZE_VECTOR(j, name, type)                  \
  IGNORE_EXCEPTION({                                       \
    name = j[#name].template get<std::vector<type>>();     \
  })

#define DESERIALIZE_VECTOR_ALIAS(j, name, prop, type)      \
  IGNORE_EXCEPTION({                                       \
    prop = j[#name].template get<std::vector<type>>();     \
  })

#define DESERIALIZE_OPTIONAL(j, name, type)   \
  IGNORE_EXCEPTION({                          \
    name = j[#name].template get<type>();     \
  })

#define DESERIALIZE_PRIVATE_OPTIONAL(j, name, type)   \
  IGNORE_EXCEPTION({                                  \
    m_##name = j[#name].template get<type>();         \
  })

#define DESERIALIZE_OPTIONAL_ALIAS(j, name, prop) \
  IGNORE_EXCEPTION({                              \
    prop = j[#name].template get<type>();         \
  })

#define DESERIALIZE_OPTIONAL_STRING(j, name)                      \
  IGNORE_EXCEPTION({                                              \
    const auto value = j[#name].template get<std::string_view>(); \
                                                                  \
    if (value.size() > 0) {                                       \
      name = std::optional{value};                                \
    }                                                             \
  })

#define DESERIALIZE_OPTIONAL_STRING_ALIAS(j, name, prop)          \
  IGNORE_EXCEPTION({                                              \
    const auto value = j[#name].template get<std::string_view>(); \
                                                                  \
    if (value.size() > 0) {                                       \
      prop = std::optional{value};                                \
    }                                                             \
  })

account::account(const dpp::json& j) {
  id = dpp::snowflake{j["id"].template get<std::string_view>()};
  
  DESERIALIZE(j, username, std::string_view);
  
  try {
    const auto hash = j["avatar"].template get<std::string>();
    const char* ext = hash.rfind("a_", 0) == 0 ? "gif" : "png";

    avatar = "https://cdn.discordapp.com/avatars/" + std::to_string(id) + "/" + hash + "." + ext + "?size=1024";
  } catch (const std::exception& _) {
    avatar = "https://cdn.discordapp.com/embed/avatars/" + std::to_string((id >> 22) % 5) + ".png";
  }
  
  created_at = static_cast<time_t>(((id >> 22) / 1000) + 1420070400);
}

bot::bot(const dpp::json& j): account(j), url("https://top.gg/bot/") {
  DESERIALIZE(j, discriminator, std::string_view);
  DESERIALIZE(j, prefix, std::string_view);
  DESERIALIZE_ALIAS(j, shortdesc, short_description, std::string_view);
  DESERIALIZE_OPTIONAL_STRING_ALIAS(j, longdesc, long_description);
  DESERIALIZE_VECTOR(j, tags, std::string_view);
  DESERIALIZE_OPTIONAL_STRING(j, website);
  DESERIALIZE_OPTIONAL_STRING(j, github);
  
  IGNORE_EXCEPTION({
    const auto j_owners = j["owners"].template get<std::vector<std::string_view>>();
    
    owners.reserve(j_owners.size());
    
    for (const auto& owner: j_owners) {
      owners.push_back(dpp::snowflake{owner});
    }
  });
  
  DESERIALIZE_VECTOR(j, guilds, size_t);
  DESERIALIZE_OPTIONAL_STRING_ALIAS(j, bannerUrl, banner);

  const auto j_approved_at = j["date"].template get<std::string_view>();
  tm approved_at_tm;
  
  strptime(j_approved_at.data(), "%Y-%m-%dT%H:%M:%S", &approved_at_tm);
  approved_at = mktime(&approved_at_tm);
  
  DESERIALIZE_ALIAS(j, certifiedBot, is_certified, bool);
  DESERIALIZE_VECTOR(j, shards, size_t);
  DESERIALIZE_ALIAS(j, points, votes, size_t);
  DESERIALIZE_ALIAS(j, monthlyPoints, monthly_votes, size_t);

  try {
    DESERIALIZE(j, invite, std::string);
  } catch (const std::exception& _) {
    invite = "https://discord.com/oauth2/authorize?scope=bot&client_id=" + std::to_string(id);
  }
  
  IGNORE_EXCEPTION({
    const auto j_support = j["support"].template get<std::string>();
    
    if (j_support.size() > 0) {
      support = std::optional{"https://discord.com/invite/" + j_support};
    }
  });
  
  try {
    DESERIALIZE(j, shard_count, size_t);
  } catch (const std::exception& _) {
    shard_count = shards.size();
  }
  
  try {
    url.append(j["vanity"].template get<std::string_view>());
  } catch (const std::exception& _) {
    url.append(std::to_string(id));
  }
}

stats::stats(const dpp::json& j) {
  DESERIALIZE_PRIVATE_OPTIONAL(j, shard_count, size_t);
  DESERIALIZE_PRIVATE_OPTIONAL(j, server_count, size_t);
  DESERIALIZE_PRIVATE_OPTIONAL(j, shards, std::vector<size_t>);
  DESERIALIZE_PRIVATE_OPTIONAL(j, shard_id, size_t);
}

stats::stats(const std::vector<size_t>& shards, const size_t shard_index): m_shards(std::optional{shards}), m_server_count(std::optional{std::reduce(shards.begin(), shards.end())}) {
  if (shard_index >= shards.size()) {
    throw std::out_of_range("Shard index out of bounds from the given shards array.");
  }
  
  m_shard_id = std::optional{shard_index};
  m_shard_count = std::optional{shards.size()};
}

std::string stats::to_json() const {
  dpp::json j;
  
  SERIALIZE_PRIVATE_OPTIONAL(j, shard_count);
  SERIALIZE_PRIVATE_OPTIONAL(j, server_count);
  SERIALIZE_PRIVATE_OPTIONAL(j, shards);
  SERIALIZE_PRIVATE_OPTIONAL(j, shard_id);
  
  return j.dump();
}

std::vector<size_t> stats::shards() const noexcept {
  return m_shards.value_or(std::vector<size_t>{});
}

size_t stats::shard_count() const noexcept {
  return m_shard_count.value_or(shards().size());
}

std::optional<size_t> stats::server_count() const noexcept {
  if (m_server_count.has_value()) {
    return m_server_count;
  } else {
    IGNORE_EXCEPTION({
      const auto& shards = m_shards.value();

      if (shards.size() > 0) {
        return std::optional{std::reduce(shards.begin(), shards.end())};
      }
    });

    return std::nullopt;
  }
}

socials::socials(const dpp::json& j) {
  DESERIALIZE_OPTIONAL_STRING(j, github);
  DESERIALIZE_OPTIONAL_STRING(j, instagram);
  DESERIALIZE_OPTIONAL_STRING(j, reddit);
  DESERIALIZE_OPTIONAL_STRING(j, twitter);
  DESERIALIZE_OPTIONAL_STRING(j, youtube);
}

user::user(const dpp::json& j): account(j) {
  DESERIALIZE_OPTIONAL_STRING(j, bio);
  DESERIALIZE_OPTIONAL_STRING(j, banner);
  
  if (j.contains("contains")) {
    socials = std::optional{topgg::socials::socials{j["socials"].template get<dpp::json>()}};
  }
  
  DESERIALIZE_ALIAS(j, supporter, is_supporter, bool);
  DESERIALIZE_ALIAS(j, certifiedDev, is_certified_dev, bool);
  DESERIALIZE_ALIAS(j, mod, is_moderator, bool);
  DESERIALIZE_ALIAS(j, webMod, is_web_moderator, bool);
  DESERIALIZE_ALIAS(j, admin, is_admin, bool);
}