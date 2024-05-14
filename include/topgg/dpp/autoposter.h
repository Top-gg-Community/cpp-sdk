#ifndef __TOPGG_CPP_SDK_DPP_AUTOPOSTER_H__
#define __TOPGG_CPP_SDK_DPP_AUTOPOSTER_H__

#include <dpp/dpp.h>
#include <topgg/dpp.h>

#include <chrono>
#include <memory>
#include <mutex>
#include <set>

#if __cplusplus < 202002L
#include <condition_variable>
#else
#include <semaphore>
#endif

namespace topgg {
  namespace autoposter {
#if __cplusplus < 202002L
    class cached;
    
    class semaphore {
      std::mutex m_mutex;
      std::condition_variable m_condition;
      size_t m_count;

      inline semaphore(const size_t count): m_count(count) {}
      
      void release();
      void acquire();
      
      friend class cached;
    public:
      semaphore() = delete;
    };
#else
    using semaphore = std::binary_semaphore;
#endif
    
    class base;

    // such a menacing name
    class killable_waiter {
      std::mutex m_mutex;
      std::condition_variable m_condition;
      size_t m_killed;

      inline killable_waiter(): m_killed(false) {}
      
      template<class R, class P>
      bool wait(const std::chrono::duration<R, P>& delay);
      void kill();
      
      friend class base;
    };
    
    class base {
      killable_waiter m_waiter;
      std::thread m_thread;
      
      virtual inline void thread_prepare() {}
      virtual ::topgg::stats thread_get() = 0;
      
    protected:
      template<class R, class P>
      base(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay);
      
      std::shared_ptr<dpp::cluster> m_cluster;
    public:
      virtual void request_stop();
      
      ~base();
    };
    
    class cached: public base {
      std::mutex m_mutex;
      semaphore m_semaphore;
      std::set<dpp::snowflake> m_guilds;
      
      inline void thread_prepare() override {
        m_semaphore.acquire();
      }
      
      inline ::topgg::stats thread_get() override {
        return ::topgg::stats{m_guilds.size()};
      }
      
    public:
      template<class R, class P>
      cached(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay);
    
      void request_stop();
    };
    
    class callback: public base {
      std::function<::topgg::stats()> m_callback;
      
      inline ::topgg::stats thread_get() override {
        return m_callback();
      }
      
    public:
      template<class R, class P>
      inline callback(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay, std::function<::topgg::stats()> callback): base(cluster, token, delay), m_callback(callback) {}
    };
  };
};
#endif