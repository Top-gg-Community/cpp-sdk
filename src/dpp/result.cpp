#include <topgg/dpp.h>

using nlohmann::json;

using topgg::internal_client_error;
using topgg::internal_server_error;
using topgg::invalid_token;
using topgg::not_found;
using topgg::ratelimited;
using topgg::result;

const char* internal_client_error::get_what(const dpp::http_error& http_error) {
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

template <typename T>
T result<T>::get() const {
  if (response.error != dpp::h_success) {
    throw internal_client_error(
        response.error, internal_client_error::get_what(response.error));
  } else if (response.status < 400) {
    return m_parse_fn(json::parse(m_response.body));
  } else {
    switch (response.status) {
      case 401:
        throw invalid_token_error();

      case 404:
        throw not_found_error();

      case 429: {
        const auto j = json::parse(m_response.body);
        const auto retry_after = j["retry_after"].template get<uint16_t>();

        throw ratelimited(retry_after);
      }

      default:
        throw internal_server_error();
    }
  }
}