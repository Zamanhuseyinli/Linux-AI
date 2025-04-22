import argparse
import time
import subprocess
from datetime import datetime
from plasma_ai_modules import (
    read_and_analyze,
    get_process_usages,
    suggest_app_actions,
    most_intensive_process,
    auto_prop_manage,
    ensure_log_directory,
    run_monitor_loop,
    get_interval_from_user
)

def log_results(log_path, result, suggestions):
    with open(log_path, "a") as log:
        log.write(f"[{datetime.now()}] Suggestion: {result}\n")
        for s in suggestions:
            log.write(f"- {s}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="🧠 Plasma System AI Monitor")

    parser.add_argument("--interval", "-i", type=int, help="Monitoring interval in seconds", default=60)
    parser.add_argument("--auto-prop", "-a", action="store_true", help="Enable automatic process management")
    parser.add_argument("--analyze", "-z", action="store_true", help="Analyze latest system log entry and display summary")
    parser.add_argument("--most-intensive", "-m", action="store_true", help="Show most resource-intensive process")
    parser.add_argument("--get-interval", choices=["s", "m"], help="Time unit: s for seconds, m for minutes")
    parser.add_argument("--suggest", "-s", action="store_true", help="Suggest actions for current top processes")
    parser.add_argument("--loop", "-l", action="store_true", help="Run system monitoring loop")

    args = parser.parse_args()

    print("[AI Engine - TensorFlow] Analyzing system...\n")

    log_path = ensure_log_directory()
    processes = get_process_usages()
# Override interval if get-interval and interval are both provided
if args.get_interval and args.interval:
    if args.interval <= 0:
        print("❌ Invalid interval. Must be a positive integer.")
        exit(1)
    unit = args.get_interval.strip().lower()
    if unit == "m":
        args.interval = args.interval * 60
    elif unit != "s":
        print("❌ Invalid time unit. Use 's' for seconds or 'm' for minutes.")
        exit(1)
    print(f"⏱ Monitoring interval set to {args.interval} seconds (via --get-interval {unit}).")

elif args.get_interval or (args.interval != 60 and not args.get_interval):
    print("⚠  Warning: To use custom time unit, provide both --get-interval and --interval. Defaulting to seconds.")

# 👇 Bu satırdan sonrası elif bloğunun dışına çıkmalı
if args.analyze:
    result = read_and_analyze()
    print(f"\n👉 AI Suggestion: {result}\n")
    subprocess.run(["notify-send", "Plasma System AI", result])
else:
    result = "No analysis run."

if args.most_intensive:
    print("📌 [Top 5 Resource-Intensive Processes]")
    for proc in processes[:5]:
        print(f"🔍 {proc['name']} | CPU: {proc['cpu_percent']}% | RAM: {proc['memory_percent']:.2f}%")

if args.suggest:
    print("\n💡 [Smart Suggestions Based on Application Usage]")
    app_suggestions = suggest_app_actions(processes[:10])
    for s in app_suggestions:
        print(s)
else:
    app_suggestions = []

if args.auto_prop:
    print("⚙ Auto-Prop is active.")
    auto_prop_manage(processes, permission=True)

log_results(log_path, result, app_suggestions)

if args.loop:
    print(f"\n🔁 Starting monitor loop every {args.interval} seconds...\n")
    while True:
        run_monitor_loop()
        time.sleep(args.interval)
else:
    print(f"🕒 Interval set to {args.interval} seconds (but loop not started, use --loop to activate).")
