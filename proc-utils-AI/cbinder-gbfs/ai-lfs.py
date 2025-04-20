import torch
import torch.nn as nn
import time
import re
from sklearn.preprocessing import MinMaxScaler
import numpy as np
import psutil
import csv
import os
from datetime import datetime

# Dosya yolları
AI_DEVICE = "/dev/ai_lfs"
IO_STATS_PROC = "/proc/sentielcpu_io_stats"
MODEL_PATH = "ai_model.pt"
LOG_FILE = "ai_lfs_log.csv"

# AI Karar etiketleri
LABELS = ["IDLE", "READ_FOCUS", "WRITE_PRIORITY", "OPTIMIZE_BALANCE"]

# Log dosyası yoksa başlık satırı ekle
if not os.path.exists(LOG_FILE):
    with open(LOG_FILE, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["timestamp", "read_bytes", "write_bytes", "ai_decision"])

def log_decision(io_data, decision):
    with open(LOG_FILE, "a", newline="") as f:
        writer = csv.writer(f)
        timestamp = datetime.now().isoformat()
        writer.writerow([timestamp, io_data[0], io_data[1], decision])

# LSTM model yapısı
class IOModel(nn.Module):
    def __init__(self, input_size=2, hidden_size=64, output_size=4):
        super(IOModel, self).__init__()
        self.lstm = nn.LSTM(input_size, hidden_size, batch_first=True)
        self.fc = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        out, _ = self.lstm(x)
        out = self.fc(out[:, -1, :])
        return out

# /proc içeriğinden IO istatistiklerini çek
def parse_io_stats(io_text):
    try:
        read = int(re.search(r"read:\s*(\d+)", io_text).group(1))
        write = int(re.search(r"write:\s*(\d+)", io_text).group(1))
        return [read, write]
    except Exception:
        return [0, 0]

# AI modeli tahmin fonksiyonu
def ai_predict(model, scaler, stats):
    tensor_input = scaler.transform([stats])
    tensor_input = torch.tensor(tensor_input, dtype=torch.float32).unsqueeze(0)  # (1, 1, 2)
    output = model(tensor_input)
    predicted = torch.argmax(output, dim=1).item()
    return LABELS[predicted]

# Sistem metriklerini toplayan fonksiyon
def collect_system_metrics():
    cpu_percent = psutil.cpu_percent(interval=1)
    mem = psutil.virtual_memory()
    load1, load5, load15 = psutil.getloadavg()
    return {
        "cpu_percent": cpu_percent,
        "mem_percent": mem.percent,
        "load_avg": [load1, load5, load15]
    }

# Ana loop
def main_loop():
    print("[AI-LFS] Gelişmiş AI LFS motoru başlatıldı...")
    model = IOModel()
    model.load_state_dict(torch.load(MODEL_PATH, map_location=torch.device('cpu')))
    model.eval()

    scaler = MinMaxScaler(feature_range=(0, 1))
    scaler.fit([[0, 0], [100000, 100000]])  # örnekleme

    while True:
        try:
            with open(IO_STATS_PROC, "r") as f:
                stats_text = f.read()
            io_data = parse_io_stats(stats_text)

            decision = ai_predict(model, scaler, io_data)

            with open(AI_DEVICE, "w") as f:
                f.write(decision)

            print(f"[AI-LFS] IO: {io_data} → AI Kararı: {decision}")
            log_decision(io_data, decision)

            metrics = collect_system_metrics()
            print(f"[AI-LFS] Ek Metrikler → CPU: {metrics['cpu_percent']}%, RAM: {metrics['mem_percent']}%, Load Avg: {metrics['load_avg']}")
        
        except Exception as e:
            print("[AI-LFS] Hata:", e)

        time.sleep(5)

# Uygulama başlangıcı
if __name__ == "__main__":
    try:
        main_loop()
    except KeyboardInterrupt:
        print("\n[AI-LFS] Kapatılıyor...")
