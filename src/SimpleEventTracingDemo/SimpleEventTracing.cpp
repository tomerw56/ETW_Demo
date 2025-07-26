#include "TraceLoggerSingleton.h"

/*! \file
* \brief See ETW at work
*
*	You will see the thread id and use it on  WPA.
*   Please see https://learn.microsoft.com/en-us/windows/win32/tracelogging/tracelogging-native-quick-start for the additional data
*
*  - Execution:
*    -# wpr -start GeneralProfile -start simple_demo.WPRP
*    -# SimpleEventTracing.exe
*    -# wpr -stop simpleeventtracing.etl
*/

int main()
{

    char sampleValue[] = "Sample value";
    TraceLoggerSingleton::Instance().Init();
    TraceLoggerSingleton::Instance().WriteControlEvent("starting");

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate work
    
    TraceLoggerSingleton::Instance().WriteValueEvent(12);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work

    TraceLoggerSingleton::Instance().WriteControlEvent("Done");
}