#pragma once

#if defined(TOPGG_STATIC_DEFINE)
    #define TOPGG_API
#else
    #ifdef _MSC_VER
        #ifdef TOPGG_EXPORTS
            #define TOPGG_API __declspec(dllexport)
        #else
            #define TOPGG_API __declspec(dllimport)
        #endif
    #else
        #define TOPGG_API
    #endif
#endif 

// Add macro for unused variables in catch blocks
#if defined(__GNUC__) || defined(__clang__)
    #define TOPGG_UNUSED __attribute__((unused))
#else
    #define TOPGG_UNUSED
#endif 