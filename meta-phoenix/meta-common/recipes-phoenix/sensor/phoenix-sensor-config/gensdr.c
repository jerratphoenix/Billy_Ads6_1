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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <gensdr.h>

static int parseRecord(json_object *jsonRecord, char *recordRaw)
{
    json_object *jsonObj;

    const char **keyString, *keyValue;
    int offset = 0, keyNum = 0;
    int recordSize = 0;
    const char **sdr_string;
    int recordTypeValue, i;

    json_object_object_get_ex (jsonRecord, "RecordType", &jsonObj);
    recordTypeValue = strtoul(json_object_get_string(jsonObj), NULL, 0);

    switch (recordTypeValue) {
    case 0x01:
        sdr_string = sdr_rec_01;
		keyNum = 45;
        break;
    case 0x02:
        sdr_string = sdr_rec_02;
		keyNum = 28;
        break;
    case 0x03:
        sdr_string = sdr_rec_03;
		keyNum = 16;
        break;
    case 0x11:
        sdr_string = sdr_rec_11;
		keyNum = 16;
        break;
    case 0x12:
        sdr_string = sdr_rec_12;
		keyNum = 16;
        break;
    case 0xc0:
        sdr_string = sdr_rec_c0;
		keyNum = 15;
        break;
    default:
        printf("Unsupport recordTypeValue = 0x%02x\n", recordTypeValue);
        return 0;
    }

    json_object_object_get_ex (jsonRecord, "RecordLength", &jsonObj);

    // sdr header + body
    recordSize = 5 + strtoul(json_object_get_string(jsonObj), NULL, 0);

    // adjust pad
    if ((recordSize % 16) > 0) {
        recordSize = recordSize + 16 - (recordSize % 16);
    }

    for (i=0, keyString = sdr_string; i< keyNum; i++, keyString++) {
        if ( !json_object_object_get_ex (jsonRecord, *keyString, &jsonObj) )
            continue;

        keyValue = json_object_get_string(jsonObj);

        if (strncmp (keyValue, "0x",2) == 0) {
            if (strlen(keyValue) <= 4) {
                recordRaw[offset] = strtoul(keyValue, NULL, 0);
                offset = offset + 1;
            } else {
                recordRaw[offset] = strtoul(keyValue, NULL, 0) % 256;
                recordRaw[offset + 1] = strtoul(keyValue, NULL, 0) / 256;
                offset = offset + 2;
            }
        } else { // string value
            strncpy (&recordRaw[offset], keyValue, strlen(keyValue));
        }
    }

    return recordSize;
}

static int parseSdrJsonFile(char *content, char *filename)
{
    json_object *jsonSDRInfo;
    char recordData[256];
    FILE *fp;
    int i, recordSize;

    fp = fopen (filename, "wb+");
    if (!fp) {
        printf ("Error opening file: %s\n", filename);
        return -1;
    }

    json_object_object_get_ex (json_tokener_parse(content), "SDRRepository", &jsonSDRInfo);

    for (i = 0; i < json_object_array_length(jsonSDRInfo); i++) {
        memset(recordData, 0, 256);

        recordSize = parseRecord (json_object_array_get_idx(jsonSDRInfo, i),
                                  recordData);

        if (recordSize <= 0)
            continue;

        fwrite(recordData, recordSize, 1, fp);
    }

    fclose (fp);

    return 0;
}

int main (int argc, char*argv[])
{
    FILE *fp;
    char *data;
    long fileSize;
    int n;

    if(argc != 3) {
        printf("Usage : %s JsonFile OutputFile\n", argv[0]);
        return -1;
    }

    fp = fopen (argv[1], "rb");
    if (!fp) {
        return -1;
    }

    fseek (fp, 0, SEEK_END);
    fileSize = ftell (fp);
    rewind(fp);

    data = (char *) malloc (fileSize);
    if (!data) {
        fclose (fp);
        return -1;
    }

    n = fread (data, sizeof (char), fileSize, fp);
    fclose (fp);

    if (parseSdrJsonFile(data, argv[2]) != 0) {
        printf("Fail\n");
    }

    free (data);

    return 0;
}
