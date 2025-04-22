import argparse
import time
import subprocess
from datetime import datetime
from writereadout_tens import (
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

def parse_arguments():
    """
    Parse command-line arguments.
    """
    parser = argparse.ArgumentParser(description="🧠 Plasma System AI Monitor")

    parser.add_argument("--interval", "-i", type=int, help="Monitoring interval in seconds", default=60)
    parser.add_argument("--auto-prop", "-a", action="store_true", help="Enable automatic process management")
    parser.add_argument("--analyze", "-z", action="store_true", help="Analyze latest system log entry and display summary")
    parser.add_argument("--most-intensive", "-m", action="store_true", help="Show most resource-intensive process")
    parser.add_argument("--get-interval", choices=["s", "m"], help="Time unit: s for seconds, m for minutes")
    parser.add_argument("--suggest", "-s", action="store_true", help="Suggest actions for current top processes")
    parser.add_argument("--loop", "-l", action="store_true", help="Run system monitoring loop")

    return parser.parse_args()

def main():
    # Parse command-line arguments
    args = parse_arguments()

    print("[AI Engine - TensorFlow] Analyzing system...\n")

    # Ensure log directory is created
    log_path = ensure_log_directory()
    processes = get_process_usages()

    # Handle --get-interval and --interval together
    if args.get_interval:
        if args.interval <= 0:
            print("❌ Invalid interval. Must be a positive integer.")
            exit(1)
        
        unit = args.get_interval.strip().lower()
        if unit == "m":
            args.interval = args.interval * 60  # Convert minutes to seconds
        elif unit != "s":
            print("❌ Invalid time unit. Use 's' for seconds or 'm' for minutes.")
            exit(1)

        print(f"⏱ Monitoring interval set to {args.interval} seconds (via --get-interval {unit}).")
    
    elif args.interval != 60:
        print("⚠  Warning: Default interval of 60 seconds will be used.")
    
    # Analyze latest system log entry if --analyze is set
    if args.analyze:
        result = read_and_analyze()
        print(f"\n👉 AI Suggestion: {result}\n")
        subprocess.run(["notify-send", "Plasma System AI", result])
    else:
        result = "No analysis run."

    # Show most resource-intensive processes if --most-intensive is set
    if args.most_intensive:
        print("📌 [Top 5 Resource-Intensive Processes]")
        for proc in processes[:5]:
            print(f"🔍 {proc['name']} | CPU: {proc['cpu_percent']}% | RAM: {proc['memory_percent']:.2f}%")

    # Smart suggestions if --suggest is set
    if args.suggest:
        print("\n💡 [Smart Suggestions Based on Application Usage]")
        app_suggestions = suggest_app_actions(processes[:10])
        for s in app_suggestions:
            print(s)
    else:
        app_suggestions = []

    # Enable auto-prop if --auto-prop is set
    if args.auto_prop:
        print("⚙ Auto-Prop is active.")
        auto_prop_manage(processes, permission=True)

    # Log results to file
    log_results(log_path, result, app_suggestions)

    # Start monitoring loop if --loop is set
    if args.loop:
        print(f"\n🔁 Starting monitor loop every {args.interval} seconds...\n")
        while True:
            run_monitor_loop(args)  # Pass the 'args' argument to the function
            time.sleep(args.interval)
    else:
        print(f"🕒 Interval set to {args.interval} seconds (but loop not started, use --loop to activate).")

if __name__ == "__main__":
    main()
