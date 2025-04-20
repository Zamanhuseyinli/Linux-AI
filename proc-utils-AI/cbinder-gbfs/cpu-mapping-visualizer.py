import csv
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from collections import defaultdict
import os

DATASET_FILE = "/var/AI-stump/ai_dataset.csv"
MAX_CPU_CORES = 32  # sistemin desteklediği max core sayısı

def parse_cpu_data():
    core_usage_map = defaultdict(list)

    if not os.path.exists(DATASET_FILE):
        raise FileNotFoundError("Veri dosyası bulunamadı: " + DATASET_FILE)

    with open(DATASET_FILE, "r") as file:
        reader = csv.DictReader(file)
        for row in reader:
            try:
                cores = eval(row["assigned_cores"])
                usage = float(row["cpu_usage"])
                for core in cores:
                    core_usage_map[core].append(usage)
            except:
                continue
    return core_usage_map

def generate_heatmap(core_usage_map):
    core_values = [0] * MAX_CPU_CORES
    for i in range(MAX_CPU_CORES):
        values = core_usage_map.get(i, [])
        core_values[i] = round(np.mean(values), 2) if values else 0.0

    # CPU çekirdeklerini 8x4'lik bir blok gibi görselleştirelim
    rows, cols = 8, 4
    matrix = np.array(core_values + [0] * (rows * cols - len(core_values)))
    matrix = matrix.reshape((rows, cols))

    plt.figure(figsize=(10, 6))
    sns.heatmap(matrix, annot=True, cmap="YlOrRd", cbar=True, linewidths=0.5)
    plt.title("AI-CPU Kullanım Isı Haritası (Çekirdek Bazlı)")
    plt.xlabel("Core Blokları")
    plt.ylabel("CPU Satırları")
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    print("[CPU Visualizer] CPU kullanım verileri okunuyor...")
    core_usage = parse_cpu_data()
    generate_heatmap(core_usage)
