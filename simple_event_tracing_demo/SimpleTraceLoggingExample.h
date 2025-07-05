#pragma once

#include <windows.h> // Definitions required by TraceLoggingProvider.h
#include <TraceLoggingProvider.h> // The C/C++ TraceLogging API

// Forward-declare the g_hMyComponentProvider variable that you will use for tracing in this component
TRACELOGGING_DECLARE_PROVIDER(g_hMyComponentProvider);