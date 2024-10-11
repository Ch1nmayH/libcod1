#if COMPILE_CURL == 1

#ifndef GSC_GETCOUNTRY_HPP
#define GSC_GETCOUNTRY_HPP

#include <curl/curl.h>
#include <thread>
#include <memory>

// Function declaration for asynchronous country fetch
void gsc_player_getcountry(scr_entref_t ref);

#endif // GSC_GETCOUNTRY_HPP
