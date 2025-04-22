# main-fregio.py
# Plasma System-AI: Telemetry Collector and AI Engine Entry Point

import os
import psutil
import time
import json
import argparse
from datetime import datetime

# Collect system metrics
def collect_system_metrics():
    metrics = {
        "timestamp": datetime.now().isoformat(),
        "cpu_percent": psutil.cpu_percent(interval=1),
        "cpu_freq": psutil.cpu_freq()._asdict() if psutil.cpu_freq() else {},
        "virtual_memory": psutil.virtual_memory()._asdict(),
        "swap_memory": psutil.swap_memory()._asdict(),
        "disk_usage": psutil.disk_usage('/')._asdict(),
        "battery": psutil.sensors_battery()._asdict() if psutil.sensors_battery() else {},
        "temperatures": {},
        "process_count": len(psutil.pids())
    }

    try:
        temps = psutil.sensors_temperatures()
        for name, entries in temps.items():
            metrics["temperatures"][name] = [entry._asdict() for entry in entries]
    except Exception as e:
        metrics["temperatures"] = {"error": str(e)}

    return metrics

# Save to file in ~/.ai_logs/
def save_metrics_to_file(metrics, filename="~/.ai_logs/system_metrics.json"):
    full_path = os.path.expanduser(filename)
    directory = os.path.dirname(full_path)
    os.makedirs(directory, exist_ok=True)
    
    # Write only once by opening the file in write mode ("w") instead of append mode ("a")
    with open(full_path, "w") as f:  # Change mode to "w" for writing once
        f.write(json.dumps(metrics) + "\n")

# Main function with duration and delay
def main():
    parser = argparse.ArgumentParser(description="Plasma System-AI - System Telemetry Logger")
    parser.add_argument('--duration', type=int, default=60, help='Total logging time in seconds (default: 60)')
    parser.add_argument('--delay', type=int, default=0, help='Initial delay before logging starts (default: 0)')
    args = parser.parse_args()

    print("[INFO] Plasma System-AI is starting...")
    if args.delay > 0:
        print(f"[INFO] Waiting {args.delay} seconds before starting...")
        time.sleep(args.delay)

    # Collect and save metrics only once
    data = collect_system_metrics()
    save_metrics_to_file(data)
    print(f"[+] Logged: {data['timestamp']} | CPU: {data['cpu_percent']}% | RAM: {data['virtual_memory']['percent']}%")

if __name__ == "__main__":
    main()
