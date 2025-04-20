import json
import time
import pyttsx3
from datetime import datetime

AFFINITY_FILE = "/var/AI-stump/ai-affinity-log.json"
ANALYZER_FILE = "/var/AI-stump/AI-runtime.yml"

def init_robot_voice():
    engine = pyttsx3.init()
    engine.setProperty('rate', 155)      # Daha yavaş, robotik vurgulu
    engine.setProperty('volume', 0.8)    # Sedalı ama net
    voices = engine.getProperty('voices')
    
    # Linux için espeak robotik ses önerisi (kullanıcıya göre seçilebilir)
    for voice in voices:
        if "robot" in voice.name.lower() or "klatt" in voice.id.lower():
            engine.setProperty('voice', voice.id)
            break
    else:
        # Alternatif: derin, düşük tonlu ses seç
        if len(voices) >= 2:
            engine.setProperty('voice', voices[1].id)

    return engine

def speak(text, engine=None):
    if not engine:
        engine = init_robot_voice()
    engine.say(text)
    engine.runAndWait()

def summarize_affinity(path):
    try:
        with open(path, "r") as f:
            data = json.load(f)
        last = data["history"][-1]
        summary = f"{last['service']} servisi {', '.join(map(str, last['assigned_cores']))} çekirdeklerine atandı. Öncelik seviyesi: {last['nice_level']}."
        return summary
    except Exception as e:
        return f"Çekirdek atanımı bilgisi alınamadı: {e}"

def summarize_ai_runtime(path):
    try:
        with open(path, "r") as f:
            lines = f.readlines()
        summary_lines = [line.strip() for line in lines if not line.strip().startswith("#") and ":" in line]
        spoken = ". ".join(summary_lines[:5])
        return f"AI çalışma ortamı yapılandırması: {spoken}"
    except Exception as e:
        return f"AI yapılandırma bilgisi okunamadı: {e}"

def main():
    engine = init_robot_voice()
    now = datetime.now().strftime("%H:%M")
    speak(f"Saat {now}. Sistem AI raporu başlatılıyor.", engine)

    affinity_summary = summarize_affinity(AFFINITY_FILE)
    print("[AFFINITY] ➤", affinity_summary)
    speak(affinity_summary, engine)

    runtime_summary = summarize_ai_runtime(ANALYZER_FILE)
    print("[AI-CONFIG] ➤", runtime_summary)
    speak(runtime_summary, engine)

    speak("Raporlama tamamlandı. Bir sonraki görev için bekleniyor.", engine)

if __name__ == "__main__":
    main()
