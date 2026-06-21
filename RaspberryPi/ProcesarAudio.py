import os
import joblib
import librosa
import numpy as np
import paho.mqtt.client as mqtt
import time
from mpd import MPDClient

# ==========================================
# Configuracion de archivos y red MQTT
# ==========================================
MODEL_FILE = "modelo.joblib"

MQTT_BROKER = "192.168.100.220"  
MQTT_PORT = 1883
MQTT_TOPIC = "musica"

DURACION_BLOQUE = 3  # El modelo procesa bloques de 3 segundos

def extraer_caracteristicas_bloque(y_block, sr):
    """Extrae las 44 caracteristicas en el orden exacto del entrenamiento."""
    mfcc = librosa.feature.mfcc(y=y_block, sr=sr, n_mfcc=20)
    spectral = librosa.feature.spectral_centroid(y=y_block, sr=sr)
    zero_cross = librosa.feature.zero_crossing_rate(y=y_block)

    features = np.hstack([
        np.mean(mfcc, axis=1), np.std(mfcc, axis=1),
        np.mean(spectral, axis=1), np.std(spectral, axis=1),
        np.mean(zero_cross, axis=1), np.std(zero_cross, axis=1)
    ])
    return features

mpd_client = MPDClient()
mpd_client.connect("localhost", 6600)

cancion_activa = None
y_audio = None
sr_audio = None

generos = [
    "Blues",
    "Clásica",
    "Country",
    "Disco",
    "Electrónica",
    "HipHop",
    "Jazz",
    "Metal",
    "Pop",
    "Reggae",
    "Rock"
]

try:
    # Cargar modelo e interprete
    print("🧠 Cargando modelo desde joblib...")
    if not os.path.exists(MODEL_FILE):
        raise FileNotFoundError(f"No se encuentra el archivo '{MODEL_FILE}'.")
        
    componentes = joblib.load(MODEL_FILE)
    modelo_g = componentes["modelo"]
    encoder_g = componentes["encoder"]
    modelo_g.verbose = False

    # Conectar al Broker MQTT
    print(f"📡 Conectando al broker MQTT en {MQTT_BROKER}...")
    cliente_mqtt = mqtt.Client()
    cliente_mqtt.connect(MQTT_BROKER, MQTT_PORT, 60)
except FileNotFoundError as e:
    print(f"\n❌ Error de archivos: {e}")
except Exception as e:
    print(f"\n❌ Ocurrió un error inesperado: {e}")

print("Escuchando a MPD en tiempo real...\n")

while True:
    try:
        status = mpd_client.status()
        state = status.get('state')

        if state == 'play':
            current_song = mpd_client.currentsong()
            song_path = current_song.get('file')

            if song_path != cancion_activa:
                print(f"🎵 Nueva canción:{song_path}")
                cancion_activa = song_path
                # Cargar audio a memoria
                print("Cargando audio a memoria")
                y_audio, sr_audio = librosa.load(f"/var/lib/mpd/music/{song_path}", sr=22050)
                print("Audio cargado en memoria")

            elapsed = float(status.get('elapsed', 0))

            if elapsed >= 3:
                inicio = int((elapsed - 3) * sr_audio)
                fin = int(elapsed * sr_audio)
                y_block = y_audio[inicio:fin]

                # Inferencia
                features = extraer_caracteristicas_bloque(y_block, sr_audio)
                pred = modelo_g.predict([features])[0]

                # Publicar en MQTT, topic: musica
                cliente_mqtt.publish(MQTT_TOPIC, str(pred))
                print(f"Género enviado: {pred} - {generos[pred]}")
        
        time.sleep(1) # Revisar estado cada segundo (por si se puso pausa, acabo la cancion, o se cambio de cancion).
    except Exception as e:
        print(f"Error: {e}")
        time.sleep(5)