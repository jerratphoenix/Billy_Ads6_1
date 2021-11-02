/*
 * Copyright (c) 2021 Phoenix Technologies Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "type.hpp"
#include "debug.hpp"
#include "json_parser.hpp"
#include "phoenix_action.hpp"

namespace phoenix
{
namespace dbus
{
namespace monitor
{
#define MAX_SEQ_NUMBER 0xFFFF
#define BMC_SRC 0x20

std::string getSensorNameFromPath(std::string path);
void applyAlertAction(uint8_t policyNumber, std::vector<uint8_t> ed, uint8_t eventFilterNum);
void applyAction(uint8_t action, uint8_t global_action);
uint8_t checkEventData(uint8_t ed, uint8_t and_mask, uint8_t cmp_1, uint8_t cmp_2);
uint8_t checkMatch(std::vector<uint8_t> ed, int index);
int checkEventfilterTable(std::vector<uint8_t> ed);
int findAlertString(uint8_t is_event_specific, uint8_t stringKey, uint8_t eventNum);
int findNextEntry(int index, uint8_t policy_num, uint8_t pre_des_channel, bool is_channel);

}//monitor
}//dbus
}//phoenix
