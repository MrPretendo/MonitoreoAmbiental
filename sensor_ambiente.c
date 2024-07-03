#include <SPI.h>
#include <Ethernet.h>

#define DHTPIN 2          // Pin digital conectado al sensor DHT
#define MQ135PIN A0       // Pin analógico conectado al sensor MQ135
#define MAX_TIMINGS 85    // Máximo número de timings para leer DHT

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192,168,1,100);  // IP del servidor (reemplaza con la dirección correcta)
EthernetClient client;

struct DHT_Data {
  float temperature;
  float humidity;
};

DHT_Data readDHT() {
  DHT_Data result = {NAN, NAN};
  uint8_t data[5] = {0, 0, 0, 0, 0};
  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0;

  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, LOW);
  delay(18);
  
  pinMode(DHTPIN, INPUT_PULLUP);
  
  for (int i = 0; i < MAX_TIMINGS; i++) {
    counter = 0;
    while (digitalRead(DHTPIN) == laststate) {
      counter++;
      delayMicroseconds(1);
      if (counter == 255) break;
    }
    laststate = digitalRead(DHTPIN);
    if (counter == 255) break;
    if ((i >= 4) && (i % 2 == 0)) {
      data[j / 8] <<= 1;
      if (counter > 16) data[j / 8] |= 1;
      j++;
    }
  }

  if (j >= 40) {
    uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    if (data[4] == checksum) {
      result.humidity = (data[0] * 256 + data[1]) / 10.0f;
      result.temperature = ((data[2] & 0x7F) * 256 + data[3]) / 10.0f;
      if (data[2] & 0x80) result.temperature *= -1;
    }
  }

  return result;
}

float readMQ135() {
  int sensorValue = analogRead(MQ135PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  return voltage * 100; // Valor aproximado en ppm
}

void setup() {
  Serial.begin(9600);
  
  Ethernet.begin(mac);
  delay(1000);
  Serial.println("Conectado a la red");
}

void loop() {
  DHT_Data dhtData = readDHT();
  float airQuality = readMQ135();

  if (isnan(dhtData.humidity) || isnan(dhtData.temperature) || isnan(airQuality)) {
    Serial.println("Error leyendo los sensores");
    return;
  }

  if (client.connect(server, 80)) {
    Serial.println("Conectado al servidor");
    
    String postData = "temperature=" + String(dhtData.temperature) + 
                      "&humidity=" + String(dhtData.humidity) + 
                      "&airQuality=" + String(airQuality);
    
    client.println("POST /api/sensor-data HTTP/1.1");
    client.println("Host: 192.168.1.100");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);
  } else {
    Serial.println("Fallo en la conexión");
  }

  if (client.connected()) {
    client.stop();
  }

  delay(60000);  // Espera 1 minuto antes de la próxima lectura
}