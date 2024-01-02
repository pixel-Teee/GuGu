#pragma once

#include <time.h>
#include <string.h>

#ifdef ANDROID
#include <android/log.h>
#endif

// Compatible with Android NDK
enum GuGuLogPriority {
    GuGu_LOG_DEBUG = 3,
    GuGu_LOG_INFO,
    GuGu_LOG_WARN,
    GuGu_LOG_ERROR,
};
const static char* g_GuGu_log_priority_str[7] = {
    0, 0, 0,
    "Debug",
    "Info",
    "Warn",
    "Error"
};

//--------------------------------------------------------------------------------
// => switches for each log level
// [user setting]
//--------------------------------------------------------------------------------
#define _GuGu_LOG_DEBUG_ENABLED      1
#define _GuGu_LOG_INFO_ENABLED       1
#define _GuGu_LOG_WARN_ENABLED       1
#define _GuGu_LOG_ERROR_ENABLED      1

//--------------------------------------------------------------------------------
// => select log format
// [user setting]
//--------------------------------------------------------------------------------
#define _GuGu_LOGFMT_FULL            1
#define _GuGu_LOGFMT_MEDIUM          0
#define _GuGu_LOGFMT_SIMPLE          0

//--------------------------------------------------------------------------------
// => filename
//--------------------------------------------------------------------------------
// only filename
#define _GuGu_FILE strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
// contain folder name such as src/log.cpp
//#define _FILE __FILE__

//--------------------------------------------------------------------------------
// => function
//--------------------------------------------------------------------------------
// only function name
#define _GuGu_FUNCTION __FUNCTION__
// function name with parameter types
//#define _FUNCTION __PRETTY_FUNCTION__

//--------------------------------------------------------------------------------
// => tag
//--------------------------------------------------------------------------------
// the tag for current module/library/file
#define _GuGu_MODULE_TAG "GuGu"


#if _GuGu_LOGFMT_FULL
static inline char* timenow() {
    time_t rawtime = time(NULL);
    struct tm* timeinfo = localtime(&rawtime);
    static char now[64];

    //--------------------------------------------------------------------------------
    // => timestamp format
    //--------------------------------------------------------------------------------
#define _GuGu_TIMESTAMP_FMT "%Y-%m-%d %H:%M:%S"
// if timezone required, use %z
//#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S(%z)"

    now[strftime(now, sizeof(now), _GuGu_TIMESTAMP_FMT, timeinfo)] = '\0';
#undef _GuGu_TIMESTAMP_FMT
    return now;
}
#endif

//--------------------------------------------------------------------------------
// => log format
//--------------------------------------------------------------------------------
// the param `fmt` is user specified format
// _PXL_LOG_FULL_FMT(fmt) ammed pre-defined stuffs (time, filename, line num, function) with colors
#define _GuGu_LOG_FULL_FMT(fmt)                   ("%s | %s | %s[%-5s]\x1b[0m | %s, line %d, %s | " fmt "\n")
#define _GuGu_LOG_FULL_ARGS(tag, color, priority) timenow(), tag, color, g_GuGu_log_priority_str[priority], _GuGu_FILE, __LINE__, _GuGu_FUNCTION

// the param `fmt` is user specified format
// _PXL_LOG_MEDIUM_FMT(fmt) ammed pre-defined stuffs (filename, line num)
#define _GuGu_LOG_MEDIUM_FMT(fmt)                   ("%s/%s%-5s\x1b[0m | %s, line %d | " fmt "\n")
#define _GuGu_LOG_MEDIUM_ARGS(tag, color, priority) tag, color, g_GuGu_log_priority_str[priority], _GuGu_FILE, __LINE__


//--------------------------------------------------------------------------------
// => log template for all levels
//--------------------------------------------------------------------------------
// the full format log
#ifdef ANDROID
#define _GuGu_LOGT_FULL(priority, fmt, tag, fd, color, ...) do { \
        __android_log_print(priority, tag, _GuGu_LOG_FULL_FMT(fmt), _GuGu_LOG_FULL_ARGS(tag, color, priority), ##__VA_ARGS__); \
        fprintf(fd, _GuGu_LOG_FULL_FMT(fmt), _GuGu_LOG_FULL_ARGS(tag, color, priority), ##__VA_ARGS__); \
    } while(0)
#else
#define _GuGu_LOGT_FULL(priority, fmt, tag, fd, color, ...) do { \
        fprintf(fd, _GuGu_LOG_FULL_FMT(fmt), _GuGu_LOG_FULL_ARGS(tag, color, priority), ##__VA_ARGS__); \
    } while(0)
#endif

// the medium format log
#ifdef ANDROID
#define _GuGu_LOGT_MEDIUM(priority, fmt, tag, fd, color, ...) do { \
        __android_log_print(priority, tag, _GuGu_LOG_MEDIUM_FMT(fmt), _GuGu_LOG_MEDIUM_ARGS(tag, color, priority), ##__VA_ARGS__); \
        fprintf(fd, _GuGu_LOG_MEDIUM_FMT(fmt), _GuGu_LOG_MEDIUM_ARGS(tag, color, priority), ##__VA_ARGS__); \
    } while(0)
#else
#define _GuGu_LOGT_MEDIUM(priority, fmt, tag, fd, color, ...) do { \
        fprintf(fd, _GuGu_LOG_MEDIUM_FMT(fmt), _GuGu_LOG_MEDIUM_ARGS(tag, color, priority), ##__VA_ARGS__); \
    } while(0)
#endif

// simple format log
#ifdef ANDROID
#define _GuGu_LOGT_SIMPLE(priority, fmt, ...) do { \
       __android_log_print(priority, _GuGu_MODULE_TAG, fmt, ##__VA_ARGS__); \
       fprintf(stdout, (fmt "\n"), ##__VA_ARGS__); \
    } while(0)
#else
#define _GuGu_LOGT_SIMPLE(priority, fmt, ...) do { \
        fprintf(stdout, (fmt "\n"), ##__VA_ARGS__); \
    } while(0)
#endif


//--------------------------------------------------------------------------------
// => log template for each level
//--------------------------------------------------------------------------------
// NOTE: if using stderr, `run-and-install.sh`(start on PC, run on Android) will mess up log order
// For better log order, always use stdout
#if _GuGu_LOGFMT_FULL
#define _GuGu_LOGDT_FULL(fmt, ...) _GuGu_LOGT_FULL(GuGu_LOG_DEBUG, fmt, _GuGu_MODULE_TAG, stdout, "\x1b[36m", ##__VA_ARGS__)
#define _GuGu_LOGIT_FULL(fmt, ...) _GuGu_LOGT_FULL(GuGu_LOG_INFO,  fmt, _GuGu_MODULE_TAG, stdout, "\x1b[32m", ##__VA_ARGS__)
#define _GuGu_LOGWT_FULL(fmt, ...) _GuGu_LOGT_FULL(GuGu_LOG_WARN,  fmt, _GuGu_MODULE_TAG, stdout, "\x1b[33m", ##__VA_ARGS__)
#define _GuGu_LOGET_FULL(fmt, ...) _GuGu_LOGT_FULL(GuGu_LOG_ERROR, fmt, _GuGu_MODULE_TAG, stdout, "\x1b[31m", ##__VA_ARGS__)
#elif _GuGu_LOGFMT_MEDIUM
#define _GuGu_LOGDT_MEDIUM(fmt, ...) _GuGu_LOGT_MEDIUM(GuGu_LOG_DEBUG, fmt, _GuGu_MODULE_TAG, stdout, "\x1b[36m", ##__VA_ARGS__)
#define _GuGu_LOGIT_MEDIUM(fmt, ...) _GuGu_LOGT_MEDIUM(GuGu_LOG_INFO,  fmt, _GuGu_MODULE_TAG, stdout, "\x1b[32m", ##__VA_ARGS__)
#define _GuGu_LOGWT_MEDIUM(fmt, ...) _GuGu_LOGT_MEDIUM(GuGu_LOG_WARN,  fmt, _GuGu_MODULE_TAG, stdout, "\x1b[33m", ##__VA_ARGS__)
#define _GuGu_LOGET_MEDIUM(fmt, ...) _GuGu_LOGT_MEDIUM(GuGu_LOG_ERROR, fmt, _GuGu_MODULE_TAG, stdout, "\x1b[31m", ##__VA_ARGS__)
#elif _GuGu_LOGFMT_SIMPLE
#define _GuGu_LOGDT_SIMPLE(fmt, ...) _GuGu_LOGT_SIMPLE(GuGu_LOG_DEBUG, fmt, ##__VA_ARGS__)
#define _GuGu_LOGIT_SIMPLE(fmt, ...) _GuGu_LOGT_SIMPLE(GuGu_LOG_INFO,  fmt, ##__VA_ARGS__)
#define _GuGu_LOGWT_SIMPLE(fmt, ...) _GuGu_LOGT_SIMPLE(GuGu_LOG_WARN,  fmt, ##__VA_ARGS__)
#define _GuGu_LOGET_SIMPLE(fmt, ...) _GuGu_LOGT_SIMPLE(GuGu_LOG_ERROR, fmt, ##__VA_ARGS__)
#else
#pragma message("Please define one of _GuGu_LOGFMT_FULL, _GuGu_LOGFMT_MEDIUM or _GuGu_LOGFMT_SIMPLE") 
#endif

//--------------------------------------------------------------------------------
// => specify each log level's template
// [user setting]
//--------------------------------------------------------------------------------
// use full format template
#if _GuGu_LOGFMT_FULL
#define _GuGu_LOGDT(fmt, ...) _GuGu_LOGDT_FULL(fmt, ##__VA_ARGS__)
#define _GuGu_LOGIT(fmt, ...) _GuGu_LOGIT_FULL(fmt, ##__VA_ARGS__)
#define _GuGu_LOGWT(fmt, ...) _GuGu_LOGWT_FULL(fmt, ##__VA_ARGS__)
#define _GuGu_LOGET(fmt, ...) _GuGu_LOGET_FULL(fmt, ##__VA_ARGS__)
#elif _GuGu_LOGFMT_MEDIUM
#define _GuGu_LOGDT(fmt, ...) _GuGu_LOGDT_MEDIUM(fmt, ##__VA_ARGS__)
#define _GuGu_LOGIT(fmt, ...) _GuGu_LOGIT_MEDIUM(fmt, ##__VA_ARGS__)
#define _GuGu_LOGWT(fmt, ...) _GuGu_LOGWT_MEDIUM(fmt, ##__VA_ARGS__)
#define _GuGu_LOGET(fmt, ...) _GuGu_LOGET_MEDIUM(fmt, ##__VA_ARGS__)
#elif _GuGu_LOGFMT_SIMPLE
#define _GuGu_LOGIT(fmt, ...) _GuGu_LOGIT_SIMPLE(fmt, ##__VA_ARGS__)
#define _GuGu_LOGDT(fmt, ...) _GuGu_LOGDT_SIMPLE(fmt, ##__VA_ARGS__)
#define _GuGu_LOGWT(fmt, ...) _GuGu_LOGWT_SIMPLE(fmt, ##__VA_ARGS__)
#define _GuGu_LOGET(fmt, ...) _GuGu_LOGET_SIMPLE(fmt, ##__VA_ARGS__)
#else
#pragma message("Please define one of _GuGu_LOGFMT_FULL, _GuGu_LOGFMT_MEDIUM or _GuGu_LOGFMT_SIMPLE") 
#endif

//--------------------------------------------------------------------------------
// => enable log macro according to LOG_LEVEL
//--------------------------------------------------------------------------------
#if _GuGu_LOG_INFO_ENABLED
#define GuGu_LOGI(fmt, ...) _GuGu_LOGIT(fmt, ##__VA_ARGS__)
#else
#define GuGu_LOGI(fmt, ...)
#endif

#if _GuGu_LOG_DEBUG_ENABLED
#define GuGu_LOGD(fmt, ...) _GuGu_LOGDT(fmt, ##__VA_ARGS__)
#else
#define GuGu_LOGD(fmt, ...)
#endif

#if _GuGu_LOG_WARN_ENABLED
#define GuGu_LOGW(fmt, ...) _GuGu_LOGWT(fmt, ##__VA_ARGS__)
#else
#define GuGu_LOGW(fmt, ...)
#endif

#if _GuGu_LOG_ERROR_ENABLED
#define GuGu_LOGE(fmt, ...) _GuGu_LOGET(fmt, ##__VA_ARGS__)
#else
#define GuGu_LOGE(fmt, ...)
#endif

#if _GuGu_LOG_ERROR_ENABLED
#define GuGu_LOG_IF_ERROR(condition, fmt, ...) do { \
        if (condition) GuGu_LOGE(fmt, ##__VA_ARGS__); \
    } while(0) 
#else
#define GuGu_LOG_IF_ERROR(condition, fmt, ...)
#endif