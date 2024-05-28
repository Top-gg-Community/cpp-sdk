#pragma once

#include <topgg/topgg.h>

#include <condition_variable>
#include <unordered_set>
#include <chrono>
#include <memory>
#include <mutex>

#define TOPGG_NO_COPYING(name)                      \
  name(const name& other) = delete;                 \
  name(name&& other) = delete;                      \
  name& operator=(const name& other) & = delete;    \
  name& operator=(name&& other) & = delete

namespace topgg {
  namespace autoposter {
    class cached;

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
      killable_semaphore() = delete;
      TOPGG_NO_COPYING(killable_semaphore);
    
      friend class cached;
    };

    class base;

    // such a menacing name
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
      TOPGG_NO_COPYING(killable_waiter);
    
      friend class base;
    };

    class TOPGG_EXPORT base: private ::topgg::base_client {
      killable_waiter m_waiter;
      std::thread m_thread;

      virtual inline bool before_fetch() {
        return true;
      }

      virtual inline void after_fetch() {}

      virtual ::topgg::stats get_stats(dpp::cluster* bot) = 0;
      static void thread_loop(base* self, dpp::cluster* thread_cluster);

    protected:
      std::shared_ptr<dpp::cluster> m_cluster;

      template<class R, class P>
      base(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay)
        : ::topgg::base_client(token), m_cluster(std::shared_ptr{cluster}) {
        if (delay < std::chrono::minutes(15)) {
          throw std::invalid_argument{"Delay can't be shorter than 15 minutes."};
        }

        // clang-format off
        m_thread = std::thread([this](const std::chrono::duration<R, P>& t_delay) {
          std::shared_ptr<dpp::cluster> thread_cluster{this->m_cluster};

          while (this->m_waiter.wait(t_delay) && this->before_fetch()) {
            base::thread_loop(this, thread_cluster.get());
          }
        }, std::cref(delay));
        // clang-format on
      }

    public:
      base() = delete;
      TOPGG_NO_COPYING(base);
      
      virtual void stop();

      ~base();
    };

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
      template<class R, class P>
      inline cached(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay)
        : base(cluster, token, delay), m_semaphore(killable_semaphore{0}) {
        start_listening();
      }

      cached() = delete;
      TOPGG_NO_COPYING(cached);

      void stop() override;
    };

    class TOPGG_EXPORT custom: public base {
      std::function<::topgg::stats(dpp::cluster*)> m_callback;

      inline ::topgg::stats get_stats(dpp::cluster* bot) override {
        return m_callback(bot);
      }

    public:
      template<class R, class P>
      inline custom(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay, std::function<::topgg::stats(dpp::cluster*)>&& callback)
        : base(cluster, token, delay), m_callback(callback) {}

      custom() = delete;
      TOPGG_NO_COPYING(custom);
    };
  }; // namespace autoposter
}; // namespace topgg

#undef TOPGG_NO_COPYING