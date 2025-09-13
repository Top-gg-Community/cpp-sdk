/**
 * @module topgg
 * @file result.h
 * @brief A simple API wrapper for Top.gg written in C++.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024-2025 Top.gg & null8626
 * @date 2025-09-13
 * @version 3.0.0
 */

#pragma once

#include <topgg/topgg.h>

#include <functional>
#include <stdexcept>
#include <variant>
#include <utility>

namespace topgg {
  class internal_result;

  /**
   * @brief Unexpected error from Top.gg's end.
   *
   * @since 2.0.0
   */
  class internal_server_error: public std::runtime_error {
    inline internal_server_error()
      : std::runtime_error("Received an unexpected error from Top.gg's end.") {}

    friend class internal_result;
  };

  /**
   * @brief Invalid API token.
   *
   * @since 2.0.0
   */
  class invalid_token: public std::invalid_argument {
    inline invalid_token()
      : std::invalid_argument("Invalid API token.") {}

    friend class client;
    friend class internal_result;
  };

  /**
   * @brief Such query does not exist.
   *
   * @since 2.0.0
   */
  class not_found: public std::runtime_error {
    inline not_found()
      : std::runtime_error("Such query does not exist.") {}

    friend class internal_result;
  };

  /**
   * @brief Ratelimited from sending more requests.
   *
   * @since 2.0.0
   */
  class ratelimited: public std::runtime_error {
    inline ratelimited(const uint16_t retry_after_in)
      : std::runtime_error("This client is ratelimited from further requests. Please try again later."), retry_after(retry_after_in) {}

  public:
    /**
     * @brief How long the client should wait (in seconds) before it can make a request to the API again.
     *
     * @since 2.0.0
     */
    const uint16_t retry_after;

    ratelimited() = delete;

    friend class internal_result;
  };

  template<typename T>
  class result;

  class internal_result {
    const dpp::http_request_completion_t m_response;

    static void handle_response(const dpp::http_request_completion_t& response);

    inline internal_result(): m_response() {}
    inline internal_result(const dpp::http_request_completion_t& response): m_response(response) {}

    template<typename T>
    friend class result;
    friend class client;
  };

  class client;

  /**
   * @brief The desired data or an error.
   *
   * @see topgg::async_result
   * @since 2.0.0
   */
  template<typename T>
  class result {
    const internal_result m_internal;
    const std::variant<std::function<T(const dpp::json& json)>, T> m_data;

    inline result(const T& data): m_data(data) {}
    inline result(const dpp::http_request_completion_t& response, const std::function<T(const dpp::json&)>& parse_fn): m_internal(response), m_data(parse_fn) {}

  public:
    result() = delete;

    /**
     * @brief Tries to retrieve the data.
     *
     * @throw topgg::internal_server_error Unexpected error from Top.gg's end.
     * @throw topgg::invalid_token Invalid API token.
     * @throw topgg::not_found Such query does not exist.
     * @throw topgg::ratelimited Ratelimited from sending more requests.
     * @throw dpp::http_error An unexpected HTTP exception has occured.
     * @return T The desired data.
     * @since 2.0.0
     */
    T get() const {
      try {
        const auto parse_fn{std::get<std::function<T(const dpp::json& json)>>(m_data)};

        internal_result::handle_response(m_internal.m_response);

        return parse_fn(dpp::json::parse(m_internal.m_response.body));
      } catch (TOPGG_UNUSED const std::bad_variant_access&) {
        return std::get<T>(m_data);
      }
    }

    friend class client;
  };

#ifdef DPP_CORO
  /**
   * @brief The desired data from a C++20 coroutine or an error.
   *
   * @see topgg::result
   * @since 2.0.0
   */
  template<typename T>
  class TOPGG_EXPORT async_result {
    dpp::async<result<T>> m_fut;

    template<class F>
    inline async_result(F&& cb)
      : m_fut(std::forward<F>(cb)) {}

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
     * @brief Suspends the caller and tries to retrieve the data.
     *
     * @throw topgg::internal_server_error Unexpected error from Top.gg's end.
     * @throw topgg::invalid_token Invalid API token.
     * @throw topgg::not_found Such query does not exist.
     * @throw topgg::ratelimited Ratelimited from sending more requests.
     * @throw dpp::http_error An unexpected HTTP exception has occured.
     * @return T The desired data.
     * @see topgg::result::get
     * @since 2.0.0
     */
    inline T& operator co_await() & {
      return m_fut.operator co_await().get();
    }

    /**
     * @brief Suspends the caller and tries to retrieve the data.
     *
     * @throw topgg::internal_server_error Unexpected error from Top.gg's end.
     * @throw topgg::invalid_token Invalid API token.
     * @throw topgg::not_found Such query does not exist.
     * @throw topgg::ratelimited Ratelimited from sending more requests.
     * @throw dpp::http_error An unexpected HTTP exception has occured.
     * @return T The desired data.
     * @see topgg::result::get
     * @since 2.0.0
     */
    inline const T& operator co_await() const& {
      return m_fut.operator co_await().get();
    }

    /**
     * @brief Suspends the caller and tries to retrieve the fetched data.
     *
     * @throw topgg::internal_server_error Unexpected error from Top.gg's end.
     * @throw topgg::invalid_token Invalid API token.
     * @throw topgg::not_found Such query does not exist.
     * @throw topgg::ratelimited Ratelimited from sending more requests.
     * @throw dpp::http_error An unexpected HTTP exception has occured.
     * @return T The desired data.
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