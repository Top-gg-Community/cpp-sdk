#include <topgg/topgg.h>

#include <chrono>

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

#define _TOPGG_DESERIALIZE(j, name, type) \
  name = j[#name].template get<type>()

#define _TOPGG_DESERIALIZE_ALIAS(j, name, prop, type) \
  prop = j[#name].template get<type>()

#define _TOPGG_IGNORE_EXCEPTION(scope) \
  try scope catch (TOPGG_UNUSED const std::exception&) {}

#define _TOPGG_DESERIALIZE_VECTOR(j, name, type)                  \
  _TOPGG_IGNORE_EXCEPTION({                                       \
    name = j[#name].template get<std::vector<type>>();            \
  })

#define _TOPGG_DESERIALIZE_VECTOR_ALIAS(j, name, prop, type)      \
  _TOPGG_IGNORE_EXCEPTION({                                       \
    prop = j[#name].template get<std::vector<type>>();            \
  })

#define _TOPGG_DESERIALIZE_OPTIONAL(j, name, type)   \
  _TOPGG_IGNORE_EXCEPTION({                          \
    name = j[#name].template get<type>();            \
  })

#define _TOPGG_DESERIALIZE_PRIVATE_OPTIONAL(j, name, type)   \
  _TOPGG_IGNORE_EXCEPTION({                                  \
    m_##name = j[#name].template get<type>();                \
  })

#define _TOPGG_DESERIALIZE_OPTIONAL_ALIAS(j, name, prop) \
  _TOPGG_IGNORE_EXCEPTION({                              \
    prop = j[#name].template get<type>();                \
  })

#define _TOPGG_DESERIALIZE_OPTIONAL_STRING(j, name)                      \
  _TOPGG_IGNORE_EXCEPTION({                                              \
    const auto value{j[#name].template get<std::string>()};              \
                                                                         \
    if (value.size() > 0) {                                              \
      name = std::optional{value};                                       \
    }                                                                    \
  })

#define _TOPGG_DESERIALIZE_OPTIONAL_STRING_ALIAS(j, name, prop)          \
  _TOPGG_IGNORE_EXCEPTION({                                              \
    const auto value{j[#name].template get<std::string>()};       \
                                                                  \
    if (value.size() > 0) {                                       \
      prop = std::optional{value};                                \
    }                                                             \
  })

using topgg::bot;
using topgg::bot_query;
using topgg::voter;

static time_t timestamp_from_id(const dpp::snowflake& id) {
  return static_cast<time_t>(((id >> 22) / 1000) + 1420070400);
}

bot::bot(const dpp::json& j) {
  id = _TOPGG_SNOWFLAKE_FROM_JSON(j, clientid);
  topgg_id = _TOPGG_SNOWFLAKE_FROM_JSON(j, id);

  _TOPGG_DESERIALIZE(j, username, std::string);
  _TOPGG_DESERIALIZE(j, avatar, std::string);

  created_at = timestamp_from_id(id);

  _TOPGG_DESERIALIZE(j, prefix, std::string);
  _TOPGG_DESERIALIZE_ALIAS(j, shortdesc, short_description, std::string);
  _TOPGG_DESERIALIZE_OPTIONAL_STRING_ALIAS(j, longdesc, long_description);
  _TOPGG_DESERIALIZE_VECTOR(j, tags, std::string);
  _TOPGG_DESERIALIZE_OPTIONAL_STRING(j, website);
  _TOPGG_DESERIALIZE_OPTIONAL_STRING(j, github);

  _TOPGG_IGNORE_EXCEPTION({
    const auto j_owners{j["owners"].template get<std::vector<std::string>>()};

    owners.reserve(j_owners.size());

    for (const auto& owner: j_owners) {
      owners.push_back(dpp::snowflake{owner});
    }
  });

  const auto j_submitted_at{j["date"].template get<std::string>()};
  tm submitted_at_tm{};

  strptime(j_submitted_at.data(), "%Y-%m-%dT%H:%M:%S", &submitted_at_tm);
  submitted_at = mktime(&submitted_at_tm);

  _TOPGG_DESERIALIZE_ALIAS(j, points, votes, size_t);
  _TOPGG_DESERIALIZE_ALIAS(j, monthlyPoints, monthly_votes, size_t);
  _TOPGG_DESERIALIZE_OPTIONAL(j, invite, std::string);
  _TOPGG_DESERIALIZE_OPTIONAL(j, vanity, std::string);
  _TOPGG_DESERIALIZE_OPTIONAL(j, support, std::string);
  _TOPGG_DESERIALIZE_OPTIONAL(j, server_count, size_t);
  
  const auto reviews{j["reviews"]};

  _TOPGG_DESERIALIZE_ALIAS(reviews, averageScore, review_score, double);
  _TOPGG_DESERIALIZE_ALIAS(reviews, count, review_count, size_t);
}

static std::string querystring(const std::string& value) {
  static constexpr char hex[] = "0123456789abcdef";
  std::string output{};

  output.reserve(value.length());

  for (size_t i{}; i < value.length(); i++) {
    const auto c{value[i]};

    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9')) {
      output.push_back(c);
    } else {
      output.push_back('%');
      output.push_back(hex[(c >> 4) & 0x0f]);
      output.push_back(hex[c & 0x0f]);
    }
  }

  return output;
}

void bot_query::add_query(const char* key, const uint16_t value, const uint16_t max) {
  m_query.insert_or_assign(key, std::to_string(std::min(value, max)));
}

void bot_query::send(const topgg::get_bots_completion_event& callback) {
  std::string path{"/bots?"};

  if (m_sort != nullptr) {
    path.append("sort=");
    path.append(m_sort);
    path.push_back('&');
  }

  for (const auto& additional_query: m_query) {
    path.append(additional_query.first);
    path.push_back('=');
    path.append(additional_query.second);
    path.push_back('&');
  }

  path.pop_back();

  m_client->basic_request<std::vector<topgg::bot>>(dpp::m_get, path, callback, [](const auto& j) {
    std::vector<topgg::bot> bots{};

    bots.reserve(j["count"].template get<size_t>());

    for (const auto& bot: j["results"].template get<std::vector<dpp::json>>()) {
      bots.push_back(topgg::bot{bot});
    }

    return bots;
  });
}

#ifdef DPP_CORO
dpp::async<std::vector<topgg::bot>> bot_query::co_send() {
  return dpp::async<std::vector<topgg::bot>>{[this]<typename C>(C&& cc) { return send(std::forward<C>(cc)); }};
}
#endif

static time_t parse_vote_time(const dpp::json& j, const char* key) {
  auto j_text{j[key].template get<std::string>()};
  tm text_tm{};

  const auto dot_pos{j_text.find('.')};
  
  if (dot_pos != std::string::npos) {
    j_text = j_text.substr(0, dot_pos);
  }
                               
  strptime(j_text.data(), "%Y-%m-%dT%H:%M:%S", &text_tm);

  return mktime(&text_tm);
}

voter::voter(const dpp::json& j) {
  id = _TOPGG_SNOWFLAKE_FROM_JSON(j, id);

  _TOPGG_DESERIALIZE(j, username, std::string);
  _TOPGG_DESERIALIZE(j, avatar, std::string);

  created_at = timestamp_from_id(id);
}