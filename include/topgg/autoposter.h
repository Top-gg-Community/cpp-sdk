#pragma once

#include <topgg/topgg.h>

namespace topgg {

  namespace autoposter {

      /**
       * @brief Base class for autoposter classes.
       */
      class TOPGG_EXPORT base: private ::topgg::base_client {

      /**
       * D++ timer handle. D++ timers have a one second resolution,
       * and can be set to any number of seconds. They will repeat
       * until stopped or the cluster is destructed.
       */
      dpp::timer m_timer_handle{};

      /**
       * Implement this interface to return the bot's statistics
       * @param bot D++ cluster
       * @return topgg::stats instance
       */
      virtual ::topgg::stats get_stats(dpp::cluster& bot) = 0;

    protected:

      /**
       * @brief A shared pointer to the bot's D++ cluster.
       */
      dpp::cluster& m_cluster;

      /**
       * @brief Constructs the base autoposter class.
       *
       * @param cluster A shared pointer to the bot's D++ cluster.
       * @param token The Top.gg API token to use.
       * @param delay The minimum delay between post requests in seconds. This delay mustn't be shorter than 15 minutes.
       * @throw std::invalid_argument Throws if the delay argument is shorter than 15 minutes.
       */
      base(dpp::cluster& cluster, const std::string& token, const time_t delay);

    public:

      /**
       * @brief Don't use this class directly. Use its child classes!
       */
      base() = delete;

      /**
       * @brief This class can't be copied.
       * @param other other
       */
      base(const base& other) = delete;

      /**
       * @brief This class can't be moved.
       * @param other other
       */
      base(base&& other) = delete;

      /**
       * @brief This class can't be assigned from other objects.
       * @param other other
       */
      base& operator=(const base& other) & = delete;

      /**
       * @brief This class can't be assigned from other objects.
       * @param other other
       */
      base& operator=(base&& other) & = delete;

      /**
       * @brief Requests the autoposter timer to stop.
       * @note This function immediately stops the timer, it does not need to block.
       */
      virtual void stop();

      /**
       * @brief Requests the autoposter timer to stop. It does not need to block.
       */
      ~base();

    };

    /**
     * @brief An autoposter class that automatically retrieves the server count itself by using D++ cache.
     * @note Depends on guild cache being enabled in D++ (this is the default)
     */
    class TOPGG_EXPORT cached : public base {

      /**
       * Retrieve stats using the D++ guild cache
       * @param bot cluster pointer
       * @return topgg::stats
       */
      ::topgg::stats get_stats(TOPGG_UNUSED dpp::cluster& bot) override;

    public:
      /**
       * @brief Constructs the autoposter class.
       *
       * @param cluster Reference to the bot's D++ cluster. This pointer is used in the timer handler.
       * @param token The Top.gg API token to be used.
       * @param delay The minimum delay between post requests in seconds. This delay mustn't be shorter than 15 minutes.
       * @throw std::invalid_argument Throws if the delay argument is shorter than 15 minutes.
       */
      cached(dpp::cluster& cluster, const std::string& token, const time_t delay);

      /**
       * @brief That's not how you initiate the class buddy :)
       */
      cached() = delete;

      /**
       * @brief This class can't be copied.
       * @param other other
       */
      cached(const cached& other) = delete;

      /**
       * @brief This class can't be moved.
       * @param other other
       */
      cached(cached&& other) = delete;

      /**
       * @brief This class can't be assigned from other objects.
       * @param other other
       */
      cached& operator=(const cached& other) & = delete;

      /**
       * @brief This class can't be assigned from other objects.
       * @param other other
       */
      cached& operator=(cached&& other) & = delete;
    };

    using custom_autopost_callback = std::function<::topgg::stats(dpp::cluster&)>;

    /**
     * @brief An autoposter class that lets you manually retrieve the stats.
     */
    class TOPGG_EXPORT custom : public base {

      /**
       * Callback for retrieving stats
       */
      custom_autopost_callback m_callback;

      /**
       * Calls the callback to get the stats
       * @param bot D++ cluster
       * @return topgg::stats
       */
      inline ::topgg::stats get_stats(dpp::cluster& bot) override {
        return m_callback(bot);
      }

    public:
      /**
       * @brief Constructs the autoposter class.
       *
       * @param cluster A shared pointer to the bot's D++ cluster. This pointer is used in the timer handler.
       * @param token The Top.gg API token to be used.
       * @param delay The minimum delay between post requests in seconds. This delay mustn't be shorter than 15 minutes.
       * @param callback The callback function that returns the current stats.
       * @throw std::invalid_argument Throws if the delay argument is shorter than 15 minutes.
       */
      inline custom(dpp::cluster& cluster, const std::string& token, const time_t delay, custom_autopost_callback&& callback)
        : base(cluster, token, delay), m_callback(callback) {}

      /**
       * @brief That's not how you initiate the class buddy :)
       */
      custom() = delete;

      /**
       * @brief This class can't be copied.
       * @param other other
       */
      custom(const custom& other) = delete;

      /**
       * @brief This class can't be moved.
       * @param other other
       */
      custom(custom&& other) = delete;

      /**
       * @brief This class can't be assigned from other objects.
       * @param other other
       */
      custom& operator=(const custom& other) & = delete;

      /**
       * @brief This class can't be assigned from other objects.
       * @param other other
       */
      custom& operator=(custom&& other) & = delete;
    };

  }; // namespace autoposter

}; // namespace topgg
