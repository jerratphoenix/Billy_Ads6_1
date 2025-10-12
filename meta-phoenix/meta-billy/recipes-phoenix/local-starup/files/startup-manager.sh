#!/bin/bash

PIDS_DIR="/var/startup/.pids"

start() {
    echo "[local-startup] Starting all scripts in /var/startup"
    
    mkdir -p "$PIDS_DIR"
    
    for f in /var/startup/*; do
        if [ -x "$f" ]; then
            echo "[local-startup] Running $f"
            "$f" &
            pid=$!
            echo $pid > "$PIDS_DIR/$(basename "$f").pid"
        fi
    done
    
    echo "[local-startup] All scripts started"
}

stop() {
    echo "[local-startup] Stopping all scripts"

    if [ -d "$PIDS_DIR" ]; then
        for pidfile in "$PIDS_DIR"/*.pid; do
            if [ -f "$pidfile" ]; then
                pid=$(cat "$pidfile")
                script_name=$(basename "$pidfile" .pid)
                
                if kill -0 $pid 2>/dev/null; then
                    echo "[local-startup] Stopping $script_name (PID: $pid)"
                    kill $pid 2>/dev/null || true

                    for i in {1..50}; do
                        if ! kill -0 $pid 2>/dev/null; then
                            break
                        fi
                        sleep 0.1
                    done
                    
                    if kill -0 $pid 2>/dev/null; then
                        echo "[local-startup] Force killing $script_name (PID: $pid)"
                        kill -9 $pid 2>/dev/null || true
                    fi
                fi
                
                rm -f "$pidfile"
            fi
        done
        
        rmdir "$PIDS_DIR" 2>/dev/null || true
    fi
    
    echo "[local-startup] All scripts stopped"
}

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    *)
        echo "Usage: $0 {start|stop}"
        exit 1
        ;;
esac

exit 0