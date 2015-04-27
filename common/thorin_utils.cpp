#include <cstdlib>
#include <ctime>
#include <iostream>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "thorin_utils.h"

// common implementations of runtime utility functions
long long thorin_get_micro_time() {
#if defined(_WIN32) || defined(__CYGWIN__) // Use QueryPerformanceCounter on Windows
    LARGE_INTEGER counter, freq;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&freq);
    return counter.QuadPart * 1000000LL / freq.QuadPart;
#else
    struct timespec now;
#ifdef __APPLE__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    now.tv_sec = mts.tv_sec;
    now.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, &now);
#endif
    long long time = now.tv_sec * 1000000LL + now.tv_nsec / 1000LL;
    return time;
#endif
}
void thorin_print_micro_time(long long time) {
    std::cerr << "   timing: " << time / 1000 << "(ms)" << std::endl;
}
void thorin_print_gflops(float f) { printf("GFLOPS: %f\n", f); }
float thorin_random_val(int max) {
    return ((float)random() / RAND_MAX) * max;
}
