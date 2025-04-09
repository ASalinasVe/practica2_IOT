
#ifndef ULTRASONICO_H
#define ULTRASONICO_H

#include <WiFi.h>

class Ultrasonico {
private:
    int trigPin;
    int echoPin;
    WiFiClient cliente;

    void conectarWiFi(const char* ssid, const char* contrasenia);
    void conectarServidor(const char* ip, uint16_t puerto);

public:
    Ultrasonico(int trigPin, int echoPin);

    void begin(const char* ssid, const char* contrasenia, const char* servidor_ip, uint16_t puerto);
    void loop();
    float medirDistancia();
    bool conectado();
    void enviarDistancia(float distancia);
};

#endif

Ultrasonico::Ultrasonico(int trig, int echo)
  : trigPin(trig), echoPin(echo) {}

void Ultrasonico::conectarWiFi(const char* ssid, const char* contrasenia) {
   Serial.print("Conectando a Wi-Fi: ");
  Serial.println(ssid);

  unsigned long inicio = millis(); 

  WiFi.begin(ssid, contrasenia);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  unsigned long fin = millis(); 
  unsigned long tiempoConexion = fin - inicio;

  Serial.println();
  Serial.print("Conectado a Wi-Fi en ");
  Serial.print(tiempoConexion);
  Serial.println(" ms");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void Ultrasonico::conectarServidor(const char* ip, uint16_t puerto) {
  while (!cliente.connect(ip, puerto)) {
    delay(1000);
  }
  cliente.println("sensor");
}

void Ultrasonico::begin(const char* ssid, const char* contrasenia, const char* servidor_ip, uint16_t puerto) {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  conectarWiFi(ssid, contrasenia);
  conectarServidor(servidor_ip, puerto);
}

float Ultrasonico::medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duracion = pulseIn(echoPin, HIGH);
  return duracion * 0.034 / 2;
}

bool Ultrasonico::conectado() {
  return cliente.connected();
}

void Ultrasonico::enviarDistancia(float distancia) {
  if (cliente.connected()) {
    cliente.println(distancia);
  }
}

void Ultrasonico::loop() {
  float distancia = medirDistancia();
  enviarDistancia(distancia);
  delay(1000);
}


Ultrasonico sensor(5, 18); 

// Credenciales WiFi y servidor
const char* ssid = "Familia Salinas";
const char* contrasenia = "5278901cbba";
const char* servidor_ip = "192.168.100.3"; 
const uint16_t puerto = 1234;

void setup() {
  Serial.begin(115200);
  sensor.begin(ssid, contrasenia, servidor_ip, puerto);
}

void loop() {
  sensor.loop();
}








