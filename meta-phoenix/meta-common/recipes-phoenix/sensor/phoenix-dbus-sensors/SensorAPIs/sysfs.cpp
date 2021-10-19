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
#include <regex>
#include <vector>

#include "debug.hpp"

int32_t api_read_first_line_to_value (const std::string file, double *value)
{
    char read_buf[256];

    if (!std::filesystem::exists(file)) {
        DPRINT("Error file %s not exists\n", file.c_str());
        return -1;
    }

    std::ifstream fileStream (file.c_str());

    fileStream.getline (read_buf, 256);

    fileStream.close();
    
    *value = std::stof(read_buf);

    return 0;
}

bool find_sysfs_files(const std::filesystem::path dirPath, const std::string& matchString,
               std::vector<std::filesystem::path>& foundPaths, unsigned int symlinkDepth)
{
    if (!std::filesystem::exists(dirPath))
        return false;

    std::regex search(matchString);
    std::smatch match;
    for (auto& p : std::filesystem::recursive_directory_iterator(dirPath))
    {
        std::string path = p.path().string();
        if (!is_directory(p))
        {
            if (std::regex_search(path, match, search))
                foundPaths.emplace_back(p.path());
        }
        else if (is_symlink(p) && symlinkDepth)
        {
            find_sysfs_files(p.path(), matchString, foundPaths, symlinkDepth - 1);
        }
    }
    return true;
}

// TODO: Create more API here

