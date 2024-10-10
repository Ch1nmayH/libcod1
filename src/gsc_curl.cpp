#include "shared.hpp"

#if COMPILE_CURL == 1
struct WebhookData
{
    std::string url;
    std::string title;
    std::string message;
    int color;  // Added color as an integer field
};

void async_webhook_message(std::shared_ptr<WebhookData> data)
{
    CURL *curl;
    CURLcode responseCode;
    struct curl_slist *headers = NULL;

    // Construct a more detailed payload with title, message (description), and custom color
    std::string payload = R"({
        "embeds": [{
            "title": ")" + data->title + R"(",         // Insert the title
            "description": ")" + data->message + R"(",  // The message becomes the description
            "color": )" + std::to_string(data->color) + R"(    // Insert the custom color
        }]
    })";

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl)
    {
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_URL, data->url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

        responseCode = curl_easy_perform(curl);
        if(responseCode != CURLE_OK)
            Com_Printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(responseCode));

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    else
        Com_Printf("curl_easy_init() failed\n");

    curl_global_cleanup();
}

void gsc_curl_webhookmessage()
{
    char *url;
    char *title;
    char *message;
    int color;  // Added color argument

    // Fetch url, title, message, and color from parameters
    if (!stackGetParams("sssi", &url, &title, &message, &color))  // Expect four parameters, 'i' for int color
    {
        stackError("gsc_curl_webhookmessage() one or more arguments are undefined or have a wrong type");
        stackPushUndefined();
        return;
    }

    // Create WebhookData object with url, title, message, and color
    std::shared_ptr<WebhookData> data = std::make_shared<WebhookData>();
    data->url = url;
    data->title = title;
    data->message = message;
    data->color = color;  // Set the color here

    // Run the async webhook message in a detached thread
    std::thread(async_webhook_message, data).detach();

    stackPushBool(qtrue);  // Return true to indicate the async operation has started
}
#endif
