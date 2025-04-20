import torch
import torch.nn as nn
from sklearn.preprocessing import MinMaxScaler
import numpy as np
import pandas as pd
import os

# Model yapılandırması
class IOModel(nn.Module):
    def __init__(self, input_size=2, hidden_size=64, output_size=4):
        super(IOModel, self).__init__()
        self.lstm = nn.LSTM(input_size, hidden_size, batch_first=True)
        self.fc = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        out, _ = self.lstm(x)
        out = self.fc(out[:, -1, :])
        return out

# Etiket haritası
LABELS = {
    "IDLE": 0,
    "READ_FOCUS": 1,
    "WRITE_PRIORITY": 2,
    "OPTIMIZE_BALANCE": 3
}

# Ayarlar
LOG_FILE = "ai_lfs_log.csv"
MODEL_PATH = "ai_model.pt"
EPOCHS = 50
BATCH_SIZE = 8
LR = 0.001

def load_and_prepare_data(log_file):
    df = pd.read_csv(log_file)
    df = df.dropna()

    # Girdi ve çıktı ayrımı
    X = df[["read_bytes", "write_bytes"]].values
    y = df["ai_decision"].map(LABELS).values

    # Normalizasyon
    scaler = MinMaxScaler()
    X_scaled = scaler.fit_transform(X)

    # LSTM için 3D giriş: (örnek sayısı, zaman adımı, özellik sayısı)
    X_seq = np.expand_dims(X_scaled, axis=1)  # (n_samples, 1, 2)

    return torch.tensor(X_seq, dtype=torch.float32), torch.tensor(y, dtype=torch.long), scaler

def train():
    print("[TRAIN] Eğitim başlatıldı...")
    if not os.path.exists(LOG_FILE):
        print("[TRAIN] Log dosyası bulunamadı.")
        return

    X, y, scaler = load_and_prepare_data(LOG_FILE)

    model = IOModel()
    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=LR)

    for epoch in range(EPOCHS):
        permutation = torch.randperm(X.size(0))
        total_loss = 0

        for i in range(0, X.size(0), BATCH_SIZE):
            indices = permutation[i:i+BATCH_SIZE]
            batch_x, batch_y = X[indices], y[indices]

            optimizer.zero_grad()
            outputs = model(batch_x)
            loss = criterion(outputs, batch_y)
            loss.backward()
            optimizer.step()

            total_loss += loss.item()

        print(f"[TRAIN] Epoch {epoch+1}/{EPOCHS}, Loss: {total_loss:.4f}")

    torch.save(model.state_dict(), MODEL_PATH)
    print(f"[TRAIN] Model kaydedildi: {MODEL_PATH}")

if __name__ == "__main__":
    train()
