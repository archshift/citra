// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <chrono>
#include <curl/curl.h>

#include "common/string_util.h"

#include "core/hle/service/http/http.h"
#include "core/hle/service/http/http_c.h"
#include "core/hle/service/service.h"

namespace Service {
namespace HTTP {

std::unordered_map<ContextHandle, std::unique_ptr<HttpContext>> context_map;
ContextHandle next_handle;

static int BufWriter(u8 *data, size_t size, size_t nmemb, std::vector<u8>* out_buffer) {
    if (out_buffer == nullptr)
        return -1;
    out_buffer->insert(out_buffer->end(), data, data + size * nmemb);
    return static_cast<int>(size * nmemb);
}

HttpContext::HttpContext() {
    state = RequestState::NOT_STARTED;
    cancel_request = false;
    req_type = RequestType::NONE;
    request_hdrs = nullptr;
    response_code = 0;
    content_length = 0.0;
    downloaded_size = 0.0;
}

HttpContext::~HttpContext() {
    curl_slist_free_all(request_hdrs);
}

static CURLcode SetConnectionType(CURL* connection, RequestType type) {
    switch (type) {
    case RequestType::GET:
        return curl_easy_setopt(connection, CURLOPT_HTTPGET, 1);
    case RequestType::POST:
    case RequestType::POST_ALT:
        return curl_easy_setopt(connection, CURLOPT_POST, 1);
    case RequestType::PUT:
    case RequestType::PUT_ALT:
        return curl_easy_setopt(connection, CURLOPT_UPLOAD, 1);
    case RequestType::DELETE:
        break; // TODO
    case RequestType::HEAD:
        return curl_easy_setopt(connection, CURLOPT_NOBODY, 1);
    }
}

void MakeRequest(HttpContext* context) {
    CURLM* manager = curl_multi_init();
    CURL* connection = curl_easy_init();

    CURLcode res;
    CURLMcode mres;

    {
        std::lock_guard<std::mutex> lock(context->mutex);

        res = curl_easy_setopt(connection, CURLOPT_URL, context->url.c_str());
        res = SetConnectionType(connection, context->req_type);
        res = curl_easy_setopt(connection, CURLOPT_HTTPHEADER, context->request_hdrs);
    }

    std::vector<u8> response_hdrs, response_data;
    res = curl_easy_setopt(connection, CURLOPT_HEADERFUNCTION, BufWriter);
    res = curl_easy_setopt(connection, CURLOPT_WRITEFUNCTION, BufWriter);
    res = curl_easy_setopt(connection, CURLOPT_HEADERDATA, &response_hdrs);
    res = curl_easy_setopt(connection, CURLOPT_WRITEDATA, &response_data);

    curl_multi_add_handle(manager, connection);

    int still_running = 0;
    int repeats = 0;
    curl_multi_perform(manager, &still_running);

    do {
        // Number of file descriptors. If this is zero, nothing happened to
        // the active connection.
        int numfds;

        // Set the timeout to 1sec.
        if (curl_multi_wait(manager, NULL, 0, 1000, &numfds) != CURLM_OK)
            break;

        if (numfds == 0) {
            // We allow one repeat try--after which, we start to sleep the
            // thread until the timeout runs out.
            if (repeats++ > 1)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            // While the file descriptor is responding, there's no need to check for repeats.
            repeats = 0;
        }

        {
            std::lock_guard<std::mutex> lock(context->mutex);
            if (context->cancel_request)
                break;
        }

        curl_multi_perform(manager, &still_running);
    } while (still_running != 0);

    {
        std::lock_guard<std::mutex> lock(context->mutex);

        context->response_hdrs = std::move(response_hdrs);
        context->response_data = std::move(response_data);
        res = curl_easy_getinfo(connection, CURLINFO_RESPONSE_CODE, &context->response_code);
        res = curl_easy_getinfo(connection, CURLINFO_SIZE_DOWNLOAD, &context->downloaded_size);
        res = curl_easy_getinfo(connection, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &context->content_length);

        context->state = RequestState::READY;
    }

    curl_multi_remove_handle(manager, connection);
    curl_easy_cleanup(connection);
    curl_multi_cleanup(manager);
}

void AddRequestHeader(const std::string& name, const std::string& value, curl_slist** hdr_list) {
    // TODO: header value is empty
    std::string header = Common::StringFromFormat("%s: %s", name.c_str(), value.c_str());
    *hdr_list = curl_slist_append(*hdr_list, header.c_str());
}

void Init() {
    AddService(new HTTP_C_Interface);

    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void ClearInstance() {
    for (auto& pair : context_map) {
        pair.second->cancel_request = true;
    }

    for (auto& pair : context_map) {
        if (pair.second->req_thread != nullptr)
            pair.second->req_thread->join();
    }

    context_map.clear();
    next_handle = 0;
}

void Shutdown() {
    ClearInstance();
    curl_global_cleanup();
}

} // namespace HTTP
} // namespace Service
