#!/bin/sh
set -e

# using: ./collect_vr_logs.sh <tag>
# ex: ./collect_vr_logs.sh crashdump
# or    ./collect_vr_logs.sh poweroff

TAG="$1"

MAPPINGS="
/tmp/voltage_dump.log|/var/log/voltage_dump_event
/tmp/vr_controller.log|/var/log/vr_controller_event
"

TRIGGER="$(date +"%Y-%m-%d %H:%M:%S")"
TRIGGER_SEC="$(date -d "$TRIGGER" +%s)"
START_SEC=$((TRIGGER_SEC - 120))
END_SEC=$((TRIGGER_SEC + 120))
START_TIME="$(date -d "@$START_SEC" +"%Y-%m-%d %H:%M:%S")"
END_TIME="$(date -d "@$END_SEC" +"%Y-%m-%d %H:%M:%S")"

echo "Trigger: $TRIGGER"
echo "Range: $START_TIME to $END_TIME"
echo "Waiting 2 minutes..."
sleep 120

while IFS='|' read -r SOURCE_LOG BASE_OUTPUT; do
    [ -n "$SOURCE_LOG" ] || continue
    [ -f "$SOURCE_LOG" ] || continue

    ONE="${BASE_OUTPUT}_${TAG}_1.log"
    TWO="${BASE_OUTPUT}_${TAG}_2.log"

    if [ -f "$ONE" ]; then
        mv -f "$ONE" "$TWO"
    fi
    NEXT="$ONE"
    TMP="${NEXT}.tmp"

    {
        echo "========================================"
        echo "[$TAG] Event at: $TRIGGER"
        echo "Time range: $START_TIME to $END_TIME"
        echo "========================================"
    } > "$TMP"

    awk -v s="$START_TIME" -v e="$END_TIME" '{
        t=$1" "$2; sub(/\.[0-9]+-.*$/,"",t)
        if(t<s)next; if(t>e)exit; print
    }' "$SOURCE_LOG" >> "$TMP"

    COUNT=$(grep -c "^[0-9]" "$TMP" || echo 0)

    {
        echo "========================================"
        echo "[$TAG] Total logs extracted: $COUNT"
        echo "========================================"
    } >> "$TMP"

    mv -f "$TMP" "$NEXT"
    echo "[$TAG] $SOURCE_LOG -> $NEXT ($COUNT logs)"
done <<EOF
$MAPPINGS
EOF
