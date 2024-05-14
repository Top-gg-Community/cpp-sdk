# Top.gg SDK for C++

The official C++ SDK for the [Top.gg API](https://docs.top.gg).

## D++ examples

### Fetching a user from its Discord ID

```cpp
#include <topgg/dpp.h>
#include <dpp/dpp.h>
#include <iostream>

int main() {
  dpp::cluster bot{"your bot token"};
  topgg::client topgg_client{&bot, "your top.gg token"};

  topgg_client.get_user(661200758510977084, [](const auto& result) {
    try {
      const auto user = result.get();
    
      std::cout << user.username() << std::endl;
    } catch (const std::exception& ext) {
      std::cout << "error: " << ext.what() << std::endl;
    }
  });

  return 0;
}
```

### Posting your bot's statistics

```cpp
#include <topgg/dpp.h>
#include <dpp/dpp.h>
#include <iostream>

int main() {
  dpp::cluster bot{"your bot token"};
  topgg::client topgg_client{&bot, "your top.gg token"};

  const size_t server_count = 12345;

  topgg_client.post_stats(topgg::stats{server_count}, []() {
    std::cout << "stats posted!" << std::endl;
  });

  return 0;
}
```

### Checking if a user has voted your bot

```cpp
#include <topgg/dpp.h>
#include <dpp/dpp.h>
#include <iostream>

int main() {
  dpp::cluster bot{"your bot token"};
  topgg::client topgg_client{&bot, "your top.gg token"};

  topgg_client.has_voted(661200758510977084, [](const auto& result) {
    try {
      if (result.get()) {
        std::cout << "checks out" << std::endl;
      }
    } catch (const std::exception& ext) {
      std::cout << "error: " << ext.what() << std::endl;
    }
  });

  return 0;
}
```

### Cached autoposting

```cpp
#include <topgg/dpp.h>
#include <dpp/dpp.h>
#include <iostream>

int main() {
  std::shared_ptr<dpp::cluster> bot{new dpp::cluster{"your bot token"}};
  
  topgg::autoposter::cached autoposter{bot, "your top.gg token", std::chrono::minutes(15)};

  // your bot's code...

  return 0;
}
```

### Stats-fed autoposting

```cpp
#include <topgg/dpp.h>
#include <dpp/dpp.h>
#include <iostream>

static topgg::stats fetch_stats(dpp::cluster* bot) {
  // fetch server count here...
}

int main() {
  std::shared_ptr<dpp::cluster> bot{new dpp::cluster{"your bot token"}};
  
  topgg::autoposter::callback autoposter{bot, "your top.gg token", std::chrono::minutes(15), fetch_stats};

  // your bot's code...

  return 0;
}
```