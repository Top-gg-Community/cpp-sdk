/**
 * @module topgg
 * @file cpp-httplib.h
 * @brief A community-maintained C++ API Client for the Top.gg API.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024-2025 Top.gg & null8626
 * @date 2025-10-02
 * @version 2.1.0
 */

#pragma once

#ifndef CPPHTTPLIB_HTTPLIB_H
#include <cpp-httplib/cpp-httplib.h>
#endif

#include <topgg/webhooks/models.h>

#include <type_traits>
#include <functional>
#include <optional>
#include <string>

namespace topgg {
  namespace webhook {
    class internal_cpp_httplib {
    protected:
      const std::string m_authorization;

      std::optional<json> parse(const httplib::Request& request, httplib::Response& response) const noexcept;
      
      inline internal_cpp_httplib(const std::string& authorization): m_authorization(authorization) {}
    
    public:
      internal_cpp_httplib() = delete;
    };

    using cpp_httplib_endpoint = std::function<void(const httplib::Request&, httplib::Response&)>;
    
    /**
     * @brief An abstract class that receives a Top.gg webhook event. Designed for use in cpp-httplib.
     *
     * @see topgg::webhook::vote_event
     * @since 2.1.0
     */
    template<class T>
    class cpp_httplib: private internal_cpp_httplib {
    public:
      cpp_httplib() = delete;

      inline cpp_httplib(const std::string& authorization): internal_cpp_httplib(authorization) {
        static_assert(std::is_constructible_v<T, const json&>, "T must be a valid model class.");
      }

      /**
       * @brief Returns the endpoint callback to be used in a cpp-httplib server route.
       *
       * @return cpp_httplib_endpoint The endpoint callback.
       * @since 2.1.0
       */
      inline cpp_httplib_endpoint endpoint() noexcept {
        return [this](const httplib::Request& request, httplib::Response& response) {
          const auto json_data{this->parse(request, response)};

          if (json_data.has_value()) {
            const T data{json_data.value()};

            this->callback(data);
          }
        };
      }

      /**
       * @brief The virtual callback that will be called whenever an incoming webhook request to the endpoint has been authenticated.
       *
       * @param T data The webhook event data.
       * @since 2.1.0
       */
      virtual void callback(const T& data) = 0;
    };
  }; // namespace webhook
}; // namespace topgg