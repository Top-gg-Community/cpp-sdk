#pragma once

#include <dpp/dpp.h>

#include <topgg/dpp.h>

#include <functional>
#include <stdexcept>
#include <variant>

#define TOPGG_ERROR_CLASS(class_name, parent_class, what_response) \
  class class_name: public parent_class {                          \
    inline class_name(): parent_class(what_response) {}            \
                                                                   \
    template<typename T>                                           \
    friend class result;                                           \
  }

namespace topgg {
  template<typename T>
  class result;
  
  TOPGG_ERROR_CLASS(internal_server_error, std::runtime_error, "Received an unexpected error from Top.gg's end.");
  TOPGG_ERROR_CLASS(invalid_token, std::invalid_argument, "Invalid Top.gg API token.");
  TOPGG_ERROR_CLASS(not_found, std::runtime_error, "Such query does not exist.");
  
  class ratelimited: public std::runtime_error {
    const uint16_t m_retry_after;
  
    inline ratelimited(const uint16_t retry_after): m_retry_after(retry_after), std::runtime_error("This client is ratelimited from further requests. Please try again later.") {}
  public:
    inline constexpr uint16_t retry_after() const noexcept {
      return m_retry_after;
    }
    
    template<typename T>
    friend class result;
  };
  
  class internal_client_error: public std::runtime_error {
    dpp::http_error m_http_error;
  
    inline internal_client_error(dpp::http_error http_error, const char* message): m_http_error(http_error), std::runtime_error(message) {}
    static const char* get_what(const dpp::http_error& http_error);
  public:
    inline constexpr dpp::http_error const& get_http_error() const noexcept {
      return m_http_error;
    }
    
    template<typename T>
    friend class result;
  };
  
  class client;
  
  template<typename T>
  class result {
    const dpp::http_request_completion_t m_response;
    const std::function<T(dpp::json& json)> m_parse_fn;
    
    inline constexpr result(dpp::http_request_completion_t response, std::function<T(const dpp::json& json)> parse_fn): m_response(response), m_parse_fn(parse_fn) {}
  public:
    T get() const;
  
    friend class client;
  };
};

#undef TOPGG_ERROR_CLASS