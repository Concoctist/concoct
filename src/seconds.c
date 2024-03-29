/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (c) 2020-2023 BlakeTheBlock and Lloyd Dilley
 * http://concoct.ist/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>    // fabs()
#include "seconds.h"

#ifdef _WIN32
/*
  Since this project conforms to the C99 standard, portable gettimeofday() functionality is necessary for Windows.
  Otherwise, timespec from the C11 standard could be used to obtain microseconds. Visual Studio does not support
  timespec_get() until VS 2019 version 16.8.0.
*/

// Fills timeval structure with the number of seconds and microseconds since the Unix epoch
int gettimeofday(struct timeval* tv, struct timezone* tz)
{
  FILETIME ft;        // 64-bit value representing the number of 100-nanosecond intervals since 01-01-1601 00:00 UTC
  uint64_t microsecs; // microseconds
  TIME_ZONE_INFORMATION timezone;

  if(tv)
  {
#if WINVER >= _WIN32_WINNT_WIN8
    GetSystemTimePreciseAsFileTime(&ft);
#else
    GetSystemTimeAsFileTime(&ft);
#endif
    // Concatenate high and low values of FILETIME struct fields and remove seconds between Windows and Unix epoch
    microsecs = (((uint64_t)ft.dwHighDateTime << 32) + ft.dwLowDateTime) / 10 - EPOCH_OFFSET;
    // Save number of seconds
    tv->tv_sec = (time_t)(microsecs / MICROSECONDS_PER_SECOND);
    // Save number of microseconds
    tv->tv_usec = (suseconds_t)(microsecs % MICROSECONDS_PER_SECOND);
  }

  // Time zones are not used for this project, but this is here for completeness
  if(tz)
  {
    GetTimeZoneInformation(&timezone);
    tz->tz_minuteswest = timezone.Bias;
    tz->tz_dsttime = 0;
  }

  return 0;
}
#endif // _WIN32

// Returns delta of 2 timevals
double microdelta(time_t startsec, suseconds_t startusec, const struct timeval* stop)
{
  double start_total = startsec + (double)startusec / MICROSECONDS_PER_SECOND;
  double stop_total = stop->tv_sec + (double)stop->tv_usec / MICROSECONDS_PER_SECOND;
  if(start_total == 0.0)
    return 0.0;
  return fabs(stop_total - start_total);
}
