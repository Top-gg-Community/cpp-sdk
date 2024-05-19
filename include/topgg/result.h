#pragma once

#include <topgg/topgg.h>

#include <functional>
#include <stdexcept>
#include <variant>

#define TOPGG_ERROR_CLASS(class_name, parent_class, what_response) \
  class class_name: public parent_class {                          \
    inline class_name(): parent_class(what_response) {}            \
                                                                   \
    friend class result_internal;                                  \
  }

namespace topgg {
  class result_internal;
  
  TOPGG_ERROR_CLASS(internal_server_error, std::runtime_error, "Received an unexpected error from Top.gg's end.");
  TOPGG_ERROR_CLASS(invalid_token, std::invalid_argument, "Invalid Top.gg API token.");
  TOPGG_ERROR_CLASS(not_found, std::runtime_error, "Such query does not exist.");
  
  class ratelimited: public std::runtime_error {
    const uint16_t m_retry_after;
  
    inline ratelimited(const uint16_t retry_after): std::runtime_error("This client is ratelimited from further requests. Please try again later."), m_retry_after(retry_after) {}
  public:
    inline constexpr uint16_t retry_after() const noexcept {
      return m_retry_after;
    }
    
    friend class result_internal;
  };
  
  class internal_client_error: public std::runtime_error {
    dpp::http_error m_http_error;
  
    inline internal_client_error(dpp::http_error http_error, const char* message): std::runtime_error(message), m_http_error(http_error) {}
  public:
    inline constexpr dpp::http_error const& get_http_error() const noexcept {
      return m_http_error;
    }
    
    friend class result_internal;
  };
  
  class client;
  
  // this doesn't need to be exported smh
  class TOPGG_EXPORT result_internal {
  protected:
    const dpp::http_request_completion_t m_response;
  
    void prepare() const;
    inline result_internal(dpp::http_request_completion_t response): m_response(response) {}
    
  public:
    result_internal() = delete;
  };
  
  template<typename T>
  class TOPGG_EXPORT result: private result_internal {
    const std::function<T(dpp::json& json)> m_parse_fn;
    
    inline result(dpp::http_request_completion_t response, std::function<T(const dpp::json& json)> parse_fn): result_internal(response), m_parse_fn(parse_fn) {}
  public:
    T get() const {
      result_internal::prepare();
      
      return m_parse_fn(dpp::json::parse(m_response.body));
    }
  
    friend class client;
  };
};

#undef TOPGG_ERROR_CLASS