# Top.gg SDK for C++

The official C++ SDK for the [Top.gg API](https://docs.top.gg).

## Building from source

Linux (Debian-like):

```sh
# install D++
wget -O dpp.deb https://dl.dpp.dev/latest
dpkg -i dpp.deb

# build topgg
cmake -B build .
cmake --build build --config Release
```

Linux (CentOS-like):

```sh
# install D++
yum install wget
wget -O dpp.rpm https://dl.dpp.dev/latest/linux-x64/rpm
yum localinstall dpp.rpm

# build topgg
cmake -B build .
cmake --build build --config Release
```

macOS:

```sh
# install D++
brew install libdpp
brew link libdpp

# build topgg
cmake -B build .
cmake --build build --config Release
```

Windows:

```bat
cmake -B build .
cmake --build build --config Release
```

## Examples

### Fetching a bot from its Discord ID

```cpp
dpp::cluster bot{"your bot token"};
topgg::client topgg_client{bot, "your top.gg token"};

// using C++17 callbacks
topgg_client.get_bot(264811613708746752, [](const auto& result) {
  try {
    const auto topgg_bot = result.get();
  
    std::cout << topgg_bot.username << std::endl;
  } catch (const std::exception& ext) {
    std::cout << "error: " << ext.what() << std::endl;
  }
});

// using C++20 coroutines
try {
  const auto result = co_await topgg_client.co_get_bot(264811613708746752);
  const auto topgg_bot = result.get();
  
  std::cout << topgg_bot.username << std::endl;
} catch (const std::exception& ext) {
  std::cout << "error: " << ext.what() << std::endl;
}
```

### Fetching a user from its Discord ID

```cpp
dpp::cluster bot{"your bot token"};
topgg::client topgg_client{bot, "your top.gg token"};

// using C++17 callbacks
topgg_client.get_user(264811613708746752, [](const auto& result) {
  try {
    const auto user = result.get();
  
    std::cout << user.username << std::endl;
  } catch (const std::exception& ext) {
    std::cout << "error: " << ext.what() << std::endl;
  }
});

// using C++20 coroutines
try {
  const auto result = co_await topgg_client.co_get_user(661200758510977084);
  const auto user = result.get();
  
  std::cout << user.username << std::endl;
} catch (const std::exception& ext) {
  std::cout << "error: " << ext.what() << std::endl;
}
```

### Posting your bot's statistics

```cpp
dpp::cluster bot{"your bot token"};
topgg::client topgg_client{bot, "your top.gg token"};

// using C++17 callbacks
topgg_client.post_stats([](const auto success) {
  if (success) {
    std::cout << "stats posted!" << std::endl;
  }
});

// using C++20 coroutines
const auto success = co_await topgg_client.post_stats();

if (success) {
  std::cout << "stats posted!" << std::endl;
}
```

### Checking if a user has voted your bot

```cpp
dpp::cluster bot{"your bot token"};
topgg::client topgg_client{bot, "your top.gg token"};

// using C++17 callbacks
topgg_client.has_voted(661200758510977084, [](const auto& result) {
  try {
    if (result.get()) {
      std::cout << "checks out" << std::endl;
    }
  } catch (const std::exception& ext) {
    std::cout << "error: " << ext.what() << std::endl;
  }
});

// using C++20 coroutines
try {
  const auto result = co_await topgg_client.has_voted(661200758510977084);

  if (result.get()) {
    std::cout << "checks out" << std::endl;
  }
} catch (const std::exception& ext) {
  std::cout << "error: " << ext.what() << std::endl;
}
```

### Cached autoposting

If you want for the SDK to automatically retrieve the server count itself by listening to Discord's gateway events, use `topgg::autoposter::cached`!

```cpp
dpp::cluster bot{"your bot token"};
topgg::client topgg_client{bot, "your top.gg token"};

topgg_client.start_autoposter();
```

### Stats-fed autoposting

If you want to use your own function (e.g: retrieve the server count on your own by making an SQL query), then you can use `topgg::autoposter::custom`!

```cpp
dpp::cluster bot{"your bot token"};
topgg::client topgg_client{bot, "your top.gg token"};

topgg_client.start_autoposter([](dpp::cluster& bot_inner) {
  return topgg::stats{...};
});
```