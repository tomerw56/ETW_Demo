/*! \file TraceLoggerSingleton.h
* \brief holds the singelton for loggig events.
*
* <b>Note that we use the same guid and name 'SimpleTraceLoggingProvider' in simple_demo.WPRP</b>
*/

#pragma once
#include <windows.h>
#include <traceloggingprovider.h>

/* Forward-declare the g_hMyComponentProvider variable that you will use for tracing in this component*/
TRACELOGGING_DECLARE_PROVIDER(g_hMyComponentProvider);
#include <string>
#include <mutex>
#include <chrono>
#include <type_traits>

/*! The Guid and name we use on  simple_demo.WPRP*/
TRACELOGGING_DEFINE_PROVIDER(
    g_hMyComponentProvider,     // name of the global handle
    "SimpleTraceLoggingProvider",    // provider name
    // Your static GUID here
    (0x0205c616, 0xcf97, 0x5c11, 0x97, 0x56, 0x56, 0xa2, 0xce, 0xe0, 0x2c, 0xa7)
);

struct EventData {
    int64_t unix_time_stamp;
    const char* comment;
} ;

/*! The actual log
* <b>Please note the fixed strings need!</b>
 */
class TraceLoggerSingleton {
public:
    static TraceLoggerSingleton& Instance() {
        static TraceLoggerSingleton instance;
        return instance;
    }
	/*singelton pattern*/
    void Init() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!initialized_) {
           
            TraceLoggingRegister(g_hMyComponentProvider);
            initialized_ = true;
        }
    }

  void WriteControlEvent(const std::string& comment,DWORD& thread_id) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!initialized_) return;

        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()).count();
        EventData event_data;
        event_data.comment = comment.c_str();
        event_data.unix_time_stamp = static_cast<int64_t>(timestamp);
        TraceLoggingWrite(
            g_hMyComponentProvider,
            "Control",
            TraceLoggingStruct(2, "EventData"),
            TraceLoggingString(event_data.comment, "comment"),
            TraceLoggingUInt32(event_data.unix_time_stamp, "unix_time_stamp"),
            TraceLoggingUInt32(thread_id, "thread_id"),
            TraceLoggingPointer(&event_data, "ControlEventData")
        );


    }

    template<typename T>
    void WriteValueEvent( const T& value, DWORD& thread_id) {
        static_assert(
            std::is_same<T, bool>::value ||
            std::is_same<T, int>::value ||
            std::is_same<T, float>::value ||
            std::is_same<T, double>::value,
            "Unsupported type for WriteValueEvent");

        std::lock_guard<std::mutex> lock(mutex_);

        if (!initialized_) return;        
        TraceLoggingWrite(
            g_hMyComponentProvider,
            "Value",
            TraceLoggingUInt32(thread_id, "thread_id"),
            TraceLoggingValue(value, "key1")

        );
    }

    ~TraceLoggerSingleton() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (initialized_) {
            TraceLoggingUnregister(g_hMyComponentProvider);
            initialized_ = false;
        }
    }

    // Delete copy/move operations
    TraceLoggerSingleton(const TraceLoggerSingleton&) = delete;
    TraceLoggerSingleton& operator=(const TraceLoggerSingleton&) = delete;
    TraceLoggerSingleton(TraceLoggerSingleton&&) = delete;
    TraceLoggerSingleton& operator=(TraceLoggerSingleton&&) = delete;

private:
    TraceLoggerSingleton() : initialized_(false) {}
    std::mutex mutex_;
    bool initialized_;
};

