# Testes de Telemetria com Ebyte E220 e ESP32

Este repositório contém os testes realizados como parte do Trabalho de Conclusão de Curso (TCC), envolvendo comunicação sem fio e telemetria utilizando os módulos **Ebyte E220** e **ESP32**.

---

## Objetivo

O objetivo deste trabalho é avaliar o desempenho de comunicação entre dispositivos sem fio utilizando diferentes módulos de rádio, analisando métricas como **alcance máximo**, **taxa de entrega de pacotes (Packet Delivery Ratio)** e **estabilidade do sinal**.  

Os testes foram realizados para fornecer dados experimentais que suportem análises de confiabilidade e viabilidade da comunicação para aplicações de telemetria em um veículo baja SAE da equipe Vitória Baja.

---

## Dispositivos Utilizados

- **Ebyte E220** – módulo LoRa para comunicação ponto a ponto.
- **ESP32** – microcontrolador com suporte a comunicação serial, utilizado para interfacear com os módulos de rádio.
- **GY-NEO6MV2** – módulo GPS.

---

## Requisitos

- [Arduino IDE](https://www.arduino.cc/en/software)
- Python 3.10
- Módulo Ebyte E220
- Microcontrolador ESP32

---

## Resultados Esperados

- Gráficos de **RSSI x Distância**
- Taxa de entrega de pacotes por distância
- Comparação de desempenho entre diferentes configurações dos módulos

---

## Contribuição

Este repositório é parte do TCC do autor. Contribuições externas não estão previstas, mas sugestões e comentários são bem-vindos.

---

## Autor

**[Lucas Pereira Taborda]**  
Trabalho de Conclusão de Curso – Avaliação de um ambiente para telemetria de veículos Baja SAE baseado em comunicação LoRa.
Centro Tecnológico - Colegiado do curso de Ciência da Computação - UFES