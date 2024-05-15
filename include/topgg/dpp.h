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

#include <topgg/dpp/result.h>
#include <topgg/dpp/models.h>
#include <topgg/dpp/client.h>
#include <topgg/dpp/autoposter.h>