#include <topgg/dpp.h>
#include <stdexcept>

using topgg::autoposter::killable_waiter;
using topgg::autoposter::semaphore;
using topgg::autoposter::callback;
using topgg::autoposter::cached;
using topgg::autoposter::base;
using topgg::stats;

#if __cplusplus < 202002L
void semaphore::release() {
  std::lock_guard<std::mutex> lock{m_mutex};
  ++m_count;
  m_condition.notify_one();
}

void semaphore::acquire() {
  std::unique_lock<std::mutex> lock{m_mutex};
  
  while (!m_count) {
    m_condition.wait(lock);
  }
  
  m_count = 0;
}
#endif

void killable_waiter::kill() {
  std::lock_guard<std::mutex> lock{m_mutex};
  m_killed = true;
  m_condition.notify_one();
}

template<class R, class P>
bool killable_waiter::wait(const std::chrono::duration<R, P>& delay) {
  std::unique_lock<std::mutex> lock{m_mutex};
  
  m_condition.wait_for(lock, delay, [this]() -> bool { return this->m_killed; });
  
  return !m_killed;
}

template<class R, class P>
base::base(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay): m_cluster(std::shared_ptr{cluster}) {
  m_thread = std::thread([this, token, delay]() {
    std::shared_ptr<dpp::cluster> thread_cluster{this->m_cluster};

    while (this->m_waiter.wait(delay)) {
      this->thread_prepare();
      this->m_mutex.lock();

      const auto s = this->thread_get(thread_cluster.get());
      
      this->m_mutex.unlock();
      
      const auto s_json = s.to_json();
      const std::multimap<std::string, std::string> headers = {
        {"Authorization", "Bearer " + token};
        {"Connection", "close"},
        {"Content-Type", "application/json"},
        {"Content-Length", std::to_string(s_json.size())},
        {"User-Agent", "topgg (https://github.com/top-gg-community/cpp-sdk) D++"}
      };
      
      thread_cluster->request("https://top.gg/api/bots/stats", dpp::m_post, [](const auto& _) {}, s_json, "application/json", headers);
    }
  });
}

void base::request_stop() {
  m_waiter.kill();
}

base::~base() {
  request_stop();
  m_thread.join();
}

template<class R, class P>
cached::cached(std::shared_ptr<dpp::cluster>& cluster, const std::string& token, const std::chrono::duration<R, P>& delay): base(cluster, token, delay), m_semaphore(semaphore{0}) {
  auto this_cluster = m_cluster.get();
  
  if (delay < std::chrono::minutes(15)) {
    throw std::invalid_argument("Delay can't be shorter than 15 minutes.");
  }
  
  this_cluster->on_ready([this](const auto& event) {
    if (dpp::run_once<struct _>()) {
      this->m_mutex.lock();
      
      this->m_guilds = event.guilds;
      
      this->m_mutex.unlock();
      this->m_semaphore.release();
    }
  });
  
  this_cluster->on_guild_create([this](const auto& event) {
    if (!event.created->is_unavailable()) {
      this->m_mutex.lock();
      
      this->m_guilds.insert(event.created->id);
      
      this->m_mutex.unlock();
      this->m_semaphore.release();
    }
  });
  
  this_cluster->on_guild_delete([this](const auto& event) {
    this->m_mutex.lock();
    
    this->m_guilds.erase(event.deleted.id);
    
    this->m_mutex.unlock();
    this->m_semaphore.release();
  });
}

void cached::request_stop() {
  m_mutex.lock();
  
  base::request_stop();
  
  m_mutex.unlock();
}