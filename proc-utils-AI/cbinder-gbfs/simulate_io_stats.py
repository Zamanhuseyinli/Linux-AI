import random
import csv
import os
from datetime import datetime

LOG_FILE = "ai_lfs_log.csv"

LABELS = ["IDLE", "READ_FOCUS", "WRITE_PRIORITY", "OPTIMIZE_BALANCE"]

# Simülasyon senaryoları
def generate_scenario():
    scenario = random.choice(LABELS)

    if scenario == "IDLE":
        read = random.randint(0, 1000)
        write = random.randint(0, 1000)
    elif scenario == "READ_FOCUS":
        read = random.randint(10000, 50000)
        write = random.randint(1000, 5000)
    elif scenario == "WRITE_PRIORITY":
        read = random.randint(1000, 5000)
        write = random.randint(10000, 50000)
    elif scenario == "OPTIMIZE_BALANCE":
        base = random.randint(10000, 40000)
        fluctuation = random.randint(-3000, 3000)
        read = base + fluctuation
        write = base - fluctuation
    else:
        read = write = 0

    return read, write, scenario

def simulate_entries(count=500):
    # Eğer dosya yoksa başlık ekle
    file_exists = os.path.exists(LOG_FILE)
    with open(LOG_FILE, "a", newline="") as f:
        writer = csv.writer(f)
        if not file_exists:
            writer.writerow(["timestamp", "read_bytes", "write_bytes", "ai_decision"])

        for _ in range(count):
            read, write, decision = generate_scenario()
            timestamp = datetime.now().isoformat()
            writer.writerow([timestamp, read, write, decision])

    print(f"[SIM] {count} örnek başarıyla eklendi → {LOG_FILE}")

if __name__ == "__main__":
    simulate_entries(count=500)
