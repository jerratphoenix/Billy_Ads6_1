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

#include "type.hpp"
#include "debug.hpp"
#include "json_parser.hpp"
#include "phoenix_action.hpp"
#include "phoenix_util.hpp"
#include "sendmail.hpp"
#include <snmp.hpp>
#include <snmp_notification.hpp>
#include <time.h>
#include <sys/time.h>

using namespace phosphor::network::snmp;

namespace phoenix
{
namespace dbus
{
namespace monitor
{
bool previous_alert = false;
uint8_t pre_channel = 0;
uint8_t pre_destination = 0;
uint16_t seqNumber = 0;

std::string getSensorNameFromPath(std::string path)
{
    std::string_view name(path);
    name.remove_prefix(
        std::min(name.find_last_of("/") + 1, name.size()));

    return std::string(name);
}

int findNextEntry(int index, uint8_t policy_num, uint8_t pre_des_channel, bool is_channel)
{
    DPRINT ("%s!!\n", __func__);

    bool found = false;
    int found_next_entry = -1;

    for(int j = index + 1; j < MAX_PEF_ALERT_POLICY_ENTRIES; j++)
    {

        uint8_t channel_dest = is_channel ?
            alert_policy_table[j].channel : alert_policy_table[j].destination;
    
        DPRINT ("%s, enable = %d, number = %d, chann_dest = %x\n"
            , __func__
            , alert_policy_table[j].enable
            , (alert_policy_table[j].policyNumber == policy_num)
            , channel_dest != pre_des_channel);

        if( alert_policy_table[j].enable && 
            alert_policy_table[j].policyNumber == policy_num &&
            channel_dest != pre_des_channel )
        {
            found_next_entry = j;
            found = true;

            DPRINT ("Next entry index : %x\n", j);

            continue;
        }
    }

    if( !found )
    {
        DPRINT ("Not found !!\n");
        return found_next_entry;
    }

    return found_next_entry;
}

int findAlertString(uint8_t is_event_specific, uint8_t stringKey, uint8_t eventNum)
{
    DPRINT ("%s!!\n", __func__);

    DPRINT ("Event number: %x, String Key: %x, event specific: %x\n"
        , eventNum, stringKey, is_event_specific);

    int string_index = MAX_PEF_ALERT_STRING_ENTRIES;

    if( is_event_specific )
    {
        for(int i = 0; i < MAX_PEF_ALERT_STRING_ENTRIES; i++)
        {
            if( string_table[i].eventFilterNum == eventNum &&
                string_table[i].alertStringSet == stringKey ) 
            {
                string_index = i;
            }
        }
    } else {
        for(int i = 0; i < MAX_PEF_ALERT_STRING_ENTRIES; i++)
        {
            if( stringKey && string_table[i].selector == stringKey)
            {
                string_index = i;
            }
        }
    }

    return string_index;
}

void applyAlertAction(uint8_t policyNumber, std::vector<uint8_t> ed, uint8_t eventFilterNum)
{
    DPRINT ("%s!!\n", __func__);

    if( !(global_pef_table.actionGlobalControl & PEF_ACTION_ALERT) )
    {
        DPRINT ("PEF global control alert action not enable(%x)\n"
            , global_pef_table.actionGlobalControl);
        return;
    }

    if( !policyNumber || !eventFilterNum )
    {
        DPRINT ("Invalid policy number: %x or event filter number %x\n"
            , policyNumber, eventFilterNum);
        return;
    }

    for(int i = 0; i < MAX_PEF_ALERT_POLICY_ENTRIES; i++)
    {
        if( (!alert_policy_table[i].enable) || (alert_policy_table[i].policyNumber != policyNumber) )
        {
            continue;
        }
        
        if( previous_alert )
        {
            switch ( static_cast<PolicyRule>(alert_policy_table[i].policy) )
            {
                case PolicyRule::AlwaysSend:
                    DPRINT ("PolicyRule::AlwaysSend\n");
                    break;
                case PolicyRule::ToNext:
                    DPRINT ("PolicyRule::ToNext\n");
                    continue;
                case PolicyRule::NoMore:
                    DPRINT ("PolicyRule::NoMore\n");
                    return;
                case PolicyRule::ToDiffChannel:
                {
                    DPRINT ("PolicyRule::ToDiffChannel\n");

                    int next = findNextEntry(i, policyNumber, pre_channel
                        , ChannelDestType::IsChannel);

                    if( next < 0 )
                    {
                        /*Not found the fit entry*/
                        return;
                    } else {
                        i = next;
                    }
                }
                break;
                case PolicyRule::ToDiffdestinationType:
                {
                    DPRINT ("PolicyRule::ToDiffdestinationType\n");

                    int next = findNextEntry(i, policyNumber, pre_destination
                        , ChannelDestType::IsDestination);

                    if( next < 0 )
                    {
                        /*Not found the fit entry*/
                        return;
                    } else {
                        i = next;
                    }
                } 
                break;
                default:
                    DPRINT ("No define policy rule: %x\n", alert_policy_table[i].policy);
                    break;
            }
        }

        int strIndex = findAlertString(alert_policy_table[i].is_event_specific
                            , alert_policy_table[i].alertSrtingKey, eventFilterNum);

        DPRINT ("Previous_alert: %d\n", previous_alert);

        if(strIndex < MAX_PEF_ALERT_STRING_ENTRIES)
        {
            DPRINT ("%s, PEF_ACTION_ALERT!!\n", __func__);
            DPRINT ("String table index: %x, alert string is \"%s\"\n"
                , strIndex, string_table[strIndex].alertString.c_str());

            if( seqNumber <= MAX_SEQ_NUMBER )
            {
                seqNumber += 1;
                DPRINT("seqNumber = %d\n", seqNumber);
            }

            /* Get the systime */
            struct timeval tv;
            gettimeofday(&tv, NULL);
            DPRINT("Seconds since Jan. 1, 1970: %ld\n", tv.tv_sec);

            DPRINT ("Dest:  %x\n", alert_policy_table[i].destination);

            //TODO: eventType, eventOffset, guid, sensorDevice, entity, entityInstance, mftID, systemID
            /* SNMP alert action */
            previous_alert = phoenixSendTrap<PhoenixErrorNotification>(seqNumber    //seqNum
                                           , pef_table[eventFilterNum-1].sensorType //sensorType
                                           , 0                                      //eventType
                                           , 0                                      //eventOffset
                                           , 0                                      //guid
                                           , tv.tv_sec                              //timeStamp
                                           , UNSPECIFIED_WORD                       //utc
                                           , BMC_SRC                                //trapSourceType
                                           , BMC_SRC                                //eventSourceType
                                           , pef_table[eventFilterNum-1].severity   //eventSeverity
                                           , 0                                      //sensorDevice
                                           , pef_table[eventFilterNum-1].sensorNumber //sensorNumber
                                           , 0                                      //entity
                                           , 0                                      //entityInstance
                                           , ed[0]                                  //eventdata 1 
                                           , ed[1]                                  //eventdata 2
                                           , ed[2]                                  //eventdata 3
                                           , UNSPECIFIED                            //languageCode
                                           , 0                                      //mftID
                                           , 0                                      //systemID
                                           , string_table[strIndex].alertString     //msg
                                           , alert_policy_table[i].destination);    //destination

            DPRINT ("previous_channel: %x, pre_destination: %x\n", pre_channel, pre_destination);

            pre_channel = alert_policy_table[i].channel;
            pre_destination = alert_policy_table[i].destination;

            DPRINT ("current_channel: %x, current_destination: %x\n", pre_channel, pre_destination);

            /* Email alert action*/ 
            email_action(seqNumber, pef_table[eventFilterNum-1].sensorType
                , pef_table[eventFilterNum-1].sensorNumber, ed);
        }
    }
}

#define CMP(x, y, z) ((x & y) & z)
void applyAction(uint8_t action, uint8_t global_action)
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
}

uint8_t checkEventData(uint8_t ed, uint8_t and_mask, uint8_t cmp_1, uint8_t cmp_2)
{
	uint8_t temp, match = 1;

	temp = ed & and_mask;

    DPRINT("%s, %d, ed = %x, and_mask = %x, cmp1 = %x, cmp2 = %x\n"
        , __func__, __LINE__
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

uint8_t checkMatch(std::vector<uint8_t> ed, int index)
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


int checkEventfilterTable(std::vector<uint8_t> ed)
{
    uint8_t filter_action = 0;
    uint8_t eventFilterNum = 0;
    uint8_t policyNumber = 0;
    int matched = 0;
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

            if( pef_table[i].filterAction & PEF_ACTION_ALERT )
            {
                policyNumber = ( pef_table[i].alertPolicyNumber & 0x0f );
                if( policyNumber )
                {
                    eventFilterNum = i + 1;
                    applyAlertAction(policyNumber, ed, eventFilterNum);
                }
            }

            DPRINT("%s, ed[0]: %x, ed[1]: %x, ed[2]: %x, filter_action: %x, "
                   "policyNumber = %x, eventFilterNum = %x\n"
                , __func__
                , ed[0], ed[1], ed[2]
                , filter_action
                , policyNumber
                , eventFilterNum);
        }
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
