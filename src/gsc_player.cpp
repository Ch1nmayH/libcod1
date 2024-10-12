#include "shared.hpp"
#include <curl/curl.h>
#include <future>
#include <map>
#include <string>
#include <maxminddb.h>
#include <cstdio>
#include <iostream>
#include "vendor/json.hpp"


using json = nlohmann::json;


extern cvar_t *player_sprintTime;

void gsc_player_setvelocity(scr_entref_t ref)
{
    int id = ref.entnum;
    vec3_t velocity;

    if (!stackGetParams("v", &velocity))
    {
        stackError("gsc_player_setvelocity() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_setvelocity() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    playerState_t *ps = SV_GameClientNum(id);
    VectorCopy(velocity, ps->velocity);

    stackPushBool(qtrue);
}

void gsc_player_getvelocity(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getvelocity() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    playerState_t *ps = SV_GameClientNum(id);

    stackPushVector(ps->velocity);
}

void gsc_player_getuserinfo(scr_entref_t ref)
{
    int id = ref.entnum;
    char *key;

    if (!stackGetParams("s", &key))
    {
        stackError("gsc_player_getuserinfo() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getuserinfo() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    char *val = Info_ValueForKey(client->userinfo, key);

    if (strlen(val))
        stackPushString(val);
    else
        stackPushString("");
}

void gsc_player_setuserinfo(scr_entref_t ref)
{
    int id = ref.entnum;
    char *key, *value;

    if (!stackGetParams("ss", &key, &value))
    {
        stackError("gsc_player_setuserinfo() one or more arguments is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_setuserinfo() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    Info_SetValueForKey(client->userinfo, key, value);

    stackPushBool(qtrue);
}

void gsc_player_button_ads(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_ads() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.buttons & KEY_MASK_ADS_MODE ? qtrue : qfalse);
}

void gsc_player_button_left(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_left() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.rightmove == KEY_MASK_MOVELEFT ? qtrue : qfalse);
}

void gsc_player_button_right(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_right() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.rightmove == KEY_MASK_MOVERIGHT ? qtrue : qfalse);
}

void gsc_player_button_forward(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_forward() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.forwardmove == KEY_MASK_FORWARD ? qtrue : qfalse);
}

void gsc_player_button_back(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_back() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.forwardmove == KEY_MASK_BACK ? qtrue : qfalse);
}

void gsc_player_button_jump(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_jump() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.upmove == KEY_MASK_JUMP ? qtrue : qfalse);
}

void gsc_player_button_leanleft(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_leanleft() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    
    stackPushBool(client->lastUsercmd.wbuttons & KEY_MASK_LEANLEFT ? qtrue : qfalse);
}

void gsc_player_button_leanright(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_leanright() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.wbuttons & KEY_MASK_LEANRIGHT ? qtrue : qfalse);
}

void gsc_player_button_reload(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_button_reload() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.wbuttons & KEY_MASK_RELOAD ? qtrue : qfalse);
}

void gsc_player_getangles(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getangles() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }
    
    playerState_t *ps = SV_GameClientNum(id);

    stackPushVector(ps->viewangles);
}

void gsc_player_getstance(scr_entref_t ref)
{
    int id = ref.entnum;
    
    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getstance() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }
    
    playerState_t *ps = SV_GameClientNum(id);
    if(ps->eFlags & EF_CROUCHING)
        stackPushString("crouch");
    else if(ps->eFlags & EF_PRONE)
        stackPushString("prone");
    else
        stackPushString("stand");
}

void gsc_player_getip(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getip() entity %i is not a player", id);
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

    stackPushString(ip);
}

void gsc_player_getping(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getping() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    stackPushInt(client->ping);
}

void gsc_player_processclientcommand(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_processclientcommand() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    ClientCommand(id);

    stackPushBool(qtrue);
}

void gsc_player_dropclient(scr_entref_t ref)
{
    int id = ref.entnum;
    char *reason;

    if (Scr_GetNumParam() > 0 && !stackGetParams("s", &reason))
    {
        stackError("gsc_player_dropclient() argument has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_dropclient() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    if(Scr_GetNumParam() > 0)
        SV_DropClient(client, reason);
    else
        SV_DropClient(client, NULL);

    stackPushBool(qtrue);
}

void gsc_player_setspeed(scr_entref_t ref)
{
    int id = ref.entnum;
    int speed;

    if (!stackGetParams("i", &speed))
    {
        stackError("gsc_player_setspeed() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_setspeed() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    if (speed < 0)
    {
        stackError("gsc_player_setspeed() param must be equal or above zero");
        stackPushUndefined();
        return;
    }

    customPlayerState[id].speed = speed;

    stackPushBool(qtrue);
}

void gsc_player_getfps(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getfps() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    stackPushInt(customPlayerState[id].fps);
}

void gsc_player_isonladder(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_isonladder() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    playerState_t *ps = SV_GameClientNum(id);

    stackPushBool(ps->pm_flags & PMF_LADDER ? qtrue : qfalse);
}

void gsc_player_noclip(scr_entref_t ref)
{
    int id = ref.entnum;
    char *noclip;

    if (!stackGetParams("s", &noclip))
    {
        stackError("gsc_player_noclip() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_noclip() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    gentity_t *entity = &g_entities[id];

    if (!Q_stricmp(noclip, "on") || atoi(noclip))
    {
        entity->client->noclip = qtrue;
    }
    else if (!Q_stricmp(noclip, "off") || !Q_stricmp(noclip, "0"))
    {
        entity->client->noclip = qfalse;
    }
    else
    {
        entity->client->noclip = !entity->client->noclip;
    }

    stackPushBool(qtrue);
}

void gsc_player_ufo(scr_entref_t ref)
{
    int id = ref.entnum;
    char *ufo;

    if (!stackGetParams("s", &ufo))
    {
        stackError("gsc_player_ufo() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_ufo() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    gentity_t *entity = &g_entities[id];

    if (!Q_stricmp(ufo, "on") || atoi(ufo))
    {
        entity->client->ufo = qtrue;
    }
    else if (!Q_stricmp(ufo, "off") || !Q_stricmp(ufo, "0"))
    {
        entity->client->ufo = qfalse;
    }
    else
    {
        entity->client->ufo = !entity->client->ufo;
    }

    stackPushBool(qtrue);
}

void gsc_player_connectionlesspackettoclient(scr_entref_t ref)
{
    int id = ref.entnum;
    char *cmd;

    if (!stackGetParams("s", &cmd))
    {
        stackError("gsc_player_connectionlesspackettoclient() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_connectionlesspackettoclient() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    NET_OutOfBandPrint(NS_SERVER, client->netchan.remoteAddress, cmd);

    stackPushBool(qtrue);
}

void gsc_player_setjumpheight(scr_entref_t ref)
{
    int id = ref.entnum;
    float jump_height;

    if (!stackGetParams("f", &jump_height))
    {
        stackError("gsc_player_setjumpheight() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_setjumpheight() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    if(jump_height < 0)
        customPlayerState[id].overrideJumpHeight = false;
    else
    {
        customPlayerState[id].overrideJumpHeight = true;
        customPlayerState[id].jumpHeight = jump_height;
    }

    stackPushBool(qtrue);
}

void gsc_player_setairjumps(scr_entref_t ref)
{
    int id = ref.entnum;
    int airJumps;

    if (!stackGetParams("i", &airJumps))
    {
        stackError("gsc_player_setairjumps() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_setairjumps() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    customPlayerState[id].airJumpsAvailable = airJumps;

    stackPushBool(qtrue);
}

void gsc_player_getairjumps(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getairjumps() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    stackPushInt(customPlayerState[id].airJumpsAvailable);
}

void gsc_player_disableitemautopickup(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_disableitempickup() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    int old_setting = !customPlayerState[id].noAutoPickup;
    customPlayerState[id].noAutoPickup = true;

    stackPushInt(old_setting);
}

void gsc_player_enableitemautopickup(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_enableitempickup() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    int old_setting = !customPlayerState[id].noAutoPickup;
    customPlayerState[id].noAutoPickup = false;

    stackPushInt(old_setting);
}

void gsc_player_getsprintremaining(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getsprintremaining() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }
    
    float sprint_time = player_sprintTime->value * 1000.0;
    int timeUsed = customPlayerState[id].sprintTimer;
    int timeRemaining = sprint_time - timeUsed;
    
    stackPushInt(timeRemaining);
}

void gsc_player_playscriptanimation(scr_entref_t ref)
{
    int id = ref.entnum;
    int scriptAnimEventType;
    int isContinue;
    int force;

    if (!stackGetParams("iii", &scriptAnimEventType, &isContinue, &force))
    {
        stackError("gsc_player_playscriptanimation() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_playscriptanimation() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    if (scriptAnimEventType < 0 || scriptAnimEventType >= NUM_ANIM_EVENTTYPES)
    {
        stackError("gsc_player_playscriptanimation() argument is not a valid scriptAnimEventType");
        stackPushUndefined();
        return;
    }
    
    gentity_t *entity = &g_entities[id];

    stackPushInt(BG_AnimScriptEvent(&entity->client->ps, (scriptAnimEventTypes_t)scriptAnimEventType, isContinue, force));
}

void gsc_player_isbot(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_isbot() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->bIsTestClient);
}
void gsc_player_sethiddenfromscoreboard(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_sethiddenfromscoreboard() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    bool hidden = Scr_GetInt(0);
    customPlayerState[id].hiddenFromScoreboard = hidden;

    stackPushBool(true);
}

void gsc_player_ishiddenfromscoreboard(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_ishiddenfromscoreboard() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    stackPushBool(customPlayerState[id].hiddenFromScoreboard);
}


void gsc_player_getcountryusingmmdb(scr_entref_t ref)
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

    // Convert player's IP to string format
    snprintf(ip, sizeof(ip), "%d.%d.%d.%d",
        client->netchan.remoteAddress.ip[0],
        client->netchan.remoteAddress.ip[1],
        client->netchan.remoteAddress.ip[2],
        client->netchan.remoteAddress.ip[3]);

    // Get the country from the player's IP
    const char* country = lookup_country_by_ip(ip);

    // Push the country name onto the stack to return to the GSC script
    stackPushString(country);
}



const char* lookup_country_by_ip(const char* ip)
{
    static char country[64] = "Unknown";  // Default country if no match found
    MMDB_s mmdb;
    
    // Open the GeoLite2 database - update the path to the location of your GeoLite2-Country.mmdb file
    int status = MMDB_open("./GeoLite2-Country.mmdb", MMDB_MODE_MMAP, &mmdb);
    
    if (status != MMDB_SUCCESS) {
        printf("Error opening GeoLite2 database: %s\n", MMDB_strerror(status));
        return country;  // Return "Unknown" if there's an issue opening the database
    }

    // Perform the IP lookup in the database
    MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb, ip, NULL, NULL);
    
    if (result.found_entry) {
        MMDB_entry_data_s entry_data;

        // Get the country name in English
        int status = MMDB_get_value(&result.entry, &entry_data, "country", "names", "en", NULL);
        
        if (status == MMDB_SUCCESS && entry_data.has_data) {
            snprintf(country, sizeof(country), "%.*s", entry_data.data_size, entry_data.utf8_string);  // Copy country name
        }
    } else {
        printf("No entry found for IP: %s\n", ip);
    }

    // Close the GeoLite2 database
    MMDB_close(&mmdb);

    return country;  // Return the country name or "Unknown"
}


// A map to cache IP to country lookups
std::map<std::string, std::string> countryMap;


size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


// Helper function for making the API request
std::string fetchCountryFromAPI(const std::string& ipStr)
{
    CURL *curl = curl_easy_init();
    CURLcode res;
    std::string response_string;

    if (curl)
    {
        std::string provider = "https://api.iplocation.net/?&ip="; // Geolocation API
        std::string url = provider + ipStr;

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
                    return j["country_name"];
                }
            }
            catch (const std::exception& e)
            {
                stackError("gsc_player_getcountry() failed to parse JSON: %s", e.what());
            }
        }
        else
        {
            stackError("gsc_player_getcountry() curl_easy_perform() failed: %s", curl_easy_strerror(res));
        }
    }
    else
    {
        stackError("gsc_player_getcountry() failed to initialize curl");
    }
    return "";  // In case of failure, return empty string
}

// The asynchronous function
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

    // Check if IP is already cached in countryMap
    if (countryMap.find(ipStr) != countryMap.end())
    {
        stackPushString(countryMap[ipStr].c_str());
        return;
    }

    // Asynchronous task to fetch country and push result
    std::future<void> result = std::async(std::launch::async, [ipStr]() {
        std::string country = fetchCountryFromAPI(ipStr);

        if (!country.empty())
        {
            countryMap[ipStr] = country;  // Cache the country

            // Push the result to the game engine stack (Note: may need thread-safe access)
            stackPushString(country.c_str());
        }
        else
        {
            // In case of failure, push undefined
            stackPushUndefined();
        }
    });

    // Allow the game to continue running while the async call processes.
}

// Function to fetch country from MaxMind GeoIP database (this will be called asynchronously)
std::string fetchCountryFromMMDB(const std::string& ipStr)
{
    // Path to the GeoLite2 database
    const char *dbPath = "./GeoLite2-Country.mmdb";
    MMDB_s mmdb;

    int status = MMDB_open(dbPath, MMDB_MODE_MMAP, &mmdb);
    if (status != MMDB_SUCCESS)
    {
        std::cerr << "Can't open MaxMind database: " << MMDB_strerror(status) << std::endl;
        return "";
    }

    int gai_error, mmdb_error;
    MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb, ipStr.c_str(), &gai_error, &mmdb_error);

    if (gai_error != 0)
    {
        std::cerr << "Error from getaddrinfo for " << ipStr << ": " << gai_strerror(gai_error) << std::endl;
        MMDB_close(&mmdb);
        return "";
    }

    if (mmdb_error != MMDB_SUCCESS)
    {
        std::cerr << "Error from MaxMind DB lookup: " << MMDB_strerror(mmdb_error) << std::endl;
        MMDB_close(&mmdb);
        return "";
    }

    if (result.found_entry)
    {
        MMDB_entry_data_s entry_data;

        // Lookup the country name in the database result
        int status = MMDB_get_value(&result.entry, &entry_data, "country", "names", "en", NULL);

        if (status == MMDB_SUCCESS && entry_data.has_data && entry_data.type == MMDB_DATA_TYPE_UTF8_STRING)
        {
            std::string country(entry_data.utf8_string, entry_data.data_size);
            MMDB_close(&mmdb);
            return country;
        }
        else
        {
            std::cerr << "No country data for IP " << ipStr << std::endl;
        }
    }

    MMDB_close(&mmdb);
    return "";  // If the lookup fails, return an empty string
}

// Main function to get player's country from MMDB asynchronously
void gsc_player_getcountryfrommmdb(scr_entref_t ref)
{
    int id = ref.entnum;

    if (id >= MAX_CLIENTS)
    {
        stackError("gsc_player_getcountryfrommmdb() entity %i is not a player", id);
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
    if (countryMap.find(ipStr) != countryMap.end())
    {
        stackPushString(countryMap[ipStr].c_str());
        return;
    }

    // Asynchronously fetch country from MaxMind DB
    auto fetchCountryFuture = std::async(std::launch::async, fetchCountryFromMMDB, ipStr);

    // Detach the async task and continue execution (non-blocking)
    fetchCountryFuture.then([&](std::future<std::string> countryFuture) {
        std::string country = countryFuture.get();

        if (!country.empty())
        {
            countryMap[ipStr] = country;  // Cache the country for future lookups
            stackPushString(country.c_str());
        }
        else
        {
            stackPushUndefined();  // In case of failure
        }
    });
}