#pragma once

#include <topgg/topgg.h>

#include <functional>
#include <stdexcept>
#include <variant>

namespace topgg {
  class internal_result;

  /**
   * @brief An exception that gets thrown when the client receives an unexpected error from Top.gg's end.
   */
  class internal_server_error: public std::runtime_error {
    inline internal_server_error()
      : std::runtime_error("Received an unexpected error from Top.gg's end.") {}

    friend class internal_result;
  };

  /**
   * @brief An exception that gets thrown when its known that the client uses an invalid Top.gg API token.
   */
  class invalid_token: public std::invalid_argument {
    inline invalid_token()
      : std::invalid_argument("Invalid Top.gg API token.") {}

    friend class internal_result;
  };

  /**
   * @brief An exception that gets thrown when such query does not exist.
   */
  class not_found: public std::runtime_error {
    inline not_found()
      : std::runtime_error("Such query does not exist.") {}

    friend class internal_result;
  };

  /**
   * @brief An exception that gets thrown when the client gets ratelimited from sending more HTTP requests.
   */
  class ratelimited: public std::runtime_error {
    inline ratelimited(const uint16_t retry_after_in)
      : std::runtime_error("This client is ratelimited from further requests. Please try again later."), retry_after(retry_after_in) {}

  public:
    /**
     * @brief The amount of seconds before the ratelimit is lifted.
     */
    const uint16_t retry_after;

    /**
     * @brief No outsiders are allowed to initiate this class, internal use only :)
     */
    ratelimited() = delete;

    friend class internal_result;
  };

  class client;

  template<typename T>
  class result;

  /**
   * @brief Internal result class.
   * This class is private and can't be used outside of topgg's internal code.
   */
  class TOPGG_EXPORT internal_result {
    const dpp::http_request_completion_t m_response;

    void prepare() const;

    inline internal_result(const dpp::http_request_completion_t& response)
      : m_response(response) {}

  public:
    /**
     * @brief No outsiders are allowed to use this class, internal use only :)
     */
    internal_result() = delete;

    template<typename T>
    friend class result;
  };

  /**
   * @brief A result class that gets returned from every HTTP response.
   * This class may either contain the desired data or an error.
   */
  template<typename T>
  class TOPGG_EXPORT result {
    const std::function<T(dpp::json& json)> m_parse_fn;
    const internal_result m_internal;

    inline result(const dpp::http_request_completion_t& response, const std::function<T(const dpp::json&)>& parse_fn)
      : m_internal(internal_result{response}), m_parse_fn(parse_fn) {}

  public:
    /**
     * @brief No outsiders are allowed to initiate this class, internal use only :)
     */
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
     */
    T get() const {
      m_internal.prepare();

      return m_parse_fn(dpp::json::parse(m_response.body));
    }

    friend class client;
  };
}; // namespace topgg