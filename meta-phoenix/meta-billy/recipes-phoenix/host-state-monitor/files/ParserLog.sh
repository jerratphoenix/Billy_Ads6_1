#!/bin/sh
# POSIX /bin/sh：兩檔輪替 (先建 _1，再建 _2，之後刪最舊、_2→_1、再建新 _2)
set -e
# 清除可能的 BOM (若有)
first_line=$(head -c 3 "$0" | od -An -tx1 | tr -d ' ')
if [ "$first_line" = "efbbbf" ]; then
    echo "[Warning] Script contains BOM, please re-save without BOM."
fi
MAPPINGS="
/tmp/voltage_dump.log|/var/log/voltage_dump_event
/tmp/vr_controller.log|/var/log/vr_controller_event
"
TRIGGER="$(date +"%Y-%m-%d %H:%M:%S")"
echo "Trigger timestamp: $TRIGGER"
TRIGGER_SEC="$(date -d "$TRIGGER" +%s)"
START_SEC=$((TRIGGER_SEC - 120))
END_SEC=$((TRIGGER_SEC + 120))
START_TIME="$(date -d "@$START_SEC" +"%Y-%m-%d %H:%M:%S")"
END_TIME="$(date -d "@$END_SEC" +"%Y-%m-%d %H:%M:%S")"
echo "Time range: $START_TIME to $END_TIME"
echo "Waiting 2 minutes..."
sleep 120
echo "$MAPPINGS" | while IFS='|' read -r SOURCE_LOG BASE_OUTPUT; do
    [ -n "$SOURCE_LOG" ] || continue
    if [ ! -f "$SOURCE_LOG" ]; then
        echo "Warning: $SOURCE_LOG not found, skipping..."
        continue
    fi
    ONE="${BASE_OUTPUT}_1.log"
    TWO="${BASE_OUTPUT}_2.log"
    # --- Rotation logic ---
    if [ -f "$ONE" ] && [ -f "$TWO" ]; then
        echo "[Rotate] remove _1, rename _2→_1, create new _2"
        rm -f "$ONE"
        mv -f "$TWO" "$ONE"
        NEXT="$TWO"
    elif [ -f "$ONE" ] && [ ! -f "$TWO" ]; then
        echo "[Rotate] only _1 exists, create new _2"
        NEXT="$TWO"
    elif [ ! -f "$ONE" ] && [ -f "$TWO" ]; then
        echo "[Recover] only _2 exists, rename to _1, create new _2"
        mv -f "$TWO" "$ONE"
        NEXT="$TWO"
    else
        echo "[Init] no previous logs, create _1"
        NEXT="$ONE"
    fi
    echo "Processing: $SOURCE_LOG -> $NEXT"
    TMP="${NEXT}.tmp"
    : > "$TMP"
    {
        echo "========================================"
        echo "Event at: $TRIGGER"
        echo "Time range: $START_TIME to $END_TIME"
        echo "========================================"
    } >> "$TMP"
    COUNT=0
    while IFS= read -r line; do
        DATE_PART=$(echo "$line" | awk '{print $1}')
        TIME_FULL=$(echo "$line" | awk '{print $2}')
        
        TIME_FULL=$(echo "$TIME_FULL" | cut -d'-' -f1)
        
        TIME_SEC=$(echo "$TIME_FULL" | cut -d'.' -f1)
        [ -n "$DATE_PART" ] || continue
        [ -n "$TIME_SEC" ] || continue
        LOG_SEC=$(date -d "$DATE_PART $TIME_SEC" +%s 2>/dev/null || true)
        [ -n "$LOG_SEC" ] || continue
        if [ "$LOG_SEC" -ge "$START_SEC" ] && [ "$LOG_SEC" -le "$END_SEC" ]; then
            echo "$line" >> "$TMP"
            COUNT=$((COUNT + 1))
        fi
    done < "$SOURCE_LOG"
    {
        echo "========================================"
        echo "Total logs extracted: $COUNT"
        echo "========================================"
    } >> "$TMP"
    mv -f "$TMP" "$NEXT" || {
        echo "[Error] rename failed for $TMP → $NEXT"
    }
    sync
    echo "  -> Total logs extracted: $COUNT"
done
echo "All logs extraction completed."