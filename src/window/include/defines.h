#pragma once

#ifdef _WIN32
#ifdef EXPORT_TZ_LIB
#define TZ_API __declspec(dllexport)
#else
#define TZ_API __declspec(dllimport)
#endif
#else
#define TZ_API __attribute__((visibility("default")))
#endif