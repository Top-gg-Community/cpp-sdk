#include "topgg/errors.hpp"

using namespace topgg;

TopggAPIError::TopggAPIError(const std::string& message)
    : std::runtime_error(message) {}

RateLimitError::RateLimitError(const std::string& message, int retryAfter)
    : TopggAPIError(message), m_retryAfter(retryAfter) {}

int RateLimitError::getRetryAfter() const { return m_retryAfter; }
