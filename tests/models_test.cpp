#include <gtest/gtest.h>
#include <topgg/topgg.h>

TEST(ModelsTest, BotVoteDataFromJson) {
    nlohmann::json test_json = {
        {"user", "123456789"},
        {"type", "upvote"},
        {"isWeekend", true},
        {"query", "test_query"}
    };

    auto vote_data = topgg::bot_vote_data::from_json(test_json);
    
    EXPECT_EQ(vote_data.user_id, 123456789);
    EXPECT_EQ(vote_data.type, "upvote");
    EXPECT_TRUE(vote_data.is_weekend);
    EXPECT_EQ(vote_data.query, "test_query");
} 