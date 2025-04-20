import psutil
import os
import yaml
import time

RUNTIME_PROFILE = "/var/AI-stump/AI-runtime.yml"

def load_runtime_config():
    try:
        with open(RUNTIME_PROFILE, "r") as f:
            return yaml.safe_load(f)
    except Exception as e:
        print(f"[ai-affinity] Profil dosyası okunamadı: {e}")
        return {}

def apply_affinity():
    config = load_runtime_config()
    if not config:
        return

    cpu_count = psutil.cpu_count()
    affinity_map = config.get("affinity", {})

    for proc in psutil.process_iter(attrs=['pid', 'name']):
        try:
            pname = proc.info['name']
            pid = proc.info['pid']

            if pname in affinity_map:
                rule = affinity_map[pname]
                cores = rule.get("cores", [])
                nice = rule.get("nice", 0)

                if cores:
                    proc.cpu_affinity(cores)
                    print(f"[ai-affinity] {pname} -> çekirdek: {cores}")
                if "nice" in rule:
                    os.system(f"renice -n {nice} -p {pid}")
                    print(f"[ai-affinity] {pname} -> öncelik (nice): {nice}")

        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue

def main():
    print("[ai-affinity] Başlatıldı. AI çekirdek eşlemesi aktif.")
    while True:
        apply_affinity()
        time.sleep(5)  # 5 saniyede bir tarama

if __name__ == "__main__":
    main()
