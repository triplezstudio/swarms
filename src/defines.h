#pragma once

#ifdef _WIN32
#ifdef EXPORT_SWARMS_LIB
#define SWARMS_API __declspec(dllexport)
#else
#define SWARMS_API __declspec(dllimport)
#endif
#else
#define SWARMS_API __attribute__((visibility("default")))
#endif