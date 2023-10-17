#ifndef TOPGG_ERRORS_HPP
#define TOPGG_ERRORS_HPP

#include <exception>

namespace topgg {

class TopggAPIError : public std::exception {
 public:
  TopggAPIError(const std::string& message);
  const char* what() const noexcept override;

 private:
  std::string m_message;
};

class TopggAuthError : public std::exception {
 public:
  TopggAuthError(const std::string& message);
  const char* what() const noexcept override;

 private:
  std::string m_message;
};

}  // namespace topgg

#endif  // TOPGG_ERRORS_HPP
