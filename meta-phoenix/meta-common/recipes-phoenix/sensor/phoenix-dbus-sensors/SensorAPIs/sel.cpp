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
#include <boost/algorithm/string.hpp>

#include "debug.hpp"

static bool get_sel_log_files(std::vector<std::filesystem::path>& sel_log_files)
{
    static const std::filesystem::path sel_log_dir = "/var/log";
    static const std::string sel_log_filename = "ipmi_sel";

    for (const std::filesystem::directory_entry& dirent :
         std::filesystem::directory_iterator(sel_log_dir)) {
        std::string filename = dirent.path().filename();
        if (boost::starts_with(filename, sel_log_filename)) {
            sel_log_files.emplace_back(sel_log_dir /
                                     filename);
        }
    }

    std::sort(sel_log_files.begin(), sel_log_files.end());

    return !sel_log_files.empty();
}

static bool sel_empty(void)
{
    std::vector<std::filesystem::path> sel_log_files;
    if (!get_sel_log_files(sel_log_files)) {
        return true;
    }

    return false;
}

bool api_is_bmc_sel_empty (void)
{
    return sel_empty();
}

// TODO: Create more API here


