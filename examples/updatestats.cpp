#include "topgg/client.hpp"
#include "topgg/models.hpp"

int main()
{
    topgg::Client client("YOUR_TOPGG_TOKEN", "YOUR_BOT_ID");

    int serverCount = 1000; // Replace with your bot's server count
    std::vector<int> shardCount = {500, 500}; // Replace with your bot's shard count
    std::vector<int> shardIds = {0, 1}; // Replace with your bot's shard IDs

    topgg::BotStats stats(serverCount, shardCount, shardIds);

    try {
        client.updateStats(stats);
        std::cout << "Bot stats successfully updated on Top.gg" << std::endl;
    } catch (const topgg::TopggAPIError& error) {
        std::cerr << "Failed to update bot stats: " << error.what() << std::endl;
    }

    return 0;
}