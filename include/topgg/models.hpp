#ifndef TOPGG_MODELS_HPP
#define TOPGG_MODELS_HPP

#include <string>
#include <vector>
#include <chrono>
#include <memory>

namespace topgg {

class User {
public:
    User(const std::string& id, const std::string& username, const std::string& discriminator,
         const std::string& avatarUrl);

    std::string getId() const;
    std::string getUsername() const;
    std::string getDiscriminator() const;
    std::string getAvatarUrl() const;

private:
    std::string m_id;
    std::string m_username;
    std::string m_discriminator;
    std::string m_avatarUrl;
};

class BotStats {
public:
    BotStats(int serverCount, const std::vector<int>& shardCount, const std::vector<int>& shardIds);

    int getServerCount() const;
    std::vector<int> getShardCount() const;
    std::vector<int> getShardIds() const;

private:
    int m_serverCount;
    std::vector<int> m_shardCount;
    std::vector<int> m_shardIds;
};

class UserVote {
public:
    UserVote(const std::string& userId, std::chrono::system_clock::time_point voteTime);

    std::string getUserId() const;
    std::chrono::system_clock::time_point getVoteTime() const;

private:
    std::string m_userId;
    std::chrono::system_clock::time_point m_voteTime;
};

} // namespace topgg

#endif // TOPGG_MODELS_HPP
