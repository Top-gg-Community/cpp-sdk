#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>

#include "topgg/topgg.hpp"

// Put your bot token here
const std::string BOT_TOKEN = "YOUR_BOT_TOKEN";

int main()
{
    topgg::Api api(BOT_TOKEN);

    // Get the list of users that have voted for the bot
    const auto [httpCode, responseBody] = api.get("/bots/"+api.getBotID()+"/votes");
    std::vector<topgg::Vote> votes;
    if (httpCode == 200) {
        votes = topgg::Vote::fromJsonArray(responseBody);
    } else {
        std::cerr << "Failed to get votes: " << responseBody << "\n";
        return 1;
    }

    // Group the votes by user ID and count their frequency
    std::unordered_map<std::string, int> voteCounts;
    for (const auto& vote : votes) {
        ++voteCounts[vote.getUserID()];
    }

    // Sort the users by vote count in descending order
    std::vector<std::pair<std::string, int>> usersByVotes(voteCounts.begin(), voteCounts.end());
    std::sort(usersByVotes.begin(), usersByVotes.end(), [](const auto& a, const auto& b){
        return a.second > b.second;
    });

    // Print the leaderboard
    std::cout << "Leaderboard:\n";
    for (size_t i = 0; i < usersByVotes.size() && i < 10; ++i) {
        std::cout << i+1 << ". User " << usersByVotes[i].first << ": " << usersByVotes[i].second << " votes\n";
    }

    return 0;
}
