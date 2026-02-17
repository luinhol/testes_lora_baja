import struct
import serial
import time

# comando para verificar portas COM (USB)
# python -m serial.tools.list_ports
port = "COM11"
baud = 9600 # baud rate do modulo ebyte E220

# abertura da comunicacao serial
ser = serial.Serial(port, baud, timeout=1)

# criacao do arquivo e escrita do cabecalho
dir = "./passagens/"
file_name = "leitura_serial_" + time.strftime("%H%M%S")
f = open(dir+file_name, "w")
f.write("PACKAGE, ELAPSED_TIME, RSSI\n")

# loop de execucao do codigo
while True:
    # campos de dados a serem lidos
    # HEADER = 2 byrtes (0xAA, 0x55)
    # ID = 1 byte
    # package = 4 bytes
    # elapsed_time = 4 bytes
    # checksum = 1 byte
    # rssi = 1 byte
    # TOTAL = 13 bytes
    data = ser.read(13)

    if len(data) == 13:
        # unpack na estrutura recebida, com os respectivos tamanho de cada dado
        header, id, package, elapsed_time, checksum, rssi = struct.unpack("<2sBiLBB", data)

        # print no terminal
        print(header, id, package, elapsed_time, checksum, rssi)
        # escrita no arquivo
        f.write(str(package) + ", " + str(elapsed_time) + ", " + str(rssi) + "\n")