#include <topgg/topgg.h>
#include <stdexcept>

using topgg::autoposter::base;
using topgg::autoposter::cached;
using topgg::autoposter::killable_semaphore;
using topgg::autoposter::killable_waiter;
using topgg::stats;

void killable_semaphore::release() {
  std::lock_guard<std::mutex> lock{m_mutex};

  ++m_count;
  m_condition.notify_all();
}

bool killable_semaphore::acquire() {
  std::unique_lock<std::mutex> lock{m_mutex};

  while (m_count == 0 && !m_killed) {
    m_condition.wait(lock);
  }

  m_count = 0;
  return !m_killed;
}

void killable_semaphore::kill() {
  std::lock_guard<std::mutex> lock{m_mutex};

  m_killed = true;
  m_condition.notify_all();
}

void killable_waiter::kill() {
  std::lock_guard<std::mutex> lock{m_mutex};

  m_killed = true;
  m_condition.notify_all();
}

void base::thread_loop(dpp::cluster* thread_cluster) {
  const auto s = get_stats(thread_cluster);

  after_fetch();

  const auto s_json = s.to_json();
  std::multimap<std::string, std::string> headers{m_headers};

  headers.insert(std::pair("Content-Length", std::to_string(s_json.size())));

  thread_cluster->request("https://top.gg/api/bots/stats", dpp::m_post, [](TOPGG_UNUSED const auto& _) {}, s_json, "application/json", headers);
}

void base::stop() {
  m_waiter.kill();
}

base::~base() {
  stop();
  m_thread.join();
}

void cached::start_listening() {
  auto this_cluster = m_cluster.get();

  this_cluster->on_ready([this](const auto& event) {
    if (dpp::run_once<struct _>()) {
      this->m_mutex.lock();

      for (const auto& guild: event.guilds) {
        this->m_guilds.insert(guild);
      }

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

bool cached::before_fetch() {
  if (m_semaphore.acquire()) {
    m_mutex.lock();

    return true;
  }

  return false;
}

void cached::stop() {
  base::stop();
  m_semaphore.kill();
}