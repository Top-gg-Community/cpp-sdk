#pragma once

#if defined(_WIN32) && defined(TOPGG_SHARED)
#ifdef __TOPGG_BUILDING__
#include <dpp/win32_safe_warnings.h>
#define TOPGG_EXPORT __declspec(dllexport)
#else
#define TOPGG_EXPORT __declspec(dllimport)
#endif
#else
#define TOPGG_EXPORT
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TOPGG_UNUSED __attribute__((unused))
#else
#define TOPGG_UNUSED
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-literal-operator"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-dtor"
#endif

#include <dpp/dpp.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <topgg/dpp/result.h>
#include <topgg/dpp/models.h>
#include <topgg/dpp/client.h>
#include <topgg/dpp/autoposter.h>