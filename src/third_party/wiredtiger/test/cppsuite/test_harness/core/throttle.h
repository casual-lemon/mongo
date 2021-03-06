/*-
 * Public Domain 2014-present MongoDB, Inc.
 * Public Domain 2008-2014 WiredTiger, Inc.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef THROTTLE_H
#define THROTTLE_H

#include <thread>
#include <string>

#include "configuration.h"

namespace test_harness {
class throttle {
    public:
    explicit throttle(const std::string &throttle_rate)
    {
        std::string magnitude;
        uint64_t multiplier = 0;
        /*
         * Find the ms, s, or m in the string. Searching for "ms" first as the following two
         * searches would match as well.
         */
        size_t pos = throttle_rate.find("ms");
        if (pos != std::string::npos)
            multiplier = 1;
        else {
            pos = throttle_rate.find("s");
            if (pos != std::string::npos)
                multiplier = 1000;
            else {
                pos = throttle_rate.find("m");
                if (pos != std::string::npos)
                    multiplier = 60 * 1000;
                else
                    testutil_die(-1, "no rate specifier given");
            }
        }
        magnitude = throttle_rate.substr(0, pos);
        /* This will throw if it can't cast, which is fine. */
        _ms = std::stoi(magnitude) * multiplier;
    }

    explicit throttle(configuration *config) : throttle(config->get_string(OP_RATE)) {}

    /* Default to a second per operation. */
    throttle() : throttle("1s") {}

    void
    sleep()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(_ms));
    }

    private:
    uint64_t _ms = 1000;
};
} // namespace test_harness

#endif
