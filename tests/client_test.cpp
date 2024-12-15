#include <gtest/gtest.h>
#include <topgg/topgg.h>

class TopggClientTest : public ::testing::Test {
protected:
    dpp::cluster bot{"test_token"};
    topgg::client client{bot, "test_topgg_token"};
};

TEST_F(TopggClientTest, HasVotedTest) {
    bool callback_called = false;
    
    client.has_voted(123456789, [&callback_called](const auto& result) {
        callback_called = true;
        try {
            bool has_voted = result.get();
            EXPECT_FALSE(has_voted); // Expect false since we're using test token
        } catch (const std::exception& e) {
            FAIL() << "Should not throw: " << e.what();
        }
    });

    EXPECT_TRUE(callback_called);
}
