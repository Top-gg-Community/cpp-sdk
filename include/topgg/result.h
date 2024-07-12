/**
 * @module topgg
 * @file result.h
 * @brief The official C++ wrapper for the Top.gg API.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024 Top.gg & null8626
 * @date 2024-07-09
 * @version 2.0.0
 */

#pragma once

#include <topgg/topgg.h>

#include <functional>
#include <stdexcept>
#include <variant>

#ifdef DPP_CORO
#include <utility>
#endif

namespace topgg {
  class internal_result;

  /**
   * @brief An exception that gets thrown when the client receives an unexpected error from Top.gg's end.
   *
   * @since 2.0.0
   */
  class internal_server_error: public std::runtime_error {
    inline internal_server_error()
      : std::runtime_error("Received an unexpected error from Top.gg's end.") {}

    friend class internal_result;
  };

  /**
   * @brief An exception that gets thrown when its known that the client uses an invalid Top.gg API token.
   *
   * @since 2.0.0
   */
  class invalid_token: public std::invalid_argument {
    inline invalid_token()
      : std::invalid_argument("Invalid Top.gg API token.") {}

    friend class internal_result;
  };

  /**
   * @brief An exception that gets thrown when such query does not exist.
   *
   * @since 2.0.0
   */
  class not_found: public std::runtime_error {
    inline not_found()
      : std::runtime_error("Such query does not exist.") {}

    friend class internal_result;
  };

  /**
   * @brief An exception that gets thrown when the client gets ratelimited from sending more HTTP requests.
   *
   * @since 2.0.0
   */
  class ratelimited: public std::runtime_error {
    inline ratelimited(const uint16_t retry_after_in)
      : std::runtime_error("This client is ratelimited from further requests. Please try again later."), retry_after(retry_after_in) {}

  public:
    /**
     * @brief The amount of seconds before the ratelimit is lifted.
     *
     * @since 2.0.0
     */
    const uint16_t retry_after;
    
    ratelimited() = delete;

    friend class internal_result;
  };
  
  template<typename T>
  class result;

  class TOPGG_EXPORT internal_result {
    const dpp::http_request_completion_t m_response;

    void prepare() const;

    inline internal_result(const dpp::http_request_completion_t& response)
      : m_response(response) {}

  public:
    internal_result() = delete;

    template<typename T>
    friend class result;
  };
  
  class client;

  /**
   * @brief A result class that gets returned from every HTTP response.
   * This class may either contain the desired data or an error.
   *
   * @see topgg::async_result
   * @since 2.0.0
   */
  template<typename T>
  class TOPGG_EXPORT result {
    const internal_result m_internal;
    const std::function<T(dpp::json& json)> m_parse_fn;

    inline result(const dpp::http_request_completion_t& response, const std::function<T(const dpp::json&)>& parse_fn)
      : m_internal(response), m_parse_fn(parse_fn) {}

  public:
    result() = delete;

    /**
     * @brief Tries to retrieve the returned data inside.
     *
     * @throw topgg::internal_server_error Thrown when the client receives an unexpected error from Top.gg's end.
     * @throw topgg::invalid_token Thrown when its known that the client uses an invalid Top.gg API token.
     * @throw topgg::not_found Thrown when such query does not exist.
     * @throw topgg::ratelimited Thrown when the client gets ratelimited from sending more HTTP requests.
     * @throw dpp::http_error Thrown when an unexpected HTTP exception occured.
     * @return T The desired data, if successful.
     * @since 2.0.0
     */
    T get() const {
      m_internal.prepare();

      return m_parse_fn(dpp::json::parse(m_internal.m_response.body));
    }

    friend class client;
  };

#ifdef DPP_CORO
  /**
   * @brief An async result class that gets returned from every C++20 coroutine HTTP response.
   * This class may either contain the desired data or an error.
   *
   * @see topgg::result
   * @since 2.0.0
   */
  template<typename T>
  class TOPGG_EXPORT async_result {
    dpp::async<result<T>> m_fut;
    
    template<class F>
    inline async_result(F&& cb): m_fut(std::forward<F>(cb)) {}
    
  public:
    async_result() = delete;
    
    /**
     * @brief This object can't be copied.
     *
     * @param other Other object to copy from.
     * @since 2.0.0
     */
    async_result(const async_result& other) = delete;

    /**
     * @brief Moves data from another object.
     *
     * @param other Other object to move from.
     * @since 2.0.0
     */
    async_result(async_result&& other) noexcept = default;

    /**
     * @brief This object can't be copied.
     *
     * @param other Other object to copy from.
     * @return async_result The current modified object.
     * @since 2.0.0
     */
    async_result& operator=(const async_result& other) = delete;
  
    /**
     * @brief Moves data from another object.
     *
     * @param other Other object to move from.
     * @return async_result The current modified object.
     * @since 2.0.0
     */
    async_result& operator=(async_result&& other) noexcept = default;
  
    /**
     * @brief Suspends the caller and tries to retrieve the fetched data.
     *
     * @throw topgg::internal_server_error Thrown when the client receives an unexpected error from Top.gg's end.
     * @throw topgg::invalid_token Thrown when its known that the client uses an invalid Top.gg API token.
     * @throw topgg::not_found Thrown when such query does not exist.
     * @throw topgg::ratelimited Thrown when the client gets ratelimited from sending more HTTP requests.
     * @throw dpp::http_error Thrown when an unexpected HTTP exception occured.
     * @return T The desired data, if successful.
     * @see topgg::result::get
     * @since 2.0.0
     */
    inline T& operator co_await() & {
      return m_fut.operator co_await().get();
    }
    
    /**
     * @brief Suspends the caller and tries to retrieve the fetched data.
     *
     * @throw topgg::internal_server_error Thrown when the client receives an unexpected error from Top.gg's end.
     * @throw topgg::invalid_token Thrown when its known that the client uses an invalid Top.gg API token.
     * @throw topgg::not_found Thrown when such query does not exist.
     * @throw topgg::ratelimited Thrown when the client gets ratelimited from sending more HTTP requests.
     * @throw dpp::http_error Thrown when an unexpected HTTP exception occured.
     * @return T The desired data, if successful.
     * @see topgg::result::get
     * @since 2.0.0
     */
    inline const T& operator co_await() const & {
      return m_fut.operator co_await().get();
    }
    
    /**
     * @brief Suspends the caller and tries to retrieve the fetched data.
     *
     * @throw topgg::internal_server_error Thrown when the client receives an unexpected error from Top.gg's end.
     * @throw topgg::invalid_token Thrown when its known that the client uses an invalid Top.gg API token.
     * @throw topgg::not_found Thrown when such query does not exist.
     * @throw topgg::ratelimited Thrown when the client gets ratelimited from sending more HTTP requests.
     * @throw dpp::http_error Thrown when an unexpected HTTP exception occured.
     * @return T The desired data, if successful.
     * @see topgg::result::get
     * @since 2.0.0
     */
    inline T&& operator co_await() && {
      return std::forward<dpp::async<result<T>>>(m_fut).operator co_await().get();
    }
    
    friend class client;
  };
#endif
}; // namespace topgg