/*
// Copyright (c) 2021 Phoenix Technologies Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <gpiod.hpp>

#include "debug.hpp"

int api_set_gpio(const std::string& name, int value, const int polarity)
{
    gpiod::line line;

    line = gpiod::find_line(name);
    
    if (!line) {
        DPRINT("Error finding gpio: %s\n", name);
        return -1;
    }

    try
    {
        line.request({"phoenix-dbus-sensors",
            gpiod::line_request::DIRECTION_OUTPUT,
            polarity == gpiod::line::ACTIVE_HIGH
                        ? 0
                        : gpiod::line_request::FLAG_ACTIVE_LOW});
    }
    catch (std::system_error&) {
        DPRINT("Error requesting gpio: %s\n", name);
        return -1;
    }

    try
    {
        line.set_value(value);
    }
    catch (std::system_error& exc)
    {
        DPRINT("Error %s set_value\n", name);
        return -1;
    }

    return 0;
}

int api_get_gpio(const std::string& name, int *value, const int polarity)
{
    gpiod::line line;

    line = gpiod::find_line(name);
    
    if (!line) {
        DPRINT("Error finding gpio: %s\n", name);
        return -1;
    }

    try
    {
        line.request({"phoenix-dbus-sensors",
            gpiod::line_request::DIRECTION_INPUT,
            polarity == gpiod::line::ACTIVE_HIGH
                        ? 0
                        : gpiod::line_request::FLAG_ACTIVE_LOW});
    }
    catch (std::system_error&) {
        DPRINT("Error requesting gpio: %s\n", name);
        return -1;
    }

    try
    {
        *value = line.get_value();
    }
    catch (std::system_error& exc)
    {
        DPRINT("Error %s get_value\n", name);
        return -1;
    }

    return 0;
}

// TODO: Create more API here

