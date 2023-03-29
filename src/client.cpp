#include "topgg/client.hpp"
#include "topgg/http/request.hpp"
#include "topgg/http/response.hpp"

#include <sstream>
#include <stdexcept>

using namespace topgg;
using namespace topgg::http;

Client::Client(const std::string& token, const std::string& botId)
    : m_token(token), m_botId(botId)
{
}

std::string Client::getBotId() const
{
    return m_botId;
}

std::string Client::getToken() const
{
    return m_token;
}

std::vector<UserVote> Client::getVotes()
{
    Request request(Request::Method::GET, "https://top.gg/api/bots/" + m_botId + "/votes");
    request.setHeader("Authorization", m_token);
    auto [statusCode, responseBody] = request.send();

    if (statusCode == 200) {
        std::vector<UserVote> votes;
        std::istringstream iss(responseBody);
        std::string line;
        while (std::getline(iss, line)) {
            std::istringstream iss2(line);
            std::string userIdStr;
            std::string voteTimeStr;
            if (std::getline(iss2, userIdStr, ',')) {
                if (std::getline(iss2, voteTimeStr)) {
                    try {
                        std::chrono::system_clock::time_point voteTime = std::chrono::system_clock::from_time_t(std::stoi(voteTimeStr));
                        votes.emplace_back(userIdStr, voteTime);
                    } catch (const std::exception&) {
                        continue;
                    }
                }
            }
        }
        return votes;
    } else {
        throw TopggAPIError("Failed to get votes: " + responseBody);
    }
}

BotStats Client::getStats()
{
    Request request(Request::Method::GET, "https://top.gg/api/bots/" + m_botId + "/stats");
    request.setHeader("Authorization", m_token);
    auto [statusCode, responseBody] = request.send();

    if (statusCode == 200) {
        nlohmann::json json = nlohmann::json::parse(responseBody);
        int serverCount = json.value("server_count", 0);
        std::vector<int> shardCount = json.value("shards", std::vector<int>{});
        std::vector<int> shardIds;
        if (!shardCount.empty()) {
            shardIds.resize(shardCount.size());
            std::iota(shardIds.begin(), shardIds.end(), 0);
        }
        return BotStats(serverCount, shardCount, shardIds);
    } else {
        throw TopggAPIError("Failed to get stats: " + responseBody);
    }
}

void Client::updateStats(const BotStats& stats)
{
    nlohmann::json json;
    json["server_count"] = stats.getServerCount();
    json["shards"] = stats.getShardCount();
    json["shard_ids"] = stats.getShardIds();

    Request request(Request::Method::POST, m_baseUrl + "bots/" + m_botId + "/stats");
    request.setHeader("Authorization", m_token);
    request.setHeader("Content-Type", "application/json");
    request.setBody(json.dump());

    auto [statusCode, responseBody] = request.send();

    if (statusCode != 200) {
        throw TopggAPIError("Failed to update stats: " + responseBody);
    }
}