from PIL import Image
import time
import serial

def main():
    image = Image.open('get.jpg')
    width, height = image.size
    ser = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=0.0)
    if ser.is_open:
        print(f"Port {ser.name} opened")

    try:
        ser.write("disp_screen \n".encode('ascii'))
        time.sleep(0.1)
        for x in range(width):
            for y in range(height):
                  st = "disp_px "
                  c = image.getpixel((x, y))
                  r = str(hex(c[0]))[2:]
                  if len(r) == 1:
                       r = "0"+r
                  g = str(hex(c[1]))[2:]
                  if len(g) == 1:
                       g = "0"+g
                  b = str(hex(c[2]))[2:]
                  if len(b) == 1:
                       b = "0"+b
                  st = st + str(x) + " " + str(y) + " " + r+g+b+"\n"
                  #print(st)
                  ser.write(st.encode('ascii'))
                
    finally:
        time.sleep(0.1)
        ser.close()
        print("Port closed")

if __name__ == "__main__":
	main()


