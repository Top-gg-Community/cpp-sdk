<<<<<<< HEAD
# Top.gg SDK for C++

This is a simple C++ SDK for interacting with the [Top.gg API](https://docs.top.gg).

## Features

- Making authenticated requests to the Top.gg API using a bot token
- Parsing Top.gg API responses into convenient C++ objects
- Examples demonstrating how to use the SDK to implement a votebot and leaderboard

## Getting Started

1. Clone the repository:

```
git clone https://github.com/pneb/cpp-sdk.git
```

2. Add the `include` and `src` directories to your project's include path and library path, respectively.

3. Build the `topgg` library:

```
cd cpp-sdk
mkdir build
cd build
cmake ..
make
```

4. Build the examples:

```
make example-votebot
make example-leaderboard
```

5. Run the examples:

```
./example-votebot
./example-leaderboard
```

Note that the examples require you to have a Top.gg bot token set as an environment variable `TOPGG_BOT_TOKEN`.

## Usage

Include the `topgg/http/client.hpp` header and create an instance of `topgg::Api` with your Top.gg bot token:

```c++
#include <string>
#include "topgg/topgg.hpp"

// Put your bot token here
const std::string BOT_TOKEN = "YOUR_BOT_TOKEN";

int main()
{
    topgg::Api api(BOT_TOKEN);

    // Use the api object to make requests to the Top.gg API
    return 0;
}
```

Refer to the [Top.gg API documentation](https://docs.top.gg) for information on the available endpoints and request structures.

## Contributing

Pull requests are welcome! Please include unit tests for any added or modified functionality.
=======
# Top.gg C++ SDK

The official open-source C++ wrapper for the Top.gg API
>>>>>>> upstream/main
