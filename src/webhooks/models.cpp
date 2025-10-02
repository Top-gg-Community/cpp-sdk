#include <topgg/webhooks/models.h>

#include <exception>
#include <sstream>

using namespace topgg::webhook;

static std::unordered_map<std::string, std::string> parse_query_string(const std::string& query) {
  std::unordered_map<std::string, std::string> output{};

  std::istringstream ss{query.substr(query.find('?') == 0 ? 1 : 0)};
  std::string pair{};

  while (std::getline(ss, pair, '&')) {
    const auto eq_pos{pair.find('=')};

    if (eq_pos != std::string::npos) {
      output[pair.substr(0, eq_pos)] = pair.substr(eq_pos + 1);
    }
  }

  return output;
}

vote_event::vote_event(const json& j) {
#ifdef __TOPGG_DROGON_WEBHOOKS__
  receiver_id = j["bot"].asString();

  if (receiver_id.empty()) {
    receiver_id = j["guild"].asString();
  }

  voter_id = j["user"].asString();
  is_test = j["type"].asString() == "test";
  is_weekend = j.get("isWeekend", false).asBool();

  const auto query_string{j["query"].asString()};

  query = parse_query_string(query_string);
#else
  try {
    receiver_id = j["bot"].template get<std::string>();
  } catch (TOPGG_UNUSED const std::exception&) {
    receiver_id = j["guild"].template get<std::string>();
  }

  voter_id = j["user"].template get<std::string>();

  try {
    const auto type{j["type"].template get<std::string>()};

    is_test = type == "test";
  } catch (TOPGG_UNUSED const std::exception&) {
    is_test = false;
  }

  try {
    is_weekend = j["isWeekend"].template get<bool>();
  } catch (TOPGG_UNUSED const std::exception&) {
    is_weekend = false;
  }

  try {
    const auto query_string{j["query"].template get<std::string>()};

    query = parse_query_string(query_string);
  } catch (TOPGG_UNUSED const std::exception&) {}
#endif
}