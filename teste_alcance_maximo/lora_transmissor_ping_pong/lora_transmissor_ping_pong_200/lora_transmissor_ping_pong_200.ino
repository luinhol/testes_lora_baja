#define GPS_BAUD 9600
#define LORA_BAUD 9600
#define RXD2 16
#define TXD2 17
#define CMD_PING 0x01
#define PCKTSIZE 200

#include <TinyGPS++.h>

// objetos para comunicacao com o GPS
TinyGPSPlus gps;

unsigned long lastSend;
uint8_t frame[PCKTSIZE];
uint8_t rxBuf[PCKTSIZE+1];

// variaveis do pacote
byte header[2] = {0xAA, 0x55};  // 2 bytes
byte id = 3;                    // 1 byte
uint32_t package = 0;           // 4 bytes
uint8_t checksum = 0;           // 1 byte
                                // TOTAL HEADER: 8 bytes

// variaveis do carro
uint32_t elapsed_time = 0;      // 4 bytes
float velocidade = 0.0;
uint16_t velocidade_i;              // 2 bytes
float frequencia_vela = 0.0;
uint16_t frequencia_vela_i;         // 2 bytes
uint8_t temperatura = 0;            // 1 bytes
uint8_t fluido_de_freio = 0;        // 1 byte
float ax = 0.0;
float ay = 0.0;
float az = 0.0;
int16_t ax_i, ay_i, az_i;           // 6 bytes

float roll = 0.0;
float pitch = 0.0;
float heading = 0.0;
int16_t roll_i, pitch_i, heading_i; // 6 bytes
                                    // TOTAL CARRO: 22 bytes

// variaveis do gps
float lat = 0.0;
float lon = 0.0;
float alt = 0.0;
int32_t lat_i, lon_i;           // 8 bytes
int16_t alt_i;                  // 2 bytes

uint16_t velocidade_gps;         // 2 bytes

uint8_t hour_gps = 0;           // 1 byte
uint8_t minute_gps = 0;         // 1 byte
uint8_t second_gps = 0;         // 1 byte
uint8_t cent_gps = 0;           // 1 byte

uint8_t satellites = 0;         // 1 byte
uint8_t hdop = 0;               // 1 byte
                                // TOTAL GPS: 18 bytes
// TOTAL DADOS: 40 bytes

void enviarPacote(){
  elapsed_time = millis();

  // GPS
  lat = gps.location.lat();
  lon = gps.location.lng();
  velocidade = gps.speed.kmph();
  velocidade_i = (uint16_t)(velocidade * 100.0); // conversao para int
  velocidade_gps = (uint16_t)(velocidade * 100.0); // conversao para int
  alt = gps.altitude.meters();
  hdop = gps.hdop.value();                        // dividir por 10 no receptor
  satellites = gps.satellites.value();
  hour_gps = gps.time.hour();
  minute_gps = gps.time.minute();
  second_gps = gps.time.second();
  cent_gps = gps.time.centisecond();

  uint8_t idx = 0;
  
  // header
  frame[idx++] = 0xAA;           // 1 byte
  frame[idx++] = 0x55;           // 1 byte
  frame[idx++] = id;             // 1 byte

  // package
  memcpy(&frame[idx], &package, 4);
  idx += 4;

  // elapsed time
  memcpy(&frame[idx], &elapsed_time, 4);
  idx += 4;

  // variaveis do carro
  memcpy(&frame[idx], &velocidade_i, 2);
  idx += 2;
  frequencia_vela_i = (uint16_t)(frequencia_vela * 10.0); // conversao para int
  memcpy(&frame[idx], &frequencia_vela_i, 2);
  idx += 2;
  frame[idx++] = temperatura;
  frame[idx++] = fluido_de_freio;

  ax_i = (int16_t)(ax * 1000.0); // conversao para int
  ay_i = (int16_t)(ay * 1000.0); // conversao para int
  az_i = (int16_t)(az * 1000.0); // conversao para int
  memcpy(&frame[idx], &ax_i, 2);
  idx += 2;
  memcpy(&frame[idx], &ay_i, 2);
  idx += 2;
  memcpy(&frame[idx], &az_i, 2);
  idx += 2;

  roll_i = (int16_t)(roll * 100.0); // conversao para int
  pitch_i = (int16_t)(pitch * 100.0); // conversao para int
  heading_i = (int16_t)(heading * 100.0); // conversao para int
  memcpy(&frame[idx], &roll_i, 2);
  idx += 2;
  memcpy(&frame[idx], &pitch_i, 2);
  idx += 2;
  memcpy(&frame[idx], &heading_i, 2);
  idx += 2;

  lat_i = (int32_t)(lat * 10000000.0); // conversao para int
  lon_i = (int32_t)(lon * 10000000.0); // conversao para int
  alt_i = (int16_t)(alt * 10.0); // conversao para int
  memcpy(&frame[idx], &lat_i, 4);
  idx += 4;
  memcpy(&frame[idx], &lon_i, 4);
  idx += 4;
  memcpy(&frame[idx], &alt_i, 2);
  idx += 2;
  memcpy(&frame[idx], &velocidade_i, 2);
  idx += 2;

  memcpy(&frame[idx], &hour_gps, 1);
  idx += 1;
  memcpy(&frame[idx], &minute_gps, 1);
  idx += 1;
  memcpy(&frame[idx], &second_gps, 1);
  idx += 1;
  memcpy(&frame[idx], &cent_gps, 1);
  idx += 1;

  frame[idx++] = satellites;
  frame[idx++] = hdop;

  // checksum
  checksum = 0;
  for (uint8_t i = 0; i < idx; i++) {
    checksum += frame[i];
  }

  frame[idx++] = checksum;

  // padding dos ultimos 16 bytes
  while (idx < PCKTSIZE) {
    frame[idx++] = 0x00;
  }

  // envio
  Serial2.write(frame, PCKTSIZE);


  package++; // incrementa o pacote
}

void setup() {
  Serial2.begin(LORA_BAUD, SERIAL_8N1, RXD2, TXD2);  // comunicacao com o modulo LoRa atraves dos pinos rx e tx
  Serial.begin(GPS_BAUD);
  lastSend = 0; // adquire tempo inicial ao iniciar o microcontrolador
}

void loop() {

  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (Serial2.available() >= PCKTSIZE+1) {
    Serial2.readBytes(rxBuf, PCKTSIZE+1);
    if (rxBuf[0] != 0xAA || rxBuf[1] != 0x55) {
      return;
    }

    uint8_t cmd = rxBuf[2];

    if (cmd == CMD_PING) {
      lastSend = millis();
      enviarPacote();
    }

    lastSend = millis();
  }
}