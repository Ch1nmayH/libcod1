#include <thread>
#include <mutex>
#include <map>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "gsc_getcountry.hpp"  // Header for function declaration

// A map to cache IP to country lookups
std::map<std::string, std::string> countryMap;
std::mutex countryMapMutex;  // Mutex for thread safety

// Global cvar for the API key
extern cvar_t *sv_apikey;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Asynchronous task to fetch country information
void fetchCountryAsync(std::string ipStr, int id)
{
    CURL *curl = curl_easy_init();
    CURLcode res;
    if (curl)
    {
        // Fetch the API key from the cvar
        std::string apiKey = sv_apikey->string;  // Dynamically use the API key
        if (apiKey.empty())
        {
            printf("No API key set (sv_apikey is empty).\n");
            return;
        }

        std::string provider = "https://api.iplocation.net/?key=" + apiKey + "&ip="; // Geolocation API
        std::string url = provider + ipStr;
        std::string response_string;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            try
            {
                // Parse the JSON response
                nlohmann::json j = nlohmann::json::parse(response_string);

                if (j.contains("country_name") && j["country_name"].is_string())
                {
                    std::string country = j["country_name"];

                    // Cache the result in a thread-safe way
                    {
                        std::lock_guard<std::mutex> lock(countryMapMutex);
                        countryMap[ipStr] = country;
                    }

                    // Notify that the country info was fetched (replace this with actual game code)
                    printf("Country for player %d: %s\n", id, country.c_str());
                }
            }
            catch (const std::exception& e)
            {
                printf("Error parsing JSON: %s\n", e.what());
            }
        }
        else
        {
            printf("CURL error: %s\n", curl_easy_strerror(res));
        }
    }
}

// Main function called by game to fetch country asynchronously
void gsc_player_getcountry(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getcountry() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    char ip[16];

    snprintf(ip, sizeof(ip), "%d.%d.%d.%d",
        client->netchan.remoteAddress.ip[0],
        client->netchan.remoteAddress.ip[1],
        client->netchan.remoteAddress.ip[2],
        client->netchan.remoteAddress.ip[3]);

    std::string ipStr(ip);

    // Check if IP is already cached
    {
        std::lock_guard<std::mutex> lock(countryMapMutex);
        if (countryMap.find(ipStr) != countryMap.end())
        {
            stackPushString(countryMap[ipStr].c_str());
            return;
        }
    }

    // Start a new thread to fetch country information asynchronously
    std::thread(fetchCountryAsync, ipStr, id).detach();

    // Push a placeholder (undefined) while waiting for async result
    stackPushUndefined();
}
