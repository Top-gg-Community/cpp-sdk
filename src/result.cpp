#include <topgg/result.h>

using topgg::internal_result;
using topgg::internal_server_error;
using topgg::invalid_token;
using topgg::not_found;
using topgg::ratelimited;

void internal_result::prepare() const {
  if (m_response.error != dpp::h_success) {
    throw m_response.error;
  } else if (m_response.status >= 400) {
    switch (m_response.status) {
    case 401:
      throw invalid_token{};
    case 404:
      throw not_found{};
    case 429: {
      const auto j = dpp::json::parse(m_response.body);
      const auto retry_after = j["retry_after"].template get<uint16_t>();
      throw ratelimited{retry_after};
    }
    default:
      throw internal_server_error{};
    }
  }
}