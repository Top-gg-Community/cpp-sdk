#pragma once

#include <dpp/dpp.h>
#include <nlohmann/json.hpp>
#include <functional>
#include <string>

namespace topgg {

/**
 * @brief Data received from a bot vote webhook
 */
struct bot_vote_data {
    dpp::snowflake user_id;          ///< ID of the user who voted
    std::string type;                ///< Type of vote (usually "upvote")
    bool is_weekend;                 ///< Whether the vote was done during weekend
    std::string query;               ///< Query string data if any
    
    static bot_vote_data from_json(const nlohmann::json& j) {
        bot_vote_data data;
        data.user_id = dpp::snowflake(j["user"].get<std::string>());
        data.type = j["type"].get<std::string>();
        data.is_weekend = j["isWeekend"].get<bool>();
        if (j.contains("query")) {
            data.query = j["query"].get<std::string>();
        }
        return data;
    }
};

/**
 * @brief Webhook manager for handling Top.gg vote notifications
 */
class webhook_manager {
public:
    /**
     * @brief Construct a new webhook manager
     * @param cluster Reference to the DPP cluster
     * @param port Port to listen on
     * @param auth_token Authentication token for webhook verification
     */
    webhook_manager(dpp::cluster& cluster, uint16_t port, const std::string& auth_token) 
        : cluster_(cluster), port_(port), auth_token_(auth_token) {
        
        // Set up interaction handler for webhooks
        cluster_.on_interaction_create([this](const dpp::interaction_create_t& event) {
            // Only handle webhook interactions
            if (event.raw_event.empty()) {
                return;
            }

            try {
                nlohmann::json raw_json = nlohmann::json::parse(event.raw_event);
                
                // Verify authorization
                if (!raw_json.contains("Authorization") || 
                    raw_json["Authorization"].get<std::string>() != auth_token_) {
                    event.reply(dpp::ir_channel_message_with_source, 
                              dpp::message("Unauthorized"), 
                              [](const dpp::confirmation_callback_t& cb) {});
                    return;
                }

                if (!raw_json.contains("body")) {
                    event.reply(dpp::ir_channel_message_with_source,
                              dpp::message("Missing request body"),
                              [](const dpp::confirmation_callback_t& cb) {});
                    return;
                }

                nlohmann::json json_data = nlohmann::json::parse(raw_json["body"].get<std::string>());
                auto vote_data = bot_vote_data::from_json(json_data);
                
                if (vote_callback_) {
                    vote_callback_(vote_data);
                }
                
                event.reply(dpp::ir_channel_message_with_source,
                          dpp::message("OK"),
                          [](const dpp::confirmation_callback_t& cb) {});
            } catch (const std::exception& e) {
                event.reply(dpp::ir_channel_message_with_source,
                          dpp::message("Invalid request payload"),
                          [](const dpp::confirmation_callback_t& cb) {});
            }
        });
    }

    /**
     * @brief Set the callback for bot votes
     * @param callback Function to call when a vote is received
     */
    void on_vote(std::function<void(const bot_vote_data&)> callback) {
        vote_callback_ = std::move(callback);
    }

    /**
     * @brief Start the webhook server
     */
    void start() {
        // The webhook handling is already set up in the constructor
        // and will be active as long as the cluster is running
    }

    /**
     * @brief Stop the webhook server
     */
    void stop() {
        // The webhook handling will stop when the cluster stops
    }

private:
    dpp::cluster& cluster_;
    uint16_t port_;
    std::string auth_token_;
    std::function<void(const bot_vote_data&)> vote_callback_;
};

} // namespace topgg