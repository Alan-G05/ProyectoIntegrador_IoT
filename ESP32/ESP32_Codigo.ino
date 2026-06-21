#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "SSID";
const char* password = "PASS";
const char* mqtt_server = "IP_MQTT_BROKER";
const char* topic = "musica";

#define PIN_DATOS   23  
#define NUM_LEDS    16  

Adafruit_NeoPixel aro(NUM_LEDS, PIN_DATOS, NEO_GRB + NEO_KHZ800);
WiFiClient espClient;
PubSubClient client(espClient);

// Animaciones para la respuesta del modelo

void mostrarColorFijo(uint32_t color) {
  for(int i = 0; i < aro.numPixels(); i++) aro.setPixelColor(i, color);
  aro.show(); 
}

// Barrido veloz de leds
void animacionBarridoRapido(uint32_t color, int espera) {
  for (int i = 0; i < aro.numPixels(); i++) {
    aro.setPixelColor(i, color);
    aro.show();
    delay(espera);
  }
}

// Estrobo corto (Ideal para transiciones de electrónica o metal)
void animacionEstroboCorto(uint32_t color, int espera) {
  for (int d = 0; d < 3; d++) { // Solo 3 destellos rápidos
    mostrarColorFijo(color);
    delay(espera);
    mostrarColorFijo(aro.Color(0,0,0));
    delay(espera);
  }
  mostrarColorFijo(color);
}

// Giro rápido de 2 vueltas
void animacionGiroCorto(uint32_t color, int espera) {
  for (int c = 0; c < 2; c++) { 
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < aro.numPixels(); i++) {
        if ((i + q) % 3 == 0) aro.setPixelColor(i, color);
        else aro.setPixelColor(i, 0);
      }
      aro.show();
      delay(espera);
    }
  }
  mostrarColorFijo(color);
}

void animacionReggae() {
  for (int i = 0; i < aro.numPixels(); i++) {
    if (i < 5)  aro.setPixelColor(i, aro.Color(0, 255, 0));    
    else if (i < 11) aro.setPixelColor(i, aro.Color(255, 200, 0)); 
    else aro.setPixelColor(i, aro.Color(255, 0, 0));           
  }
  aro.show();
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; 
  int id_genero = atoi((char*)payload); 
  
  Serial.print("\n🎵 Cambio detectado! Ejecutando animación para ID: ");
  Serial.println(id_genero);

  switch(id_genero) {
    case 0: // BLUES
      animacionBarridoRapido(aro.Color(0, 0, 255), 25);
      break;
    case 1: // CLASSICAL
      animacionBarridoRapido(aro.Color(130, 130, 80), 40);
      break;
    case 2: // COUNTRY
      animacionBarridoRapido(aro.Color(140, 70, 0), 30);
      break;
    case 3: // DISCO
      animacionGiroCorto(aro.Color(255, 0, 120), 60);
      break;
    case 4: // ELECTRONICA
      animacionEstroboCorto(aro.Color(0, 255, 30), 40);
      break;
    case 5: // HIPHOP
      animacionGiroCorto(aro.Color(120, 0, 120), 70);
      break;
    case 6: // JAZZ
      animacionBarridoRapido(aro.Color(240, 160, 0), 25);
      break;
    case 7: // METAL
      animacionEstroboCorto(aro.Color(240, 0, 0), 35);
      break;
    case 8: // POP
      animacionBarridoRapido(aro.Color(0, 240, 240), 20);
      break;
    case 9: // REGGAE
      animacionReggae();
      break;
    case 10: // ROCK
      animacionBarridoRapido(aro.Color(240, 35, 0), 30);
      break;
    default:
      mostrarColorFijo(aro.Color(20, 20, 20));
      break;
  }
}

void setup() {
  Serial.begin(115200);
  aro.begin();
  aro.setBrightness(90);
  aro.show();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n¡WiFi Conectado!");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Prueba")) {
      Serial.println("¡Conectado a Mosquitto!");
      client.subscribe(topic); 
    } else {
      delay(3000);
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}