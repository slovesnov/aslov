/*
 * CheckNewVersion.cpp
 *
 *  Created on: 14.02.2018
 *      Author: alexey slovesnov
 */
#ifndef NOGTK

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "CheckNewVersion.h"
#include "aslov.h"

static gpointer check_new_version_thread(gpointer d) {
  ((CheckNewVersion *)(d))->routine();
  return NULL;
}

void CheckNewVersion::start(std::string version, GSourceFunc callback) {
  m_version = "v" + version;
  m_callback = callback;
  m_newVersionThread =
      g_thread_new("", check_new_version_thread, gpointer(this));
}

void CheckNewVersion::routine() {
  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    std::string url =
        "https://api.github.com/repos/slovesnov/" + getApplicationName() + "/releases/latest";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "User-Agent: MyCppApp");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(
        curl, CURLOPT_WRITEFUNCTION,
        +[](void *contents, size_t size, size_t nmemb, void *userp) -> size_t {
          auto *buffer = static_cast<std::string *>(userp);
          buffer->append(static_cast<char *>(contents), size * nmemb);
          return size * nmemb;
        });

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
      try {
        auto j = nlohmann::json::parse(readBuffer);
        if (j.is_object() && j.contains("tag_name")) {
          auto version = j["tag_name"].get<std::string>();
          if (m_version != version) {
            m_message = j.value("body", "");
            gdk_threads_add_idle(m_callback, NULL);
          }
        } else {
          // Tag 'tag_name' not found in response
        }
      } catch (const nlohmann::json::parse_error &e) {
        // Error parsing JSON
      }
    }
  }
  // Network Error
}

CheckNewVersion::~CheckNewVersion() { g_thread_join(m_newVersionThread); }
#endif /* NOGTK */
