# Top.gg SDK for C++

The official C++ SDK for the [Top.gg API](https://docs.top.gg).

## D++ examples

## Fetching a user from its Discord ID

```cpp
#include <topgg/dpp.h>
#include <dpp/dpp.h>
#include <iostream>

int main() {
  dpp::cluster bot("your bot token");
  topgg::client topgg(&bot, "your top.gg token");

  topgg.get_user(661200758510977084, [](const auto& result) {
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
  dpp::cluster bot("your bot token");
  topgg::client topgg(&bot, "your top.gg token");

  const size_t server_count = 12345;

  topgg.post_stats(topgg::stats{server_count}, []() {
    std::cout << "stats posted!" << std::endl;
  });

  return 0;
}
```

### Checking if a user has voted your bot

```rust,no_run
#include <topgg/dpp.h>
#include <dpp/dpp.h>
#include <iostream>

int main() {
  dpp::cluster bot("your bot token");
  topgg::client topgg(&bot, "your top.gg token");

  topgg.has_voted(661200758510977084, [](const auto& result) {
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