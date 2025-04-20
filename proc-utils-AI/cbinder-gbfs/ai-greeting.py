import pyttsx3
import os
import time
from datetime import datetime

TMP_AUDIO_PATH = "/tmp/ai-greeting.wav"

def get_greeting():
    hour = datetime.now().hour
    if 6 <= hour < 12:
        return "Hello, good morning."
    elif 12 <= hour < 18:
        return "Hello, good afternoon."
    else:
        return "Hello tonight. The night is here."

def init_robot_voice():
    engine = pyttsx3.init()
    engine.setProperty('rate', 150)
    engine.setProperty('volume', 0.85)

    voices = engine.getProperty('voices')
    for voice in voices:
        if "robot" in voice.name.lower() or "klatt" in voice.id.lower():
            engine.setProperty('voice', voice.id)
            break
    else:
        if len(voices) >= 2:
            engine.setProperty('voice', voices[1].id)
    return engine

def generate_temp_audio(text, path):
    engine = init_robot_voice()
    engine.save_to_file(text, path)
    engine.runAndWait()

def play_temp_audio(path):
    # Platforma göre oynatıcı seç
    if os.name == "posix":
        os.system(f"aplay {path} >/dev/null 2>&1 || paplay {path} >/dev/null 2>&1")
    elif os.name == "nt":
        os.system(f'start /min wmplayer "{path}"')
    else:
        print("Ses çalma yöntemi tanımlanmadı.")

def main():
    greeting = get_greeting()
    print(f"[GREETING] ➤ {greeting}")
    generate_temp_audio(greeting, TMP_AUDIO_PATH)
    play_temp_audio(TMP_AUDIO_PATH)
    time.sleep(2)
    if os.path.exists(TMP_AUDIO_PATH):
        os.remove(TMP_AUDIO_PATH)

if __name__ == "__main__":
    main()
