#include <topgg/topgg.h>

using dpp::json;

using topgg::internal_result;
using topgg::internal_server_error;
using topgg::invalid_token;
using topgg::not_found;
using topgg::ratelimited;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif

[[maybe_unused]] static const char* get_dpp_error_message(const dpp::http_error& http_error) {
  switch (http_error) {
  case dpp::h_unknown:
    return "Status unknown.";

  case dpp::h_connection:
    return "Connect failed.";

  case dpp::h_bind_ip_address:
    return "Invalid local IP address.";

  case dpp::h_read:
    return "Read error.";

  case dpp::h_write:
    return "Write error.";

  case dpp::h_exceed_redirect_count:
    return "Too many 30x redirects.";

  case dpp::h_canceled:
    return "Request cancelled.";

  case dpp::h_ssl_connection:
    return "SSL connection error.";

  case dpp::h_ssl_loading_certs:
    return "SSL cert loading error.";

  case dpp::h_ssl_server_verification:
    return "SSL server verification error.";

  case dpp::h_unsupported_multipart_boundary_chars:
    return "Unsupported multipart boundary characters.";

  case dpp::h_compression:
    return "Compression error.";

  default:
    return "";
  }
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

void internal_result::handle_response(const dpp::http_request_completion_t& response) {
  if (response.error != dpp::h_success) {
    throw response.error;
  } else if (response.status >= 400) {
    switch (response.status) {
      case 401: {
        throw invalid_token{};
      }
  
      case 404: {
        throw not_found{};
      }
  
      case 429: {
        const auto j{json::parse(response.body)};
        const auto retry_after{j["retry_after"].template get<uint16_t>()};
  
        throw ratelimited{retry_after};
      }
  
      default: {
        throw internal_server_error{};
      }
    }
  }
}