#include <gtest/gtest.h>
#include <topgg/topgg.h>

class WebhookTest : public ::testing::Test {
protected:
    dpp::cluster bot{"test_token"};
    topgg::webhook_manager webhook{bot, 8080, "test_auth"};
};

TEST_F(WebhookTest, BasicTest) {
    EXPECT_NO_THROW({
        webhook.on_vote([](const topgg::bot_vote_data& vote) {
            // Just testing setup doesn't throw
        });
    });
} 