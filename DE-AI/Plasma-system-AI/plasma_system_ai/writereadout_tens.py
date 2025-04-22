import logging
import time
import json
import tensorflow as tf
import numpy as np
import psutil
import os
from datetime import datetime
import subprocess
import argparse

def extract_features(entry):
    cpu = entry.get("cpu_percent", 0)
    ram = entry.get("virtual_memory", {}).get("percent", 0)
    swap = entry.get("swap_memory", {}).get("percent", 0)
    battery = entry.get("battery", {}).get("percent", 100)

    temp_list = []
    temps = entry.get("temperatures", {})
    for sensors in temps.values():
        if isinstance(sensors, list):
            for sensor in sensors:
                temp = sensor.get("current", 0)
                temp_list.append(temp)
    avg_temp = np.mean(temp_list) if temp_list else 0

    net_sent = entry.get("net_io", {}).get("bytes_sent", 0)
    net_recv = entry.get("net_io", {}).get("bytes_recv", 0)
    disk_read = entry.get("disk_io", {}).get("read_bytes", 0)
    disk_write = entry.get("disk_io", {}).get("write_bytes", 0)
    process_count = entry.get("process_count", 0)
    uptime = entry.get("uptime", 0)

    return np.array([[
        cpu,
        ram,
        swap,
        battery,
        avg_temp,
        net_sent / 1024 / 1024,
        net_recv / 1024 / 1024,
        disk_read / 1024 / 1024,
        disk_write / 1024 / 1024,
        process_count,
        uptime / 60
    ]], dtype=np.float32)


def create_model():
    model = tf.keras.Sequential([
        tf.keras.layers.Dense(16, activation='relu', input_shape=(11,)),
        tf.keras.layers.Dense(10, activation='relu'),
        tf.keras.layers.Dense(3, activation='softmax')
    ])
    model.compile(optimizer='adam', loss='sparse_categorical_crossentropy', metrics=['accuracy'])
    return model


suggestions_en = {
    0: "✅ System stable. Everything looks good.",
    1: "⚠ High resource usage detected. Consider cleaning or restarting.",
    2: "🔥 Overheating detected. Thermal check is recommended."
}


def analyze_with_model(entry, model):
    features = extract_features(entry)
    prediction = model.predict(features, verbose=0)[0]
    label = np.argmax(prediction)

    suggestion_text = suggestions_en.get(label, "🤖 Prediction not available.")

    cpu = entry.get("cpu_percent", 0)
    cpu_freq = entry.get("cpu_freq", {}).get("current", 0)
    ram = entry.get("virtual_memory", {}).get("percent", 0)
    swap = entry.get("swap_memory", {}).get("percent", 0)
    battery = entry.get("battery", {}).get("percent", "N/A")
    net_sent = entry.get("net_io", {}).get("bytes_sent", 0) / 1024 / 1024
    net_recv = entry.get("net_io", {}).get("bytes_recv", 0) / 1024 / 1024
    disk_read = entry.get("disk_io", {}).get("read_bytes", 0) / 1024 / 1024
    disk_write = entry.get("disk_io", {}).get("write_bytes", 0) / 1024 / 1024
    process_count = entry.get("process_count", 0)
    uptime = entry.get("uptime", 0) / 60

    temp_list = []
    for sensors in entry.get("temperatures", {}).values():
        for sensor in sensors:
            temp_list.append(sensor.get("current", 0))
    avg_temp = np.mean(temp_list) if temp_list else 0

    print("📊 [System Summary]")
    print(f"👉 Suggestion: {suggestion_text}")
    print(f"🧠 CPU Usage: {cpu}% | Frequency: {cpu_freq:.1f} MHz")
    print(f"🧮 RAM: {ram}% | SWAP: {swap}%")
    print(f"🔋 Battery: {battery}%")
    print(f"🌡 Average Temperature: {avg_temp:.1f}°C")
    print(f"📡 Network - Sent: {net_sent:.2f} MB | Received: {net_recv:.2f} MB")
    print(f"💾 Disk - Read: {disk_read:.2f} MB | Write: {disk_write:.2f} MB")
    print(f"🧵 Active Processes: {process_count}")
    print(f"⏱ Uptime: {uptime:.1f} minutes")
    print(f"🧬 AI Prediction Distribution: Stable: {prediction[0]*100:.1f}% | High Load: {prediction[1]*100:.1f}% | Overheat: {prediction[2]*100:.1f}%")

    return suggestion_text


def get_process_usages():
    process_info = []
    for proc in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent']):
        try:
            info = proc.info
            process_info.append(info)
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue
    return sorted(process_info, key=lambda x: x['cpu_percent'], reverse=True)


def suggest_app_actions(process_info):
    suggestions = []
    for proc in process_info:
        name = proc['name']
        cpu = proc['cpu_percent']
        mem = proc['memory_percent']

        if cpu < 1.0 and mem < 1.0:
            suggestions.append(f"🟡 '{name}' is using very low resources. Consider closing if idle.")
        elif cpu > 20.0 or mem > 10.0:
            suggestions.append(f"🔴 '{name}' is using high resources. Monitor or restart if unnecessary.")
    return suggestions

def ensure_log_directory():
    log_dir = os.path.expanduser("~/.ai_logs")
    os.makedirs(log_dir, exist_ok=True)
    return os.path.join(log_dir, "ai_suggestions.log")


def get_metrics_file_path():
    return os.path.expanduser("~/.ai_logs/system_metrics.json")

def read_and_analyze(filename=None):
    if filename is None:
        filename = get_metrics_file_path()

    if not os.path.exists(filename):
        return "❌ Metrics file not found. No data to analyze."

    with open(filename, "r") as f:
        lines = f.readlines()
        if not lines:
            return "📭 No data available to analyze yet."

        try:
            last_entry = json.loads(lines[-1])
        except json.JSONDecodeError:
            return "⚠️ Could not parse last log entry."

        model = create_model()
        return analyze_with_model(last_entry, model)

def alert_if_needed(entry):
    cpu = entry.get("cpu_percent", 0)
    ram = entry.get("virtual_memory", {}).get("percent", 0)
    battery = entry.get("battery", {}).get("percent", 100)
    avg_temp = 0

    temp_list = []
    for sensors in entry.get("temperatures", {}).values():
        for sensor in sensors:
            temp_list.append(sensor.get("current", 0))
    if temp_list:
        avg_temp = np.mean(temp_list)

    alerts = []

    if cpu > 85:
        alerts.append(f"⚠ High CPU usage: {cpu}%")
    if ram > 90:
        alerts.append(f"⚠ High RAM usage: {ram}%")
    if avg_temp > 80:
        alerts.append(f"🔥 System temperature is very high: {avg_temp:.1f}°C")
    if battery != "N/A" and battery < 15:
        alerts.append(f"🔋 Low battery: {battery}%")

    if alerts:
        combined = "\n".join(alerts)
        subprocess.run(["notify-send", "⚠ System Alert", combined])
        print(f"\n🔔 [Automatic Warnings]\n{combined}")
def most_intensive_process(processes):
    if not processes:
        return "❓ No process data available."

    # CPU'ya %70, RAM'e %30 ağırlık verilerek bir skor hesaplıyoruz (örnek ağırlıklar)
    def score(proc):
        return (proc['cpu_percent'] * 0.7) + (proc['memory_percent'] * 0.3)

    top = max(processes, key=score)

    return (
        f"🏷 Most Intensive Process:\n"
        f"🔧 Name: {top['name']} | PID: {top['pid']}\n"
        f"🧠 CPU: {top['cpu_percent']}% | 🗂 RAM: {top['memory_percent']:.2f}%\n"
        f"📊 Weighted Score: {score(top):.2f}"
    )
def get_interval_from_args(args):
    """
    Returns interval in seconds from command-line args.
    Args:
        args: Parsed argparse.Namespace object
    Returns:
        int: Interval in seconds
    """
    if args:
        # If both interval and get_interval are provided
        if args.get_interval and args.interval:
            unit = args.get_interval.strip().lower()  # unit is "s" or "m"
            try:
                amount = int(args.interval)  # The value for the interval
                if amount <= 0:
                    raise ValueError  # Ensure the interval amount is positive
                print(f"⏱ Using interval from args: {amount}{unit}")

                # Convert to minutes if "m", otherwise keep it in seconds
                if unit == "m":
                    return amount * 60  # Convert minutes to seconds
                elif unit == "s":
                    return amount  # Keep as seconds
                else:
                    print("⚠  Invalid unit for interval. Only 's' or 'm' are allowed.")
                    return None
            except ValueError:
                print("⚠  Invalid interval value provided. Falling back to manual input.")

        # Handle if only the interval is provided directly
        elif args.interval:
            return int(args.interval)  # Just return the interval in seconds

    # Default behavior: If no valid interval is found, return None
    return None


def get_interval_from_user():
    """
    Returns interval in seconds from user input interactively.
    Returns:
        int: Interval in seconds
    """
    while True:
        print("⏱ Set the monitoring interval.")
        unit = input("Choose time unit ([s]econds / [m]inutes): ").strip().lower()
        if unit not in ["s", "m"]:
            print("❌ Invalid unit. Please choose 's' for seconds or 'm' for minutes.")
            continue

        try:
            amount = int(input("Enter time amount (positive integer): ").strip())
            if amount <= 0:
                raise ValueError
        except ValueError:
            print("❌ Invalid number. Please enter a positive integer.")
            continue

        return amount * 60 if unit == "m" else amount


def get_interval(args=None):
    """
    Returns interval in seconds, either from command-line args or interactive prompt.
    Args:
        args: Parsed argparse.Namespace object (optional)
    Returns:
        int: Interval in seconds
    """
    # İlk önce komut satırından almayı dene
    interval = get_interval_from_args(args)
    
    # Eğer komut satırından alınamadıysa, kullanıcıdan manuel giriş al
    if interval is None:
        interval = get_interval_from_user()
    
    return interval


def get_user_permission(auto_prop):
    """
    Bu fonksiyon, Auto-Prop'un etkin olup olmadığını kontrol eder.
    Eğer --auto-prop varsa, doğrudan izin verilir ve kullanıcıya sorulmaz.
    """
    if auto_prop is not None:
        return auto_prop
    return False  # If no flag is set, return False by default

def list_processes():
    """
    Bu fonksiyon, aktif tüm süreçlerin pid, isim, cpu ve hafıza kullanımını döndüren bir liste oluşturur.
    """
    processes = []
    for proc in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent']):
        processes.append(proc.info)  # Process bilgilerini listeye ekleyin
    return processes

def auto_prop_manage(processes, permission=True):
    if not permission:
        return []

    actions = []

    # List of critical processes that should not be terminated
    critical_processes = [
        "plasma-start",  # Critical Plasma process
        "systemd", 
        "gnome", 
        "plasmashell", 
        "Xorg", 
        "kde", 
        "pulseaudio",
        "python3.12"  # Critical Python 3.12 processes should not be terminated
    ]
    
    for proc in processes:
        pid = proc['pid']
        name = proc['name']
        cpu = proc['cpu_percent']
        mem = proc['memory_percent']

        # Skip termination of critical processes
        if any(critical_name in name.lower() for critical_name in critical_processes):
            actions.append(f"✅ Auto-Prop: Skipping termination for critical process '{name}' (PID: {pid})")
            continue  # Skip this process

        if cpu > 70 or mem > 30:
            try:
                # If it's not a system-critical process, terminate it
                psutil.Process(pid).terminate()
                actions.append(f"❌ Auto-Prop: Terminated high-load process '{name}' (PID: {pid})")
            except (psutil.NoSuchProcess, psutil.AccessDenied) as e:
                # Log the error and continue instead of crashing
                log_path = ensure_log_directory()
                logging.basicConfig(filename=log_path, level=logging.ERROR)
                logging.error(f"Error while trying to terminate process {name} (PID: {pid}): {e}")
                continue

    return actions

# Ensure that logging is configured
log_path = ensure_log_directory()
logging.basicConfig(filename=log_path, level=logging.INFO)

# Example usage:
processes = list_processes()  # All processes are fetched dynamically
permission = get_user_permission(None)  # Example where permission is determined by some other factor
actions = auto_prop_manage(processes, permission)

if actions:
    for action in actions:
        logging.info(action)


def run_monitor_loop(args):
    interval = get_interval(args)
    permission = get_user_permission(auto_prop=True) 
    log_path = ensure_log_directory()
    metrics_path = get_metrics_file_path()

    print(f"\n🧠 Auto-Prop Enabled: {permission}")
    print(f"🔄 Monitoring system every {interval} seconds...\n")

    while True:
        try:
            result = read_and_analyze()
            processes = get_process_usages()
            app_suggestions = suggest_app_actions(processes[:10])
            intensive_proc = most_intensive_process(processes)
            auto_prop_actions = auto_prop_manage(processes[:10], permission)

            print(f"\n👉 AI Suggestion: {result}")
            print(intensive_proc)

            print("📌 [Top 5 Resource-Intensive Processes]")
            for proc in processes[:5]:
                print(f"🔍 {proc['name']} | CPU: {proc['cpu_percent']}% | RAM: {proc['memory_percent']:.2f}%")

            print("\n💡 [Smart Suggestions Based on Application Usage]")
            for s in app_suggestions:
                print(s)

            if auto_prop_actions:
                print("\n🚀 [Auto-Prop Actions]")
                for a in auto_prop_actions:
                    print(a)

            subprocess.run(["notify-send", "Plasma System AI", result])

            # system_metrics.json okuma
            if os.path.exists(metrics_path):
                with open(metrics_path, "r") as mf:
                    lines = mf.readlines()
                    if lines:
                        last_line = lines[-1]
                        alert_if_needed(json.loads(last_line))

            # Loglama
            with open(log_path, "a") as log:
                log.write(f"[{datetime.now()}] Suggestion: {result}\n")
                log.write(str(intensive_proc) + "\n")
                for s in app_suggestions:
                    log.write(f"- {s}\n")
                for a in auto_prop_actions:
                    log.write(f"[Auto-Prop] {a}\n")
                log.write("\n")

        except Exception as e:
            print(f"❌ Error during monitoring: {e}")
        time.sleep(interval)



if __name__ == "__main__":
    print("[AI Engine - TensorFlow] Analyzing system...\n")
    result = read_and_analyze()
    log_path = ensure_log_directory()
    run_monitor_loop()

    print(f"\n👉 AI Suggestion: {result}\n")

    print("📌 [Top 5 Resource-Intensive Processes]")
    processes = get_process_usages()
    for proc in processes[:5]:
        print(f"🔍 {proc['name']} | CPU: {proc['cpu_percent']}% | RAM: {proc['memory_percent']:.2f}%")

    print("\n💡 [Smart Suggestions Based on Application Usage]")
    app_suggestions = suggest_app_actions(processes[:10])
    for s in app_suggestions:
        print(f"{s}")

    with open(log_path, "a") as log:
        log.write(f"[{datetime.now()}] Suggestion: {result}\n")
        for s in app_suggestions:
            log.write(f"- {s}\n")

    subprocess.run(["notify-send", "Plasma System AI", result])

    parser.add_argument("--interval", "-i", type=int, help="Monitoring interval in seconds", default=60)
    parser.add_argument("--auto-prop", "-a", action="store_true", help="Enable automatic process management")
    parser.add_argument("--analyze", "-z", action="store_true", help="Analyze latest system log entry and display summary")
    parser.add_argument("--most-intensive", "-m", action="store_true", help="Show most resource-intensive process")
    parser.add_argument("--suggest", "-s", action="store_true", help="Suggest actions for current top processes")

    args = parser.parse_args()

    if args.analyze:
        print(read_and_analyze())

    processes = get_process_usages()

    if args.most_intensive:
        print(most_intensive_process(processes))

    if args.suggest:
        for s in suggest_app_actions(processes):
            print(s)

    if args.auto_prop:
        print("⚙ Auto-Prop is active.")
        auto_prop_manage(processes, permission=True)

    print(f"🕒 Interval is set to {args.interval} seconds.")
    time.sleep(args.interval)

