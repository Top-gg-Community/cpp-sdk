#include "topgg/request.hpp"

#include <curl/curl.h>

using namespace topgg::http;

Request::Request(Request::Method method, const std::string& url)
    : m_method(method), m_url(url), m_headers() {}

void Request::setHeader(const std::string& name, const std::string& value) {
  m_headers[name] = value;
}

std::pair<int, std::string> Request::send() const {
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(),
                                                           curl_easy_cleanup);
  if (!curl) {
    throw std::runtime_error("Failed to initialize libcurl");
  }

  // Set URL and method
  curl_easy_setopt(curl.get(), CURLOPT_URL, m_url.c_str());
  switch (m_method) {
    case Method::GET:
      curl_easy_setopt(curl.get(), CURLOPT_HTTPGET, 1L);
      break;
    case Method::POST:
      curl_easy_setopt(curl.get(), CURLOPT_POST, 1L);
      break;
  }

  // Set request headers
  struct curl_slist* headerList = nullptr;
  for (const auto& header_it : m_headers) {
    const std::string& name = header_it.first;
    const std::string& value = header_it.second;
    const std::string header = name + ": " + value;
    headerList = curl_slist_append(headerList, header.c_str());
  }
  curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headerList);

  // Set response buffer and callback
  std::string responseBody;
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &responseBody);
  curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION,
                   [](char* ptr, size_t size, size_t nmemb, void* userdata) {
                     std::string& response =
                         *reinterpret_cast<std::string*>(userdata);
                     const size_t dataSize = size * nmemb;
                     response.append(ptr, dataSize);
                     return dataSize;
                   });

  // Perform request
  CURLcode res = curl_easy_perform(curl.get());

  // Get HTTP response code
  long httpCode = 0;
  curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &httpCode);

  // Cleanup
  curl_slist_free_all(headerList);

  if (res != CURLE_OK) {
    throw std::runtime_error("Failed to perform HTTP request: " +
                             std::string(curl_easy_strerror(res)));
  }

  return {httpCode, responseBody};
}
