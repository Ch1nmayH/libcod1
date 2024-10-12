#include "shared.hpp"

#if COMPILE_CURL == 1
struct WebhookData
{
    std::string url;
    std::string title;
    int color;  // Overall color for the embed
    std::string description;  // New description field
    std::string player_name;
    std::string player_ip;
    std::string banned_by;
    std::string reason;
};

void async_webhook_message(std::shared_ptr<WebhookData> data)
{
    CURL *curl;
    CURLcode responseCode;
    struct curl_slist *headers = NULL;

    // Start building the payload dynamically based on non-empty fields
    std::string payload = R"({"embeds": [{)";

    // Add the title if it's not empty
    if (!data->title.empty())
        payload += R"("title": ")" + data->title + R"(",)";

    // Add the overall color
    payload += R"("color": )" + std::to_string(data->color) + R"(,";

    // Add the description if it's not empty
    if (!data->description.empty())
        payload += R"("description": ")" + data->description + R"(",)";

    // Start adding fields
    payload += R"("fields":[)";

    // Add each field only if it has a value
    if (!data->player_name.empty())
        payload += R"({"name":"Player Name","value":"👤 **)" + data->player_name + R"(**","inline":true},)";
    
    if (!data->player_ip.empty())
        payload += R"({"name":"Player IP","value":"`)" + data->player_ip + R"(`","inline":true},)";
    
    if (!data->banned_by.empty())
        payload += R"({"name":"Banned By","value":"🛑 **)" + data->banned_by + R"(**","inline":true},)";
    
    if (!data->reason.empty())
        payload += R"({"name":"Reason","value":"⚠️ **)" + data->reason + R"(**"},";

    // Remove the last comma if fields were added
    if (payload.back() == ',')
        payload.pop_back();

    // Close the JSON structures
    payload += R"(]}]})";

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
    char *description;  // New description field
    char *player_name;
    char *player_ip;
    char *banned_by;
    char *reason;
   

    // Fetch all parameters, allowing any of them to be empty
    if (!stackGetParams("sssssssi", &url, &title ,&color, &description, &player_name, &player_ip, &banned_by, &reason))
    {
        stackError("gsc_curl_webhookmessage() one or more arguments are undefined or have a wrong type");
        stackPushUndefined();
        return;
    }

    std::shared_ptr<WebhookData> data = std::make_shared<WebhookData>();
    data->url = url ? url : "";
    data->title = title ? title : "";
    data->color = color;
    data->description = description ? description : "";  // Set description
    data->player_name = player_name ? player_name : "";
    data->player_ip = player_ip ? player_ip : "";
    data->banned_by = banned_by ? banned_by : "";
    data->reason = reason ? reason : "";

    std::thread(async_webhook_message, data).detach();

    stackPushBool(qtrue);  // Indicate the async operation has started
}
#endif
