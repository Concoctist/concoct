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

#ifndef SECONDS_H
#define SECONDS_H

#include <stdint.h>
#ifndef _WIN32
#include <sys/time.h> // timeval
#endif // _WIN32

static const uint32_t MICROSECONDS_PER_SECOND = 1000000ULL;

// Returns delta of 2 timevals
long double microdelta(time_t startsec, suseconds_t startusec, struct timeval* stop);

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/*
  Since this project conforms to the C99 standard, portable gettimeofday() functionality is necessary for Windows.
  Otherwise, timespec from the C11 standard could be used to obtain microseconds. Visual Studio does not support
  timespec_get() until VS 2019 version 16.8.0.
*/

typedef int32_t suseconds_t;

// Seconds between Windows epoch (01-01-1601 00:00:00 UTC) and Unix epoch (01-01-1970 00:00:00 UTC)
static const uint64_t EPOCH_OFFSET = 11644473600000000ULL;

typedef struct timeval
{
  time_t tv_sec;       // seconds
  suseconds_t tv_usec; // microseconds
} timeval;

struct timezone
{
  int tz_minuteswest; // minutes west of Greenwich
  int tz_dsttime;     // type of DST correction
};

// Fills timeval structure with the number of seconds and microseconds since the Unix epoch
int gettimeofday(struct timeval* tv, struct timezone* tz);
#endif // _WIN32

#endif // SECONDS_H
