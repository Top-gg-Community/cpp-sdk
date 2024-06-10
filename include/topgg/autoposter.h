#pragma once

#include <topgg/topgg.h>

#include <condition_variable>
#include <unordered_set>
#include <chrono>
#include <memory>
#include <mutex>

namespace topgg {
  namespace autoposter {
    class cached;

    /**
     * @brief A modified semaphore that can be aborted.
     * This class is private and can't be used outside of topgg's internal code.
     */
    class killable_semaphore {
      std::mutex m_mutex;
      std::condition_variable m_condition;
      size_t m_count;
      bool m_killed;

      inline killable_semaphore(const size_t count)
        : m_count(count), m_killed(false) {}

      void release();
      bool acquire();
      void kill();

    public:
      /**
       * @brief No outsiders are allowed to use this class, internal use only :)
       */
      killable_semaphore() = delete;

      /**
       * @brief This class can't be copied.
       */
      killable_semaphore(const killable_semaphore& other) = delete;

      /**
       * @brief This class can't be moved.
       */
      killable_semaphore(killable_semaphore&& other) = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      killable_semaphore& operator=(const killable_semaphore& other) & = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      killable_semaphore& operator=(killable_semaphore&& other) & = delete;

      friend class cached;
    };

    class base;

    /**
     * @brief A modified timer that can be aborted.
     * This class is private and can't be used outside of topgg's internal code.
     */
    class killable_waiter {
      std::mutex m_mutex;
      std::condition_variable m_condition;
      size_t m_killed;

      inline killable_waiter()
        : m_killed(false) {}

      template<class R, class P>
      bool wait(const std::chrono::duration<R, P>& delay) {
        std::unique_lock<std::mutex> lock{m_mutex};

        m_condition.wait_for(lock, delay, [this]() -> bool { return this->m_killed; });

        return !m_killed;
      }

      void kill();

    public:
      /**
       * @brief This class can't be copied.
       */
      killable_waiter(const killable_waiter& other) = delete;

      /**
       * @brief This class can't be moved.
       */
      killable_waiter(killable_waiter&& other) = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      killable_waiter& operator=(const killable_waiter& other) & = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      killable_waiter& operator=(killable_waiter&& other) & = delete;

      friend class base;
    };

    /**
     * @brief Base class for autoposter classes.
     * This class stores autoposter thread and handles requests with the API, with methods to be implemented by its child classes.
     */
    class TOPGG_EXPORT base: private ::topgg::base_client {
      killable_waiter m_waiter;
      std::thread m_thread;

      virtual inline bool before_fetch() {
        return true;
      }

      virtual inline void after_fetch() {}

      virtual ::topgg::stats get_stats(dpp::cluster* bot) = 0;
      void thread_loop(dpp::cluster* thread_cluster);

    protected:
      /**
       * @brief A shared pointer to the bot's D++ cluster.
       */
      std::shared_ptr<dpp::cluster> m_cluster;

      /**
       * @brief Constructs the base autoposter class.
       *
       * @param cluster A shared pointer to the bot's D++ cluster. This pointer will be used later in the autoposter thread.
       * @param token The Top.gg API token to use.
       * @param delay The minimum delay between post requests. This delay mustn't be shorter than 15 minutes.
       * @throw std::invalid_argument Throwns if the delay argument is shorter than 15 minutes.
       */
      template<class R, class P>
      base(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay)
        : ::topgg::base_client(token), m_cluster(std::shared_ptr{cluster}) {
        if (delay < std::chrono::minutes(15)) {
          throw std::invalid_argument{"Delay mustn't be shorter than 15 minutes."};
        }

        // clang-format off
        m_thread = std::thread([this](const std::chrono::duration<R, P>& t_delay) {
          std::shared_ptr<dpp::cluster> thread_cluster{this->m_cluster};

          while (this->m_waiter.wait(t_delay) && this->before_fetch()) {
            this->thread_loop(thread_cluster.get());
          }
        }, std::cref(delay));
        // clang-format on
      }

    public:
      /**
       * @brief Don't use this class directly. Use its child classes!
       */
      base() = delete;

      /**
       * @brief This class can't be copied.
       */
      base(const base& other) = delete;

      /**
       * @brief This class can't be moved.
       */
      base(base&& other) = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      base& operator=(const base& other) & = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      base& operator=(base&& other) & = delete;

      /**
       * @brief Requests the autoposter thread to stop.
       * @note This function does NOT block and wait for the thread to stop. That's the destructor's job.
       */
      virtual void stop();

      /**
       * @brief Requests the autoposter thread to stop and blocks until the thread is completely shut down.
       */
      ~base();
    };

    /**
     * @brief An autoposter class that automatically retrieves the server count itself by manually listening to Discord's gateway events.
     */
    class TOPGG_EXPORT cached: public base {
      std::mutex m_mutex;
      killable_semaphore m_semaphore;
      std::unordered_set<dpp::snowflake> m_guilds;

      bool before_fetch() override;

      inline void after_fetch() override {
        m_mutex.unlock();
      }

      void start_listening();

      inline ::topgg::stats get_stats(TOPGG_UNUSED dpp::cluster* _) override {
        return ::topgg::stats{m_guilds.size()};
      }

    public:
      /**
       * @brief Constructs the autoposter class.
       *
       * @param cluster A shared pointer to the bot's D++ cluster. This pointer will be used later in the autoposter thread.
       * @param token The Top.gg API token to be used.
       * @param delay The minimum delay between post requests. This delay mustn't be shorter than 15 minutes.
       * @throw std::invalid_argument Throwns if the delay argument is shorter than 15 minutes.
       */
      template<class R, class P>
      inline cached(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay)
        : base(cluster, token, delay), m_semaphore(killable_semaphore{0}) {
        start_listening();
      }

      /**
       * @brief That's not how you initiate the class buddy :)
       */
      cached() = delete;

      /**
       * @brief This class can't be copied.
       */
      cached(const cached& other) = delete;

      /**
       * @brief This class can't be moved.
       */
      cached(cached&& other) = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      cached& operator=(const cached& other) & = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      cached& operator=(cached&& other) & = delete;

      /**
       * @brief Requests the autoposter thread to stop.
       * @note This function does NOT block and wait for the thread to stop. That's the destructor's job.
       */
      void stop() override;
    };

    /**
     * @brief An autoposter class that lets you manually retrieve the stats.
     */
    class TOPGG_EXPORT custom: public base {
      std::function<::topgg::stats(dpp::cluster*)> m_callback;

      inline ::topgg::stats get_stats(dpp::cluster* bot) override {
        return m_callback(bot);
      }

    public:
      /**
       * @brief Constructs the autoposter class.
       *
       * @param cluster A shared pointer to the bot's D++ cluster. This pointer will be used later in the autoposter thread.
       * @param token The Top.gg API token to be used.
       * @param delay The minimum delay between post requests. This delay mustn't be shorter than 15 minutes.
       * @param callback The callback function that returns the current stats.
       * @throw std::invalid_argument Throwns if the delay argument is shorter than 15 minutes.
       */
      template<class R, class P>
      inline custom(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay, std::function<::topgg::stats(dpp::cluster*)>&& callback)
        : base(cluster, token, delay), m_callback(callback) {}

      /**
       * @brief That's not how you initiate the class buddy :)
       */
      custom() = delete;

      /**
       * @brief This class can't be copied.
       */
      custom(const custom& other) = delete;

      /**
       * @brief This class can't be moved.
       */
      custom(custom&& other) = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      custom& operator=(const custom& other) & = delete;

      /**
       * @brief This class can't be assigned from other objects.
       */
      custom& operator=(custom&& other) & = delete;
    };
  }; // namespace autoposter
}; // namespace topgg