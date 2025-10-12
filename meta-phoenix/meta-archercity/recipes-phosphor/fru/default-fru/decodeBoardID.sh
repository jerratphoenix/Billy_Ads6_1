#!/bin/bash

# this script uses the BOARD_ID set from checkFru.sh and provides the NAME,
# PRODID, and EEPROM_FRU values for this platform
decode_board_id() {
    if grep -q 'CPU part\s*: 0xc07' /proc/cpuinfo; then
        # AST2600
        case $BOARD_ID in
            55) NAME="XPV2S"
                PRODID="0xad";;
            57) NAME="XPV"
                PRODID="0xa3";;
            61) NAME="ArcherCityM"
                PRODID="0xa2";;
            62) NAME="ArcherCity"
                PRODID="0x9c";;
            *)  NAME="AST2600EVB"
                PRODID="0x00";;
        esac

    fi
}
