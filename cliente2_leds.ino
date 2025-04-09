
#include <WiFi.h>

#ifndef LED_H
#define LED_H

class Led {
private:
    int pin;
    unsigned long intervaloParpadeo;
    bool estado;
    unsigned long ultimoCambio;

public:
    Led(int pin, unsigned long intervalo = 200);
    void parpadeo();
    void apagar();
};

#endif

Led::Led(int pin, unsigned long intervalo)
  : pin(pin), intervaloParpadeo(intervalo), estado(false), ultimoCambio(0) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void Led::parpadeo() {
  unsigned long ahora = millis();
  if (ahora - ultimoCambio >= intervaloParpadeo) {
    estado = !estado;
    digitalWrite(pin, estado ? HIGH : LOW);
    ultimoCambio = ahora;
  }
}

void Led::apagar() {
  estado = false;
  digitalWrite(pin, LOW);
}

// Configuración de red
const char* ssid = "Familia Salinas";
const char* contrasenia = "5278901cbba";
const char* servidor_ip = "192.168.100.3";
const uint16_t puerto = 1234;

WiFiClient cliente;

Led leds[] = {
  Led(18),  // ROJO
  Led(19),  // NARANJA
  Led(21),  // AMARILLO
  Led(22),  // VERDE
  Led(23)   // AZUL
};

const int cantidadLeds = sizeof(leds) / sizeof(leds[0]);
int ledActivo = -1;

void conectarWiFi() {
  Serial.print("Conectando a Wi-Fi: ");
  Serial.println(ssid);

  unsigned long inicio = millis();
  WiFi.begin(ssid, contrasenia);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  unsigned long fin = millis();
  unsigned long tiempo_conexion = fin - inicio;

  Serial.println();
  Serial.print("Conectado a Wi-Fi en ");
  Serial.print(tiempo_conexion);
  Serial.println(" ms");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

void conectarServidor() {
  Serial.println("Conectando al servidor...");
  while (!cliente.connect(servidor_ip, puerto)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConectado al servidor");
  cliente.println("actuador");
}

float recibirDistancia() {
  static String buffer = "";
  while (cliente.available()) {
    char c = cliente.read();
    if (c == '\n') {
      float distancia = buffer.toFloat();
      buffer = "";
      return distancia;
    } else {
      buffer += c;
    }
  }
  return -1.0;  
}

void setup() {
  Serial.begin(115200);
  conectarWiFi();
  conectarServidor();
}

void loop() {
  float distancia = recibirDistancia();
  if (distancia < 0) return;

  Serial.print("Distancia recibida: ");
  Serial.println(distancia);

  int nuevoLedActivo = -1;
  if (distancia <= 10) nuevoLedActivo = 0;
  else if (distancia <= 20) nuevoLedActivo = 1;
  else if (distancia <= 30) nuevoLedActivo = 2;
  else if (distancia <= 40) nuevoLedActivo = 3;
  else if (distancia <= 50) nuevoLedActivo = 4;

  if (nuevoLedActivo != ledActivo) {
    for (int i = 0; i < cantidadLeds; i++) leds[i].apagar();
    ledActivo = nuevoLedActivo;
  }

  if (ledActivo >= 0 && ledActivo < cantidadLeds) {
    leds[ledActivo].parpadeo();
  }
}


