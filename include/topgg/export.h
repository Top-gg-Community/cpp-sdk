/**
 * @module topgg
 * @file export.h
 * @brief A simple API wrapper for Top.gg written in C++.
 * @authors Top.gg, null8626
 * @copyright Copyright (c) 2024-2025 Top.gg & null8626
 * @date 2025-09-13
 * @version 3.0.0
 */

#pragma once

#if defined(_WIN32) && defined(__TOPGG_API__)
#if defined(DPP_STATIC) && !defined(TOPGG_STATIC)
#define TOPGG_STATIC
#elif defined(TOPGG_STATIC) && !defined(DPP_STATIC)
#define DPP_STATIC
#endif
#endif

#if defined(_WIN32) && !defined(TOPGG_STATIC)
#ifdef __TOPGG_BUILDING_DLL__
#ifdef __TOPGG_API__
#include <dpp/win32_safe_warnings.h>
#endif
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