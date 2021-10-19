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

static inline std::string getSensorNameFromPath(std::string path)
{
    std::string_view name(path);
    name.remove_prefix(
        std::min(name.find_last_of("/") + 1, name.size()));

    return std::string(name);
}

#define CMP(x, y, z) ((x & y) & z)
static inline void applyAction(uint8_t action, uint8_t global_action)
{
    if( CMP(action, PEF_ACTION_POWER_DOWN, global_action) )
    {
        //TODO: check start up delay

        DPRINT("%s, PEF_ACTION_POWER_DOWN!!\n", __func__);
        power_off();
    }

    if( CMP(action, PEF_ACTION_POWER_CYCLE, global_action) )
    {
        //TODO: check start up delay

        DPRINT("%s, PEF_ACTION_POWER_CYCLE!!\n", __func__);

        /* No effect if power is already off */
        if( is_power_on() )
        {
            power_cycle();
        }
    }

    if( CMP(action, PEF_ACTION_POWER_RESET, global_action) )
    {
        DPRINT("%s, PEF_ACTION_POWER_RESET!!\n", __func__);
        hard_reset();
    }

    if( CMP(action, PEF_ACTION_ALERT, global_action) )
    {
        //TODO: check start up delay

        DPRINT("%s, PEF_ACTION_ALERT!!\n", __func__);
        send_alert();
    }

}

static inline uint8_t checkEventData(uint8_t ed, uint8_t and_mask, uint8_t cmp_1, uint8_t cmp_2)
{
	uint8_t temp, match = 1;

	temp = ed & and_mask;

    DPRINT("%s, %d, ed = %x, and = %x, cmp1 = %x, cmp2 = %x\n", __func__, __LINE__
        , ed, and_mask, cmp_1, cmp_2);

	if ((temp & cmp_1) == (cmp_1 & cmp_2)) {
		if ((cmp_1 != 0xff) && (and_mask & ~cmp_1)) {
			temp &= ~cmp_1;

			if (cmp_2 & ~cmp_1) {
				if ((temp & cmp_2) == 0) {
					match = 0;
				}
			}

			if (match && (cmp_2==0) && (cmp_1==0)) {
				if ((~(temp | cmp_2 ) & ~cmp_1) == 0) {
					match = 0;
				}
			}
		}
	} else {
		match = 0;
	}

    DPRINT("%s, match = %x\n", __func__, match);
	return match;
}

static inline uint8_t checkMatch(std::vector<uint8_t> ed, int index)
{
    uint8_t ret = 0;

    ret = checkEventData(ed[0], pef_table[index].ED_1_ANDMask
            , pef_table[index].ED_1_Compare_1, pef_table[index].ED_1_Compare_2);
    if(!ret) 
    {
        DPRINT("Event data 1 doesn't match\n!");
        return 0; 
    }

    ret = checkEventData(ed[1], pef_table[index].ED_2_ANDMask
            , pef_table[index].ED_2_Compare_1, pef_table[index].ED_2_Compare_2);
    if(!ret) 
    {
        DPRINT("Event data 2 doesn't match\n!");
        return 0; 
    }

    ret = checkEventData(ed[2], pef_table[index].ED_3_ANDMask
            , pef_table[index].ED_3_Compare_1, pef_table[index].ED_3_Compare_2);
    if(!ret) 
    {
        DPRINT("Event data 3 doesn't match\n!");
        return 0; 
    }

    return ret; 
}


static inline int checkEventfilterTable(std::vector<uint8_t> ed)
{
    int matched = 0;
    uint8_t filter_action = 0;
    int pefTableSize = getJsonSize();

    for( int i = 0; i < pefTableSize; i++ )
    {
        if( (pef_table[i].filterConfig & FILTER_TABLE_EN) == 0 )
        {
            continue;
        }

        if( checkMatch(ed, i) )
        {
            matched = 1;
            filter_action |= pef_table[i].filterAction;

            DPRINT("%s, ed[0]: %x, ed[1]: %x, ed[2]: %x, filter_action: %x\n"
                , __func__
                , ed[0], ed[1], ed[2]
                , filter_action);
        }

        //TODO: alert 
        
    }

	if ( !matched )
    {
		return 0;
	}

    applyAction(filter_action, global_pef_table.actionGlobalControl);

    return matched;
}

}//monitor
}//dbus
}//phoenix

