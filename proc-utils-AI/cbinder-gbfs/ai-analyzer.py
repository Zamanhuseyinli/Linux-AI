import yaml
import csv
import os
import datetime
import json
import argparse
from statistics import mean
from typing import List, Dict, Tuple

SERVICE_DIR = "/var/AI-stump/services/"
DATASET_FILE = "/var/AI-stump/ai_dataset.csv"

def load_service_profile(service_id: str) -> Dict:
    path = os.path.join(SERVICE_DIR, f"{service_id}.svc.yml")
    with open(path, "r") as file:
        return yaml.safe_load(file)

def fetch_historical_metrics(service_id: str) -> List[Dict]:
    if not os.path.exists(DATASET_FILE):
        return []

    metrics = []
    with open(DATASET_FILE, "r") as file:
        reader = csv.DictReader(file)
        for row in reader:
            if row["service_id"] == service_id:
                try:
                    metrics.append({
                        "cpu_usage": float(row["cpu_usage"]),
                        "io_read": int(row["io_read"]),
                        "io_write": int(row["io_write"]),
                        "assigned_cores": eval(row["assigned_cores"]),
                        "nice_level": int(row["nice_level"]),
                        "outcome_score": float(row["outcome_score"]),
                        "power_watts": float(row.get("power_watts", 0)),
                        "cpu_temp": float(row.get("cpu_temp", 0))
                    })
                except:
                    continue
    return metrics

def score_affinity(entries: List[Dict]) -> float:
    score_weight = 0.5
    power_penalty = 0.3
    temp_penalty = 0.2

    avg_score = mean([e["outcome_score"] for e in entries])
    avg_power = mean([e["power_watts"] for e in entries])
    avg_temp = mean([e["cpu_temp"] for e in entries])

    final_score = (score_weight * avg_score) - (power_penalty * avg_power) - (temp_penalty * (avg_temp / 100))
    return final_score

def recommend_affinity(service_id: str) -> Tuple[List[int], int, float]:
    history = fetch_historical_metrics(service_id)
    if not history:
        profile = load_service_profile(service_id)
        return profile["ai_guidance"]["default_affinity"], profile["ai_guidance"]["preferred_nice"], 0.0

    affinity_scores = {}
    for entry in history:
        key = (tuple(entry["assigned_cores"]), entry["nice_level"])
        if key not in affinity_scores:
            affinity_scores[key] = []
        affinity_scores[key].append(entry)

    best_config = max(affinity_scores.items(), key=lambda x: score_affinity(x[1]))
    final_score = score_affinity(best_config[1])
    return list(best_config[0][0]), best_config[0][1], final_score

def list_services() -> List[str]:
    return [f.replace(".svc.yml", "") for f in os.listdir(SERVICE_DIR) if f.endswith(".svc.yml")]

def print_recommendation(service_id: str, json_output: bool = False):
    affinity, nice, score = recommend_affinity(service_id)
    result = {
        "service_id": service_id,
        "recommended_affinity": affinity,
        "recommended_nice": nice,
        "score": round(score, 3)
    }

    if json_output:
        print(json.dumps(result, indent=2))
    else:
        print(f"[AI-Analyzer] Servis: {service_id}")
        print(f"⮕ Affinity       : {affinity}")
        print(f"⮕ Nice Level     : {nice}")
        print(f"⮕ Skor           : {round(score, 3)}")
        print("--------")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--json", action="store_true", help="JSON formatında çıktı verir.")
    args = parser.parse_args()

    print("[AI-Analyzer] Başlatıldı...\n")
    for service in list_services():
        print_recommendation(service, json_output=args.json)
