#include <topgg/webhooks/drogon.h>

#include <exception>

using namespace topgg;

std::optional<webhook::json> webhook::internal_drogon::parse(const ::drogon::HttpRequestPtr& request, const ::drogon::HttpResponsePtr& response) const noexcept {
  if (request->getMethod() != ::drogon::Post) {
    response->setStatusCode(::drogon::k405MethodNotAllowed);
    response->setContentTypeCode(::drogon::CT_TEXT_PLAIN);
    response->setBody("Method not allowed");

    return std::nullopt;
  }

  const auto authorization{request->getHeader("Authorization")};
  
  if (authorization != m_authorization) {
    response->setStatusCode(::drogon::k401Unauthorized);
    response->setContentTypeCode(::drogon::CT_TEXT_PLAIN);
    response->setBody("Unauthorized");

    return std::nullopt;
  }

  const std::string json_body{request->body()};

  Json::CharReaderBuilder builder{};
  const auto reader{builder.newCharReader()};

  std::string errors{};
  Json::Value root{};

  if (!reader->parse(json_body.c_str(), json_body.c_str() + json_body.size(), &root, &errors)) {
    response->setStatusCode(::drogon::k400BadRequest);
    response->setContentTypeCode(::drogon::CT_TEXT_PLAIN);
    response->setBody("Invalid webhook::json body");
    
    return std::nullopt;
  }

  response->setStatusCode(::drogon::k204NoContent);
  
  return root;
}