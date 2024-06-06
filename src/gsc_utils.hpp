#ifndef _GSC_UTILS_HPP_
#define _GSC_UTILS_HPP_

/* gsc functions */
#include "gsc.hpp"

void gsc_utils_sendcommandtoclient();

void gsc_utils_logprintconsole();

void gsc_utils_getsubstr();
void gsc_utils_getascii();
void gsc_utils_toupper();
void gsc_utils_tolower();
void gsc_utils_strtok();
void gsc_utils_replace();

void gsc_utils_file_exists();
void gsc_utils_fopen();
void gsc_utils_fread();
void gsc_utils_fwrite();
void gsc_utils_fclose();

void gsc_utils_getsystemtime();

void gsc_utils_getconfigstring();
void gsc_utils_makelocalizedstring();
#if COMPILE_LIBCURL == 1
void gsc_utils_webhookmessage();
#endif

#endif