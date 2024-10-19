#include "shared.hpp"

#if COMPILE_CURL == 1
#include <sstream>
#include <string>

struct WebhookData
{
    std::string url;
    std::string title;
    int color;
    std::string player_name;
    std::string player_ip;
    std::string admin_name;
    std::string reason;
};

// Function to escape quotes and other special characters in a JSON string
std::string escape_json_string(const std::string &input)
{
    std::ostringstream ss;
    for (auto c : input)
    {
        switch (c)
        {
        case '\"': ss << "\\\""; break;  // Escape double quote
        case '\\': ss << "\\\\"; break;  // Escape backslash
        case '\n': ss << "\\n"; break;   // Escape newlines
        default: ss << c; break;
        }
    }
    return ss.str();
}

void async_webhook_message(std::shared_ptr<WebhookData> data)
{
    CURL *curl;
    CURLcode responseCode;
    struct curl_slist *headers = NULL;

    // Ensure that all user-supplied strings are escaped
    std::string escaped_title = escape_json_string(data->title);
    std::string payload = R"({
        "embeds": [{
            "title": ")" + escaped_title + R"(",
            "color": )" + std::to_string(data->color) + R"(, 
            "description": ")";

    // Add player details if available
    if (!data->player_name.empty())
        payload += "Player: " + escape_json_string(data->player_name) + R"(\n)";

    if (!data->player_ip.empty())
        payload += "IP: " + escape_json_string(data->player_ip) + R"(\n)";

    // Add admin details if available
    if (!data->admin_name.empty())
        payload += "Admin: " + escape_json_string(data->admin_name) + R"(\n)";

    // Add reason if available
    if (!data->reason.empty())
        payload += "Reason: " + escape_json_string(data->reason) + R"(\n)";

    // Remove the last newline and close the JSON description and object
    if (payload.back() == '\n')
        payload.pop_back();
    
    payload += R"("
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
        if (responseCode != CURLE_OK)
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
    int color;
    char *player_name;
    char *player_ip;
    char *admin_name;
    char *reason;

    // Fetch all parameters including the new ones, but check if they are set
    if (!stackGetParams("ssissss", &url, &title, &color, &player_name, &player_ip, &admin_name, &reason))
    {
        stackError("gsc_curl_webhookmessage() one or more arguments are undefined or have a wrong type");
        stackPushUndefined();
        return;
    }

    std::shared_ptr<WebhookData> data = std::make_shared<WebhookData>();
    data->url = url;
    data->title = title;
    data->color = color;
    
    // Assign fields only if they are not nullptr
    data->player_name = player_name ? player_name : "";
    data->player_ip = player_ip ? player_ip : "";
    data->admin_name = admin_name ? admin_name : "";
    data->reason = reason ? reason : "";

    std::thread(async_webhook_message, data).detach();

    stackPushBool(qtrue);  // Return true to indicate the async operation has started
}
#endif
