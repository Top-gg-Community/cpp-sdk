#include <topgg/dpp.h>

using topgg::account;
using topgg::bot;
using topgg::socials;
using topgg::stats;
using topgg::user;

#ifdef _WIN32
#include <iomanip>
#include <sstream>

static void strptime(const char* s, const char* f, struct tm* tm) {
  std::istringstream input(s);
  input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
  input >> std::get_time(tm, f);
}
#endif

#define DESERIALIZE(j, name, type) m_##name = j[#name].template get<type>()

#define DESERIALIZE_ALIAS(j, name, prop, type) \
  m_##prop = j[#name].template get<type>()

#define IGNORE_EXCEPTION(scope) \
  try scope catch (const std::exception& _) {}

#define DESERIALIZE_VECTOR(j, name, type) \
  IGNORE_EXCEPTION({ m_##name = j[#name].template get<std::vector<type>>(); })

#define DESERIALIZE_VECTOR_ALIAS(j, name, prop, type) \
  IGNORE_EXCEPTION({ m_##prop = j[#name].template get<std::vector<type>>(); })

#define DESERIALIZE_OPTIONAL_STRING(j, name)                 \
  IGNORE_EXCEPTION({                                         \
    const auto value = j[#name].template get<std::string>(); \
                                                             \
    if (value.size() > 0) {                                  \
      m_##name = std::optional{value};                       \
    }                                                        \
  })

#define DESERIALIZE_OPTIONAL_STRING_ALIAS(j, name, prop)     \
  IGNORE_EXCEPTION({                                         \
    const auto value = j[#name].template get<std::string>(); \
                                                             \
    if (value.size() > 0) {                                  \
      m_##prop = std::optional{value};                       \
    }                                                        \
  })

account::account(const nlohmann::json& j) {
  m_id = dpp::snowflake{j["id"].template get<std::string>()};

  DESERIALIZE(j, username, std::string);

  try {
    const auto hash = j["avatar"].template get<std::string>();
    const char* ext = hash.rfind("a_", 0) == 0 ? "gif" : "png";

    m_avatar = "https://cdn.discordapp.com/avatars/" + std::to_string(m_id) +
               "/" + hash + "." + ext + "?size=1024";
  } catch (const std::exception& _) {
    m_avatar = "https://cdn.discordapp.com/embed/avatars/" +
               std::to_string((m_id >> 22) % 5) + ".png";
  }
}

bot::bot(const nlohmann::json& j) : account(j), m_url("https://top.gg/bot/") {
  DESERIALIZE(j, discriminator, std::string);
  DESERIALIZE(j, prefix, std::string);
  DESERIALIZE_ALIAS(j, shortdesc, short_description, std::string);
  DESERIALIZE_OPTIONAL_STRING_ALIAS(j, longdesc, long_description);
  DESERIALIZE_VECTOR(j, tags, std::string);
  DESERIALIZE_OPTIONAL_STRING(j, website);
  DESERIALIZE_OPTIONAL_STRING(j, github);

  IGNORE_EXCEPTION({
    const auto owners = j["owners"].template get<std::vector<std::string>>();

    m_owners.reserve(owners.size());

    for (const auto& owner : owners) {
      m_owners.push_back(dpp::snowflake{owner});
    }
  });

  DESERIALIZE_VECTOR(j, guilds, size_t);
  DESERIALIZE_OPTIONAL_STRING_ALIAS(j, bannerUrl, banner);

  const auto approved_at = j["date"].template get<std::string>();
  struct tm approved_at_tm;

  strptime(approved_at.c_str(), "%Y-%m-%dT%H:%M:%S", &approved_at_tm);
  m_approved_at = mktime(&approved_at_tm);

  DESERIALIZE_ALIAS(j, certifiedBot, is_certified, bool);
  DESERIALIZE_VECTOR(j, shards, size_t);
  DESERIALIZE_ALIAS(j, points, votes, size_t);
  DESERIALIZE_ALIAS(j, monthlyPoints, monthly_votes, size_t);

  try {
    DESERIALIZE(j, invite, std::string);
  } catch (const std::exception& _) {
    m_invite = "https://discord.com/oauth2/authorize?scope=bot&client_id=" +
               std::to_string(m_id);
  }

  IGNORE_EXCEPTION({
    const auto support = j["support"].template get<std::string>();

    if (support.size() > 0) {
      m_support = std::optional{"https://discord.com/invite/" + support};
    }
  });

  try {
    DESERIALIZE(j, shard_count, size_t);
  } catch (const std::exception& _) {
    m_shard_count = m_shards.size();
  }

  try {
    m_url.append(j["vanity"].template get<std::string>());
  } catch (const std::exception& _) {
    m_url.append(std::to_string(m_id));
  }
}

stats::stats(const nlohmann::json& j) {
  DESERIALIZE_VECTOR(j, shards, size_t);

  try {
    DESERIALIZE(j, shard_count, size_t);
  } catch (const std::exception& _) {
    m_shard_count = m_shards.size();
  }

  try {
    const auto server_count = j["server_count"].template get<size_t>();

    m_server_count = std::optional{server_count};
  } catch (const std::exception& _) {
    if (m_shards.size() > 0) {
      size_t server_count = 0;

      for (const auto& shards : m_shards) {
        server_count += shards;
      }

      m_server_count = std::optional{server_count};
    }
  }
}

socials::socials(const nlohmann::json& j) {
  DESERIALIZE_OPTIONAL_STRING(j, github);
  DESERIALIZE_OPTIONAL_STRING(j, instagram);
  DESERIALIZE_OPTIONAL_STRING(j, reddit);
  DESERIALIZE_OPTIONAL_STRING(j, twitter);
  DESERIALIZE_OPTIONAL_STRING(j, youtube);
}

user::user(const nlohmann::json& j) : account(j) {
  DESERIALIZE_OPTIONAL_STRING(j, bio);
  DESERIALIZE_OPTIONAL_STRING(j, banner);

  if (j.contains("contains")) {
    m_socials = std::optional{
        topgg::socials::socials{j["socials"].template get<nlohmann::json>()}};
  }

  DESERIALIZE_ALIAS(j, supporter, is_supporter, bool);
  DESERIALIZE_ALIAS(j, certifiedDev, is_certified_dev, bool);
  DESERIALIZE_ALIAS(j, mod, is_moderator, bool);
  DESERIALIZE_ALIAS(j, webMod, is_web_moderator, bool);
  DESERIALIZE_ALIAS(j, admin, is_admin, bool);
}