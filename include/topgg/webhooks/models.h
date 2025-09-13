/**
 * @module topgg
 * @file models.h
 * @brief A simple API wrapper for Top.gg written in C++.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024-2025 Top.gg & null8626
 * @date 2025-09-13
 * @version 3.0.0
 */

#pragma once

#include <topgg/export.h>

#ifdef __TOPGG_DROGON_WEBHOOKS__
#include <json/json.h>
#else
#include <nlohmann/json.hpp>
#endif

#include <unordered_map>
#include <string>

namespace topgg {
  namespace webhook {
#ifdef __TOPGG_DROGON_WEBHOOKS__
    using json = Json::Value;
#else
    using json = nlohmann::json;
#endif

    /**
     * @brief A dispatched Top.gg vote webhook event.
     *
     * @since 3.0.0
     */
    class vote_event {
    public:
      TOPGG_EXPORT vote_event(const json& j);

      vote_event() = delete;
  
      /**
       * @brief The ID of the project that received a vote.
       *
       * @since 3.0.0
       */
      std::string receiver_id;
  
      /**
       * @brief The ID of the Top.gg user who voted.
       *
       * @since 3.0.0
       */
      std::string voter_id;
  
      /**
       * @brief Whether this vote is just a test done from the page settings.
       *
       * @since 3.0.0
       */
      bool is_test;
  
      /**
       * @brief Whether the weekend multiplier is active, where a single vote counts as two.
       *
       * @since 3.0.0
       */
      bool is_weekend;
  
      /**
       * @brief Query strings found on the vote page.
       *
       * @since 3.0.0
       */
      std::unordered_map<std::string, std::string> query;
    };
  }; // namespace webhook
}; // namespace topgg