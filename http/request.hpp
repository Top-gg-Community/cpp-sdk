#ifndef TOPGG_HTTP_REQUEST_HPP
#define TOPGG_HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include <map>

namespace topgg {
namespace http {

/**
 * HTTP request class for sending HTTP requests.
 *
 * Supports GET and POST request methods, with support for headers and query parameters.
 *
 * Throws a topgg::TopggAPIError when the request fails.
 */
class Request {
public:
    enum class Method {
        GET,
        POST
    };

    Request(Method method, const std::string& url);

    void setHeader(const std::string& key, const std::string& value);
    void setQueryParameter(const std::string& key, const std::string& value);
    void setBody(const std::string& body);

    std::pair<int, std::string> send();

private:
    Method m_method;
    std::string m_url;
    std::map<std::string, std::string> m_headers;
    std::map<std::string, std::string> m_queryParams;
    std::string m_body;
};

} // namespace http
} // namespace topgg

#endif // TOPGG_HTTP_REQUEST_HPP
