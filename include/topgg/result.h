/**
 * @module topgg
 * @file result.h
 * @brief The official C++ wrapper for the Top.gg API.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024 Top.gg & null8626
 * @date 2024-07-12
 * @version 2.0.0
 */

#pragma once

#include <topgg/export.h>
#include <topgg/models.h>
#include <dpp/dpp.h>

#include <functional>
#include <stdexcept>
#include <variant>
#include <utility>

namespace topgg {
  class TOPGG_API internal_result {
    protected:
      const dpp::http_request_completion_t m_response;

    protected:
      void prepare() const;
      
      internal_result(const dpp::http_request_completion_t& response)
        : m_response(response) {}

    public:
      internal_result() = delete;
  };

  class TOPGG_API internal_server_error : public std::runtime_error {
    internal_server_error()
      : std::runtime_error("Received an unexpected error from Top.gg's end.") {}

    friend class internal_result;
  };

  class TOPGG_API invalid_token : public std::invalid_argument {
    invalid_token()
      : std::invalid_argument("Invalid Top.gg API token.") {}

    friend class internal_result;
  };

  class TOPGG_API not_found : public std::runtime_error {
    not_found()
      : std::runtime_error("Such query does not exist.") {}

    friend class internal_result;
  };

  class TOPGG_API ratelimited : public std::runtime_error {
    ratelimited(const uint16_t retry_after_in)
      : std::runtime_error("This client is ratelimited from further requests. Please try again later."), retry_after(retry_after_in) {}

  public:
    const uint16_t retry_after;
    
    ratelimited() = delete;

    friend class internal_result;
  };

  template<typename T>
  class TOPGG_API result : protected internal_result {
    const std::function<T(const dpp::json&)> m_parse_fn;

  public:
    result() = delete;

    result(const dpp::http_request_completion_t& response, const std::function<T(const dpp::json&)>& parse_fn)
      : internal_result(response), m_parse_fn(parse_fn) {}

    inline T get() const {
      prepare();
      return m_parse_fn(dpp::json::parse(m_response.body));
    }

    friend class client;
  };

#ifdef DPP_CORO
  template<typename T>
  class TOPGG_API async_result {
    dpp::async<result<T>> m_fut;
    
    template<class F>
    async_result(F&& cb): m_fut(std::forward<F>(cb)) {}
    
  public:
    async_result() = delete;
    async_result(const async_result&) = delete;
    async_result(async_result&&) noexcept = default;
    async_result& operator=(const async_result&) = delete;
    async_result& operator=(async_result&&) noexcept = default;

    inline T& operator co_await() & {
      return m_fut.operator co_await().get();
    }
    
    inline const T& operator co_await() const & {
      return m_fut.operator co_await().get();
    }
    
    inline T&& operator co_await() && {
      return std::forward<dpp::async<result<T>>>(m_fut).operator co_await().get();
    }
    
    friend class client;
  };
#endif
}; // namespace topgg