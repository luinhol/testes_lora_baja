unsigned long t0;
int package = 0;
char buffer [64];
unsigned long elapsed_time = 0;

void setup() {
  Serial.begin(9600);  // Comunicacao com o modulo atraves dos pinos rx e tx
  t0 = millis(); // Adquire tempo inicial ao iniciar o microcontrolador
}

// HEADER = 2 byrtes (0xAA, 0x55)
// ID = 1 byte
// package = 4 bytes
// elapsed_time = 4 bytes
// checksum = 1 byte
// TOTAL = 12 bytes

byte header[2] = {0xAA, 0x55};
byte id = 3;


void loop() {

  if (millis() - t0 >= 1000) { // Adquire a diferenca do tempo atual para o tempo anterior

    elapsed_time = millis();
    // calculo do byte de checksum
    byte checksum = (id + package + elapsed_time) & 0xFF;

    // envio da informacao campo a campo via serial
    Serial.write(header, 2);
    Serial.write(id);
    Serial.write((uint8_t*)&package, 4);
    Serial.write((uint8_t*)&elapsed_time, 4);
    Serial.write(checksum);

    package++; // Incrementa o pacote
    t0 += 1000; // Incrementa o valor de tempo para o proximo pacote
  }
}

