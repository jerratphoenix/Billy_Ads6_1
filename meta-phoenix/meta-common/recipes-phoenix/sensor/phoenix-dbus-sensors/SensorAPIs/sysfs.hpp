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
#include "debug.hpp"

#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <regex>

int32_t api_read_first_line_to_value (const std::string file, double *value);

bool find_sysfs_files(const std::filesystem::path dirPath, const std::string& matchString,
               std::vector<std::filesystem::path>& foundPaths, unsigned int symlinkDepth);

