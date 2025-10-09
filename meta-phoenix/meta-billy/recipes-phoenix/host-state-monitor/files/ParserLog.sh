#!/bin/bash

declare -A LOG_MAP
LOG_MAP["/tmp/vr_controller.log"]="/var/log/vr_controller_event.log"
LOG_MAP["/tmp/voltage_dump.log"]="/var/log/voltage_dump_event.log"

TRIGGER=$(date +"%Y-%m-%d %H:%M:%S")
echo "Trigger timestamp: $TRIGGER"

TRIGGER_SEC=$(date -d "$TRIGGER" +%s)
START_SEC=$((TRIGGER_SEC - 120))
END_SEC=$((TRIGGER_SEC + 120))

START_TIME=$(date -d "@$START_SEC" +"%Y-%m-%d %H:%M:%S")
END_TIME=$(date -d "@$END_SEC" +"%Y-%m-%d %H:%M:%S")

echo "Time range: $START_TIME to $END_TIME"
echo "Waiting 2 minutes..."

sleep 120

for SOURCE_LOG in "${!LOG_MAP[@]}"; do
    OUTPUT_LOG="${LOG_MAP[$SOURCE_LOG]}"
    
    echo "Processing: $SOURCE_LOG -> $OUTPUT_LOG"

    if [[ ! -f "$SOURCE_LOG" ]]; then
        echo "Warning: $SOURCE_LOG not found, skipping..."
        continue
    fi
    
    > "$OUTPUT_LOG"
    echo "========================================" >> "$OUTPUT_LOG"
    echo "Event at: $TRIGGER" >> "$OUTPUT_LOG"
    echo "Time range: $START_TIME to $END_TIME" >> "$OUTPUT_LOG"
    echo "========================================" >> "$OUTPUT_LOG"
    
    COUNT=0
    while IFS= read -r line; do
        TIMESTAMP=$(echo "$line" | awk '{print $1" "$2}' | cut -d'.' -f1)
        
        LOG_SEC=$(date -d "$TIMESTAMP" +%s 2>/dev/null)
        
        if [[ -n "$LOG_SEC" ]] && (( LOG_SEC >= START_SEC && LOG_SEC <= END_SEC )); then
            echo "$line" >> "$OUTPUT_LOG"
            ((COUNT++))
        fi
    done < "$SOURCE_LOG"
    
    echo "========================================" >> "$OUTPUT_LOG"
    echo "Total logs extracted: $COUNT" >> "$OUTPUT_LOG"
    echo "========================================" >> "$OUTPUT_LOG"
    
    echo "  -> Total logs extracted: $COUNT"
done

echo "All logs extraction completed."