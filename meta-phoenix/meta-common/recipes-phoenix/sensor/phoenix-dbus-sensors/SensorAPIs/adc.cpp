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

#include <stdlib.h>
#include <string.h>

#include "adc.hpp"
#include "sysfs.hpp"
#include "debug.hpp"

int32_t api_get_adc_from_sysfs(const std::string file, double *value, int r1, int r2)
{
    double adc_raw;
    int32_t status;

    status = api_read_first_line_to_value(file, &adc_raw);

    if (r2 <= 0) {
        r1 = 0;
        r2 = 1;
    }

    *value = adc_raw / 1000 * (r1 + r2) / r2;

    return status;
}

static bool is_adc_hwmon(const std::filesystem::path& parent_path)
{
    std::filesystem::path name_path = parent_path / "name";

    std::ifstream name_file(name_path);
    if (!name_file.good()) {
        return false;
    }

    std::string name;
    std::getline(name_file, name);

    return name == "iio_hwmon";
}

int32_t api_get_adc(uint8_t channel, double *value, int r1, int r2)
{
    std::vector<std::filesystem::path> paths;
    std::string hwmon_path;
    std::string file;
    bool adc_file_find = false;

    if (!find_sysfs_files(std::filesystem::path("/sys/devices/platform/iio-hwmon/hwmon/"), R"(hwmon\d+/name)",
                    paths, 0)) {
        std::cerr << "No aspeed adc sensors in sysfs\n";
        return -1;
    }

    for (auto& path : paths)
    {
        if (is_adc_hwmon(path.parent_path())) {
            hwmon_path = path.parent_path().string();
            adc_file_find = true;
            break;
        }
    }

    if (adc_file_find == true) {
        //DPRINT("ADC hwmon path %s\n", hwmon_path.c_str());

        file.append(hwmon_path);
        file.append("in");
        file.append(std::to_string(channel + 1));
        file.append("_input");

        DPRINT("ADC%d file path %s\n", channel, file.c_str());

        return api_get_adc_from_sysfs(file, value, r1, r2);
    }

    // TODO: if no sysfs device for adc, try to find driver device to access

    return 0;
};


// TODO: Create more API here



