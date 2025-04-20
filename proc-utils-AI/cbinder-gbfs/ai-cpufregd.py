import os
import time
import yaml
import psutil
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

CONFIG_PATH = "/var/AI-stump/AI-runtime.yml"
SYS_RELOAD = "/sys/ai/freqctl"

class ConfigWatcher(FileSystemEventHandler):
    def on_modified(self, event):
        if event.src_path == CONFIG_PATH:
            print("[AI-cpufreqd] YAML dosyası değişti. Kernel’e reload tetikleniyor...")
            with open(SYS_RELOAD, "w") as f:
                f.write("reload")

def monitor_usage_and_autotune():
    while True:
        cpu_usages = psutil.cpu_percent(percpu=True)
        # Örnek: usage > 90 ise boost modunu tetikle
        if any(u > 90 for u in cpu_usages):
            print("[AI-cpufreqd] Yüksek CPU kullanımı tespit edildi. Turbo mod tetiklenebilir.")
            # Burada istersen frekans politikası da yazılabilir.
        time.sleep(3)

if __name__ == "__main__":
    print("[AI-cpufreqd] Başlatıldı. YAML dosyası izleniyor.")
    event_handler = ConfigWatcher()
    observer = Observer()
    observer.schedule(event_handler, path=os.path.dirname(CONFIG_PATH), recursive=False)
    observer.start()

    try:
        monitor_usage_and_autotune()
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
