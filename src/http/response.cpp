#include "topgg/response.hpp"

using namespace topgg::http;

Response::Response(int statusCode, const std::string& body)
    : m_statusCode(statusCode), m_body(body) {}

int Response::getStatusCode() const { return m_statusCode; }

const std::string& Response::getBody() const { return m_body; }
