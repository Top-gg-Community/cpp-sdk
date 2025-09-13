/**
 * @module topgg
 * @file drogon.h
 * @brief A simple API wrapper for Top.gg written in C++.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024-2025 Top.gg & null8626
 * @date 2025-09-13
 * @version 3.0.0
 */

#pragma once

#ifndef __TOPGG_DROGON_WEBHOOKS__
#define __TOPGG_DROGON_WEBHOOKS__
#endif

#include <topgg/webhooks/models.h>

#include <drogon/drogon.h>

#include <type_traits>
#include <functional>
#include <utility>
#include <string>

#define TOPGG_DROGON_WEBHOOK()                                                                                                                       \
  void asyncHandleHttpRequest(const ::drogon::HttpRequestPtr& request, std::function<void(const ::drogon::HttpResponsePtr &)>&& response) override { \
    __handle(request, std::forward<std::function<void(const ::drogon::HttpResponsePtr &)>>(response));                                               \
  }

namespace topgg {
  namespace webhook {
    class internal_drogon {
    protected:
      const std::string m_authorization;

      std::optional<json> parse(const ::drogon::HttpRequestPtr& request, const ::drogon::HttpResponsePtr& response) const noexcept;
      
      inline internal_drogon(const std::string& authorization): m_authorization(authorization) {}
    
    public:
      internal_drogon() = delete;
    };

    /**
     * @brief An abstract class that receives a Top.gg webhook event. Designed for use as a drogon::HttpSimpleController in drogon.
     *
     * @see topgg::webhook::vote_event
     * @since 3.0.0
     */
    template<class T>
    class drogon: private internal_drogon {
    public:
      drogon() = delete;

      inline drogon(const std::string& authorization): internal_drogon(authorization) {
        static_assert(std::is_constructible_v<T, const json&>, "T must be a valid model class.");
      }

      void __handle(const ::drogon::HttpRequestPtr& request, std::function<void(const ::drogon::HttpResponsePtr &)>&& response) {
        const auto response_data{::drogon::HttpResponse::newHttpResponse()};
        const auto json_data{parse(request, response_data)};

        if (json_data.has_value()) {
          const T data{json_data.value()};
          
          callback(data);
        }

        response(response_data);
      }

      /**
       * @brief The virtual callback that will be called whenever an incoming webhook request to the endpoint has been authenticated.
       *
       * @param T data The webhook event data.
       * @since 3.0.0
       */
      virtual void callback(const T& data) = 0;
    };
  }; // namespace webhook
}; // namespace topgg

#undef __TOPGG_DROGON_WEBHOOKS__