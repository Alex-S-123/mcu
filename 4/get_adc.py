import time
import serial
import matplotlib.pyplot as plt

def read_value(ser):
    while True:
        try:
            line = ser.readline().decode('ascii')
            value = float(line)
            return value
        except ValueError:
            continue


def main():
    ser = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=0.0)
    if ser.is_open:
        print(f"Port {ser.name} opened")

    measure_temperature_C = []
    measure_pres = []
    measure_hum = []
    measure_ts = []
    start_ts = time.time()
    try:
	    while True:
                ts = time.time() - start_ts
                ser.write("press_si\n".encode('ascii'))
                voltage_V = read_value(ser)
                ser.write("temp_si\n".encode('ascii'))
                temp_C = read_value(ser)
                ser.write("hum_raw\n".encode('ascii'))
                hum = read_value(ser)
                measure_ts.append(ts)
                measure_pres.append(voltage_V)
                measure_temperature_C.append(temp_C)
                measure_hum.append(hum/50000*100)
                print(f'{voltage_V} P - {temp_C}C - {hum/500}H - {ts:.2f}s')
                time.sleep(0.1)
    finally:
        ser.close()
        print("Port closed")
        plt.subplot(3, 1, 1)
        plt.plot(measure_ts, measure_pres)
        plt.title('pres')
        plt.xlabel('время, с')
        plt.ylabel('p, GPa')

        plt.subplot(3, 1, 2)
        plt.plot(measure_ts, measure_temperature_C)
        plt.title('График зависимости температуры от времени')
        plt.xlabel('время, с')
        plt.ylabel('температура, C')

        plt.subplot(3, 1, 3)
        plt.plot(measure_ts, measure_hum)
        plt.title('hum')
        plt.xlabel('время, с')
        plt.ylabel('hum, %')


        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
	main()