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

#include "pefcommands.hpp"

using json = nlohmann::json;

namespace ipmi
{
namespace pef
{
void registerPefFunctions() __attribute__((constructor));

static Progress ProgressStatus = Progress::Reserved;

namespace jsonns {
    #define STR_TO_HEX(s) strtoul(j[s].get<std::string>().c_str(), NULL, 0);
    #define HEX_TO_STR(h) std::to_string(h)

    /* Json init data to struct callbak */
    void from_json(const nlohmann::json& j, eventFilter& v) {
        v.selector =        STR_TO_HEX("Selector");
        v.filterConfig =    STR_TO_HEX("FilterConfiguration");
        v.filterAction =    STR_TO_HEX("EventFilterAction");
        v.alertPolicyNumber = STR_TO_HEX("AlertPolicyNumber");
        v.severity =        STR_TO_HEX("EventSeverity");
        v.genID_1 =         STR_TO_HEX("GeneratorIDByte_1");
        v.genID_2 =         STR_TO_HEX("GeneratorIDByte_2");
        v.sensorType =      STR_TO_HEX("SensorType");
        v.sensorNumber =    STR_TO_HEX("SensorNumber");
        v.eventTrigger =    STR_TO_HEX("EventTrigger");
        v.ED_1_OffsetMask = STR_TO_HEX("EventData_1_EventOffsetMask");
        v.ED_1_ANDMask =    STR_TO_HEX("EventData_1_ANDMask");
        v.ED_1_Compare_1 =  STR_TO_HEX("EventData_1_Compare_1");
        v.ED_1_Compare_2 =  STR_TO_HEX("EventData_1_Compare_2");
        v.ED_2_ANDMask =    STR_TO_HEX("EventData_2_ANDMask");
        v.ED_2_Compare_1 =  STR_TO_HEX("EventData_2_Compare_1");
        v.ED_2_Compare_2 =  STR_TO_HEX("EventData_2_Compare_2");
        v.ED_3_ANDMask =    STR_TO_HEX("EventData_3_ANDMask");
        v.ED_3_Compare_1 =  STR_TO_HEX("EventData_3_Compare_1");
        v.ED_3_Compare_2 =  STR_TO_HEX("EventData_3_Compare_2");
    }

    void to_json(json& j, const eventFilter& v) {
        j = json{
                    {"Selector", HEX_TO_STR(v.selector)}, 
                    {"FilterConfiguration", HEX_TO_STR(v.filterConfig)}, 
                    {"EventFilterAction", HEX_TO_STR(v.filterAction)},
                    {"AlertPolicyNumber", HEX_TO_STR(v.alertPolicyNumber)},
                    {"EventSeverity", HEX_TO_STR(v.severity)},
                    {"GeneratorIDByte_1", HEX_TO_STR(v.genID_1)},
                    {"GeneratorIDByte_2", HEX_TO_STR(v.genID_2)},
                    {"SensorType", HEX_TO_STR(v.sensorType)},
                    {"SensorNumber", HEX_TO_STR(v.sensorNumber)},
                    {"EventTrigger", HEX_TO_STR(v.eventTrigger)},
                    {"EventData_1_EventOffsetMask", HEX_TO_STR(v.ED_1_OffsetMask)},
                    {"EventData_1_ANDMask", HEX_TO_STR(v.ED_1_ANDMask)},
                    {"EventData_1_Compare_1", HEX_TO_STR(v.ED_1_Compare_1)},
                    {"EventData_1_Compare_2", HEX_TO_STR(v.ED_1_Compare_2)},
                    {"EventData_2_ANDMask", HEX_TO_STR(v.ED_2_ANDMask)},
                    {"EventData_2_Compare_1", HEX_TO_STR(v.ED_2_Compare_1)},
                    {"EventData_2_Compare_2", HEX_TO_STR(v.ED_2_Compare_2)},
                    {"EventData_3_ANDMask", HEX_TO_STR(v.ED_3_ANDMask)},
                    {"EventData_3_Compare_1", HEX_TO_STR(v.ED_3_Compare_1)},
                    {"EventData_3_Compare_2", HEX_TO_STR(v.ED_3_Compare_2)},
            };
    }

    void from_json(const nlohmann::json& j, globalConfig& v) {
        v.control =             STR_TO_HEX("PEF_control");
        v.actionGlobalControl = STR_TO_HEX("PEF_Action_global_control");
        v.startupDelay =        STR_TO_HEX("PEF_Startup_Delay");
        v.alertstartupDelay =   STR_TO_HEX("PEF_Alert_Startup_Delay");
    }

    void to_json(json& j, const globalConfig& v) {
        j = json{
                    {"PEF_control", HEX_TO_STR(v.control)}, 
                    {"PEF_Action_global_control", HEX_TO_STR(v.actionGlobalControl)}, 
                    {"PEF_Startup_Delay", HEX_TO_STR(v.startupDelay)},
                    {"PEF_Alert_Startup_Delay", HEX_TO_STR(v.alertstartupDelay)}
            };
    }

    void from_json(const nlohmann::json& j, alertPolicyTableInfo& v) {
        v.policyNumber =        STR_TO_HEX("PolicyNumber");
        v.enable =              STR_TO_HEX("Enable");
        v.policy =              STR_TO_HEX("Policy");
        v.channel =             STR_TO_HEX("Channel");
        v.destination =         STR_TO_HEX("Destination");
        v.is_event_specific =   STR_TO_HEX("Is_Event_Specific");
        v.alertSrtingKey =      STR_TO_HEX("AlertStringKey");
    }

    void to_json(json& j, const alertPolicyTableInfo& v) {
        j = json{
                    {"PolicyNumber", HEX_TO_STR(v.policyNumber)}, 
                    {"Enable", HEX_TO_STR(v.enable)}, 
                    {"Policy", HEX_TO_STR(v.policy)},
                    {"Channel", HEX_TO_STR(v.channel)}, 
                    {"Destination", HEX_TO_STR(v.destination)}, 
                    {"Is_Event_Specific", HEX_TO_STR(v.is_event_specific)},
                    {"AlertStringKey", HEX_TO_STR(v.alertSrtingKey)}
            };
    }
}//jsonns
struct jsonns::eventFilter eventFilterTable[MAX_PEF_EVENT_ENTRIES];
struct jsonns::globalConfig globalConfigTable;
struct jsonns::alertPolicyTableInfo alert_policy_table[MAX_PEF_ALERT_POLICY_ENTRIES];

uint8_t getNumberOfAlertPolicyEntries()
{
    uint8_t size = 0;
    std::ifstream jsonFile( jsonns::configFile );
    nlohmann::json data = nlohmann::json::parse(jsonFile, nullptr, false);
    if (!data.is_discarded())
    {
        size = data[jsonns::ALERT_TABLE_NAME].size();
    }

    return size;
}

int setPefAlertPolicyTable(uint8_t set)
{
    nlohmann::json j;

    std::ifstream jfile( jsonns::configFile );

    if( !jfile )
    {
        std::cerr << "Open PEF json file fail!!\n";
        return -1;
    }

    jfile >> j;
    jfile.close();


    int alertPolicyEntries = set + 1;

    // struct back to json
    for ( int i = 0; i < alertPolicyEntries; i++ ) {
        j[jsonns::ALERT_TABLE_NAME][i] = alert_policy_table[i];
    }

    std::ofstream ojfile( jsonns::configFile );

    ojfile << std::setw(4) << j << "\n";
    ojfile.close();

    return 0;
}

int getPefAlertPolicyTable()
{
    nlohmann::json j;

    std::ifstream jfile( jsonns::configFile );

    if( !jfile )
    {
        std::cerr << "Open PEF json file fail!!\n";
        return -1;
    }

    jfile >> j;
    jfile.close();

    int alertPolicyNum = getNumberOfAlertPolicyEntries();
    if( alertPolicyNum > MAX_PEF_ALERT_POLICY_ENTRIES )
    {
        alertPolicyNum = MAX_PEF_ALERT_POLICY_ENTRIES;
    }

    for ( int i = 0; i < alertPolicyNum; i++ ) {
        // Callback to from_json funciotn
        alert_policy_table[i] = j[jsonns::ALERT_TABLE_NAME][i];
    }

    return 0;
}

int getPefGlobalConfigure()
{
    nlohmann::json j;

    std::ifstream jfile( jsonns::configFile );

    if( !jfile )
    {
        std::cerr << "Open PEF json file fail!!\n";
        return -1;
    }

    jfile >> j;
    jfile.close();

    int globalpeflength = j[jsonns::GLOBAL_TABLE_NAME].size();

    for ( int i = 0; i < globalpeflength; i++ ) {
        // Callback to from_json funciotn
        globalConfigTable = j[jsonns::GLOBAL_TABLE_NAME][i];
    }

    return 0;
}

int setPefGlobalConfigure()
{
    nlohmann::json j;

    std::ifstream jfile( jsonns::configFile );

    if( !jfile )
    {
        std::cerr << "Open PEF json file fail!!\n";
        return -1;
    }

    jfile >> j;
    jfile.close();

    int globalpeflength = j[jsonns::GLOBAL_TABLE_NAME].size();

    // struct back to json
    for ( int i = 0; i < globalpeflength; i++ ) {
        j[jsonns::GLOBAL_TABLE_NAME][i] = globalConfigTable;
    }

    std::ofstream ojfile( jsonns::configFile );

    ojfile << std::setw(4) << j << "\n";
    ojfile.close();

    return 0;
}

int getPefEventFilterTable()
{
    nlohmann::json j;

    std::ifstream jfile( jsonns::configFile );

    if( !jfile )
    {
        std::cerr << "Open PEF json file fail!!\n";
        return -1;
    }

    jfile >> j;
    jfile.close();

    int eventFilterNum = j[jsonns::TABLE_NAME].size();
    if( eventFilterNum > MAX_PEF_EVENT_ENTRIES )
    {
        eventFilterNum = MAX_PEF_EVENT_ENTRIES;
    }

    for ( int i = 0; i < eventFilterNum; i++ ) {
        // Callback to from_json funciotn
        eventFilterTable[i] = j[jsonns::TABLE_NAME][i];
    }

    return 0;
}

int setPefEventFilterTable()
{
    nlohmann::json j;

    std::ifstream jfile( jsonns::configFile );

    if( !jfile )
    {
        std::cerr << "Open PEF json file fail!!\n";
        return -1;
    }

    jfile >> j;
    jfile.close();

    // struct back to json
    for ( int i = 0; i < MAX_PEF_EVENT_ENTRIES; i++ ) {
        j[jsonns::TABLE_NAME][i] = eventFilterTable[i];
    }

    std::ofstream ojfile( jsonns::configFile );

    ojfile << std::setw(4) << j << "\n";
    ojfile.close();

    return 0;
}

ipmi::RspType<uint8_t, // PEF Version
              uint8_t, // PEF Supported
              uint8_t> // Number of event filter table entries (1 based)
     ipmiGetPefCapabilities()
{
    typedef union {
        uint8_t byte;
        struct {
            uint8_t Alert         : 1;
            uint8_t PowerDown     : 1;
            uint8_t Reset         : 1;
            uint8_t PowerCycle    : 1;
            uint8_t OemAction     : 1;
            uint8_t DiagInterrupt : 1;
            uint8_t Reserved      : 1;
            uint8_t OemEventFilterSupport : 1;
        } bits;
    } capabilities;
    capabilities SupportActions;

    constexpr uint8_t pefVer = 0x51;

    SupportActions.byte = 0;

    SupportActions.bits.PowerDown = true;
    SupportActions.bits.Reset = true;
    SupportActions.bits.PowerCycle = true;

    uint8_t pefSupported = static_cast<uint8_t>(SupportActions.byte);

    constexpr uint8_t numEventFilter = 20;

    return ipmi::responseSuccess(pefVer, pefSupported, numEventFilter);
}

template <size_t N, typename T>
void copyInto(T& t, const std::array<uint8_t, N>& bytes)
{
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(N == sizeof(T));
    std::memcpy(&t, bytes.data(), bytes.size());
}

ipmi::RspType<> 
     ipmiSetPefConfig(Context::ptr ctx,
                      uint8_t parameter, 
                      message::Payload& req)
{
    switch (static_cast<PefParam>(parameter))
    {
        case PefParam::SetInProgress:
        {
            uint2_t flag;
            uint6_t rsvd;
            if (req.unpack(flag, rsvd) != 0 || !req.fullyUnpacked())
            {
                return responseReqDataLenInvalid();
            }
            if (rsvd)
            {
                return responseInvalidFieldRequest();
            }
            auto status = static_cast<Progress>(static_cast<uint8_t>(flag));
            switch (status)
            {
                case Progress::SetComplete:
                {
                    ProgressStatus = static_cast<Progress>(status);

                    // Restart PEF Service
                    system("systemctl restart phoenix-dbus-monitor.service");

                    return responseSuccess();
                }
                case Progress::SetInProgress:
                {
                    if (ProgressStatus == static_cast<Progress>(Progress::SetInProgress))
                    {
                        return response(ccParamSetLocked);
                    }
                    ProgressStatus = static_cast<Progress>(status);
                    return responseSuccess();
                }
                case Progress::CommitWrite:
                {
                    if (ProgressStatus != static_cast<Progress>(Progress::SetInProgress))
                    {
                        return responseInvalidFieldRequest();
                    }
                    ProgressStatus = static_cast<Progress>(status);

                    // Restart PEF Service
                    system("systemctl restart phoenix-dbus-monitor.service");

                    return responseSuccess();
                }
            }
            break;
        }
        case PefParam::PEFCtrl:
        {
            uint8_t ctrl;
            if (req.unpack(ctrl) != 0 || !req.fullyUnpacked())
            {
                return responseReqDataLenInvalid();
            }

            if (getPefGlobalConfigure() != 0)
            {
                return responseCommandNotAvailable();
            }

            globalConfigTable.control = ctrl;

            setPefGlobalConfigure();

            return responseSuccess();
        }
        case PefParam::PEFActionGlobalCtrl:
        {
            uint8_t globlaCtrl;
            if (req.unpack(globlaCtrl) != 0 || !req.fullyUnpacked())
            {
                return responseReqDataLenInvalid();
            }

            if (getPefGlobalConfigure() != 0)
            {
                return responseCommandNotAvailable();
            }

            globalConfigTable.actionGlobalControl = globlaCtrl;

            setPefGlobalConfigure();

            return responseSuccess();
        }
        case PefParam::EventFilterTable:
        {
            uint8_t selector;
            bool matchSelector = false;

            std::array<uint8_t, sizeof(jsonns::eventFilter)> bytes;
            if (req.unpack(bytes) != 0 || !req.fullyUnpacked())
            {
                return responseReqDataLenInvalid();
            }

            selector = bytes[0];

            if (selector < 1 || 
                selector > MAX_PEF_EVENT_ENTRIES)
            {
                return responseInvalidFieldRequest();
            }

            // find exist event filter
            for ( int i = 0; i < MAX_PEF_EVENT_ENTRIES; i++ ) {
                if (selector != eventFilterTable[i].selector)
                {
                    continue;
                }
                matchSelector = true;
                copyInto(eventFilterTable[i], bytes);
                break;
            }

            if (matchSelector == false)
            {
                // find empty event filter
                for ( int i = 0; i < MAX_PEF_EVENT_ENTRIES; i++ ) {
                    if (eventFilterTable[i].selector == 0x00)
                    {
                        copyInto(eventFilterTable[i], bytes);
                        break;
                    }
                }
            }

            setPefEventFilterTable();

            return responseSuccess();
        }
        case PefParam::EventFilterTableData1:
        {
            uint8_t selector, filterConfig;
            bool matchSelector = false;

            std::array<uint8_t, 2> bytes;
            if (req.unpack(bytes) != 0 || !req.fullyUnpacked())
            {
                return responseReqDataLenInvalid();
            }

            selector = bytes[0];
            filterConfig = bytes[1]; //Filter Configuration

            if (selector < 1 || 
                selector > MAX_PEF_EVENT_ENTRIES)
            {
                return responseInvalidFieldRequest();
            }

            // find exist event filter
            for ( int i = 0; i < MAX_PEF_EVENT_ENTRIES; i++ ) {
                if (selector != eventFilterTable[i].selector)
                {
                    continue;
                }
                matchSelector = true;
                eventFilterTable[i].selector = selector;
                eventFilterTable[i].filterConfig = filterConfig;
                break;
            }

            if (matchSelector == false)
            {
                // find empty event filter
                for ( int i = 0; i < MAX_PEF_EVENT_ENTRIES; i++ ) {
                    if (eventFilterTable[i].selector == 0x00)
                    {
                        eventFilterTable[i].selector = selector;
                        eventFilterTable[i].filterConfig = filterConfig;
                        break;
                    }
                }
            }

            setPefEventFilterTable();

            return responseSuccess();
        }

        case PefParam::AlertPolicyTable:
        {
            uint8_t selector, dest_channel;
            uint4_t policyNum;
            uint1_t enable, eventSpecific;
            uint3_t policy;
            uint7_t alertStringSet;
            if (req.unpack(selector, policy, enable, policyNum
                    , dest_channel, alertStringSet, eventSpecific) != 0 || !req.fullyUnpacked())
            {
                return responseReqDataLenInvalid();
            }

            if (selector < 1 || 
                selector > MAX_PEF_ALERT_POLICY_ENTRIES)
            {
                return responseInvalidFieldRequest();
            }
            
            int existEntriesSize = getNumberOfAlertPolicyEntries();
            if (selector > existEntriesSize)
            {
                return responseInvalidFieldRequest();
            }

            if (getPefAlertPolicyTable() != 0)
            {
                return responseCommandNotAvailable();
            }

            alert_policy_table[selector].policyNumber = static_cast<uint8_t>(policyNum);
            alert_policy_table[selector].enable = static_cast<uint8_t>(enable);
            alert_policy_table[selector].policy = static_cast<uint8_t>(policy);
            alert_policy_table[selector].channel = (dest_channel & 0xf0) >> 4;
            alert_policy_table[selector].destination = (dest_channel & 0x0f);
            alert_policy_table[selector].is_event_specific = static_cast<uint8_t>(eventSpecific);
            alert_policy_table[selector].alertSrtingKey = static_cast<uint8_t>(alertStringSet);

            setPefAlertPolicyTable(selector);

            return responseSuccess();
        }
    }

    return response(ccParamNotSupported);
}

template <typename T>
std::string_view dataRef(const T& t)
{
    static_assert(std::is_trivially_copyable_v<T>);
    return {reinterpret_cast<const char*>(&t), sizeof(T)};
}

ipmi::RspType<message::Payload> 
     ipmiGetPefConfig(Context::ptr ctx,
                      uint8_t parameter, 
                      uint8_t set, 
                      uint8_t block)
{
    message::Payload resp;
    bool getRevOnly;
    constexpr uint8_t parameterRev = 0x11;
    resp.pack(parameterRev);

    getRevOnly = (parameter & 0x7f) >> 15;
    if (getRevOnly)
    {
        return responseSuccess(std::move(resp));
    }

    parameter &= 0x7f;
    switch (static_cast<PefParam>(parameter))
    {
        case PefParam::SetInProgress:
        {
            resp.pack(static_cast<uint8_t>(ProgressStatus));
            return responseSuccess(std::move(resp));
        }
        case PefParam::PEFCtrl:
        {
            if (getPefGlobalConfigure() != 0)
            {
                return responseCommandNotAvailable();
            }

            resp.pack(globalConfigTable.control);
            return responseSuccess(std::move(resp));
        }
        case PefParam::PEFActionGlobalCtrl:
        {
            if (getPefGlobalConfigure() != 0)
            {
                return responseCommandNotAvailable();
            }

            resp.pack(globalConfigTable.actionGlobalControl);
            return responseSuccess(std::move(resp));
        }
        case PefParam::EventFilterTable:
        {
            if (set < 1 || 
                set > MAX_PEF_EVENT_ENTRIES || 
                block != 0)
            {
                return responseInvalidFieldRequest();
            }
            if (getPefEventFilterTable() != 0)
            {
                return responseCommandNotAvailable();
            }
            for ( int i = 0; i < MAX_PEF_EVENT_ENTRIES; i++ ) {

                if (set != eventFilterTable[i].selector)
                {
                    continue;
                }
                resp.pack(dataRef(eventFilterTable[i]));
                return responseSuccess(std::move(resp));
            }
            return responseInvalidFieldRequest();
        }

        case PefParam::EventFilterTableData1:
        {
            if (set < 1 || 
                set > MAX_PEF_EVENT_ENTRIES || 
                block != 0)
            {
                return responseInvalidFieldRequest();
            }
            if (getPefEventFilterTable() != 0)
            {
                return responseCommandNotAvailable();
            }

            for ( int i = 0; i < MAX_PEF_EVENT_ENTRIES; i++ ) {
                if (set != eventFilterTable[i].selector)
                {
                    continue;
                }
                resp.pack(eventFilterTable[i].selector);
                resp.pack(eventFilterTable[i].filterConfig);
                return responseSuccess(std::move(resp));
            }
            return responseInvalidFieldRequest();
        }

        case PefParam::NumberOfAlertPolicyEntries:
        {
            /* Skip 0 reserved to Alert Immediate command */
            uint8_t size = getNumberOfAlertPolicyEntries()-1;
            resp.pack(size);
            return responseSuccess(std::move(resp));
        }

        case PefParam::AlertPolicyTable:
        {
            /* Skip 0 reserved to Alert Immediate command */
            if (set < 1 || 
                set > getNumberOfAlertPolicyEntries()-1 || 
                block != 0)
            {
                return responseInvalidFieldRequest();
            }
            if (getPefAlertPolicyTable() != 0)
            {
                return responseCommandNotAvailable();
            }

            uint8_t policyNum = (alert_policy_table[set].policyNumber << 4) & 0xf0;
            policyNum |= (alert_policy_table[set].enable << 3);
            policyNum |= alert_policy_table[set].policy;
            resp.pack(policyNum);

            uint8_t channnlDest = (alert_policy_table[set].channel << 4) & 0xf0;
            channnlDest |= (alert_policy_table[set].destination & 0x0f);
            resp.pack(channnlDest);

            uint8_t alertStringKey = (alert_policy_table[set].is_event_specific << 7);
            alertStringKey |= (alert_policy_table[set].alertSrtingKey & 0x7f);
            resp.pack(alertStringKey);

            return responseSuccess(std::move(resp));
        }
    }

    return response(ccParamNotSupported);
}

void registerPefFunctions()
{
    // <Get PEF Capabilities>
    ipmi::registerHandler(ipmi::prioOdmBase, 
                          ipmi::netFnSensor,
                          ipmi::sensor_event::cmdGetPefCapabilities,
                          ipmi::Privilege::User,
                          ipmiGetPefCapabilities);

    // <Set PEF Configuration Parameters>
    ipmi::registerHandler(ipmi::prioOdmBase,
                          ipmi::netFnSensor,
                          ipmi::sensor_event::cmdSetPefConfigurationParams,
                          ipmi::Privilege::Admin,
                          ipmiSetPefConfig);

    // <Get PEF Configuration Parameters>
    ipmi::registerHandler(ipmi::prioOdmBase,
                          ipmi::netFnSensor,
                          ipmi::sensor_event::cmdGetPefConfigurationParams,
                          ipmi::Privilege::Operator,
                          ipmiGetPefConfig);
}
} // namespace pef
} // namespace ipmi
