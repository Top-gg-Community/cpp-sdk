#include <topgg/webhooks/cpp-httplib.h>

#include <exception>

using namespace topgg::webhook;

std::optional<json> internal_cpp_httplib::parse(const httplib::Request& request, httplib::Response& response) const noexcept {
  if (request.method != "POST") {
    response.status = 405;
    response.set_content("Method not allowed", "text/plain");

    return std::nullopt;
  }

  const auto authorization{request.headers.find("Authorization")};

  if (authorization == request.headers.end() || authorization->second != m_authorization) {
    response.status = 401;
    response.set_content("Unauthorized", "text/plain");

    return std::nullopt;
  }

  try {
    const auto json_body{json::parse(request.body)};
    response.status = 204;

    return json_body;
  } catch (TOPGG_UNUSED const std::exception&) {
    response.status = 400;
    response.set_content("Invalid JSON body", "text/plain");

    return std::nullopt;
  }
}