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

enum BMC_UPDATE_STATE {
    PHOENIX_BMC_UPDATE_STATE_NOT_INITIALIZED,
    PHOENIX_BMC_UPDATE_STATE_NONE,
    PHOENIX_BMC_UPDATE_STATE_COMPLETED,
};

bool api_is_last_bmc_updated (void);



