import struct
import serial
import time
import os
from datetime import datetime

FRAME_SIZE = 129

# comando para verificar portas COM (USB)
# python -m serial.tools.list_ports
port = "COM11"
baud = 9600 # baud rate do modulo ebyte E220

# abertura da comunicacao serial
ser = serial.Serial(port, baud, timeout=1)

# criacao do arquivo e escrita do cabecalho
dir = "./passagens/"
file_name = "one_way_128_" + time.strftime("%H%M%S")

if not os.path.isdir(dir):
    os.mkdir(dir)
    
f = open(dir+file_name, "w")
f.write(
    "PACKAGE,ELAPSED_TIME,"
    "HOUR_GPS,MIN_GPS,SEC_GPS,CENT_GPS,"
    "HOUR_PC,MIN_PC,SEC_PC,CENT_PC,"
    "VEL_RODA,ROTACAO,TEMP,FLUIDO,"
    "AX,AY,AZ,"
    "ROLL,PITCH,HEADING,"
    "LAT,LON,ALT,VEL_GPS,"
    "SAT,HDOP,RSSI,CHECKSUM\n"
)

# loop de execucao do codigo
while True:
    package = ser.read(FRAME_SIZE)

    if len(package) == FRAME_SIZE:
        # unpack na estrutura recebida, com os respectivos tamanho de cada dado
        agora = datetime.now()

        # hora do computador
        hora_pc = agora.hour
        minuto_pc = agora.minute
        segundo_pc = agora.second
        cent_pc = agora.microsecond / 10000

        unpacked = struct.unpack(
            "<2sBII"      # header, id, package, elapsed_time (11 bytes)
            "HHBB"        # velocidade_roda, rotacao_motor, temperatura, fluido_freio (6 bytes)
            "hhh"         # aceleracao_x, aceleracao_y, aceleracao_z (6 bytes)
            "hhh"         # roll, pitch, heading (6 bytes)
            "iihH"        # lat, lon, alt, velocidade_gps (12 bytes)
            "BBBB"        # hour_gps, minute_gps, second_gps, cent_gps (4 bytes)
            "BB"          # gps, hdop (2 bytes)
            "B"           # checksum (1 byte)
            "xxxxxxxxxxxxxxxx" # padding (16 bytes)
            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" # padding (64 bytes)
            # "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" # padding (64 bytes)
            "B",          # rssi (1 byte)
            package       # TOTAL (65 bytes)
        )

        (
            header, id, package, elapsed_time,
            velocidade, rotacao_motor, temperatura, fluido_de_freio,
            aceleracao_x, aceleracao_y, aceleracao_z,
            roll, pitch, heading,
            lat, lon, alt, vel_gps,
            hour_gps, minute_gps, second_gps, cent_gps,
            gps, hdop,
            checksum,
            rssi
        ) = unpacked
        
        # verificacao de mensagem iniciada com header
        if header != b'\xAA\x55':
            continue
        
        # correcao de escala dos valores
        velocidade_kmh = velocidade / 100.0
        velocidade_gps = velocidade / 100.0
        rotacao_motor = rotacao_motor / 10.0
        lat_deg = lat / 1e7
        lon_deg = lon / 1e7
        alt_m = alt / 10.0

        # print no terminal
        print(
            f"PKG={package} | "
            f"T={elapsed_time} | "
            f"HOR_GPS={hour_gps:02}:{minute_gps:02}:{second_gps:02} | "
            f"HOR_PC={hora_pc:02}:{minuto_pc:02}:{segundo_pc:02} | "
            f"VEL={velocidade_kmh:.2f} | "
            f"ROT={rotacao_motor:.1f} | "
            f"TEMP={temperatura} | "
            f"LAT={lat_deg:.6f} | "
            f"LON={lon_deg:.6f} | "
            f"ALT={alt_m:.1f} | "
            f"SAT={gps} | "
            f"HDOP={hdop} | "
            f"RSSI={rssi}"
        )

        # eescrita em arquivo
        f.write(
            f"{package},{elapsed_time},"
            f"{hour_gps},{minute_gps},{second_gps},{cent_gps},"
            f"{hora_pc},{minuto_pc},{segundo_pc},{cent_pc},"
            f"{velocidade},{rotacao_motor},{temperatura},{fluido_de_freio},"
            f"{aceleracao_x},{aceleracao_y},{aceleracao_z},"
            f"{roll},{pitch},{heading},"
            f"{lat},{lon},{alt},{vel_gps},"
            f"{gps},{hdop},{rssi},{checksum}\n"
        )
        f.flush()