#ifndef TOPGG_HTTP_RESPONSE_HPP
#define TOPGG_HTTP_RESPONSE_HPP

#include <string>
#include <map>

namespace topgg {
namespace http {

/**
 * HTTP response class for representing HTTP response data.
 */
class Response {
public:
    Response(int statusCode, const std::string& body);

    int getStatusCode() const;
    const std::string& getBody() const;
    std::string getHeader(const std::string& key) const;

private:
    int m_statusCode;
    std::string m_body;
    std::map<std::string, std::string> m_headers;
};

} // namespace http
} // namespace topgg

#endif // TOPGG_HTTP_RESPONSE_HPP
