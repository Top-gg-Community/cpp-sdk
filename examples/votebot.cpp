#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "topgg/topgg.hpp"

// Put your Top.gg bot token here
const std::string TOPGG_TOKEN = "YOUR_TOPGG_TOKEN";

// Put your Top.gg bot webhook server port here
const uint16_t PORT = 5000;

int main()
{
    topgg::Api api(TOPGG_TOKEN);
    topgg::WebhookServer server(api, PORT);

    server.onVote([&api](const topgg::Vote& vote) {
        // Launch in a new thread for non-blocking behavior
        std::thread([vote, &api] {
            // Increment the vote count for this user or do something else with the vote data
            std::cout << "User " << vote.getUserID() << " voted for the bot!\n";

            // Check if there was an error sending a thank you message to the user
            const auto [httpCode, responseBody] = api.post("/bots/stats/votes", {{"bot", api.getBotID()}, {"user", vote.getUserID()}});
            if (httpCode != 200) {
                std::cerr << "Failed to thank the user for voting: " << responseBody << "\n";
            }
        }).detach();
    });

    server.start();

    return 0;
}