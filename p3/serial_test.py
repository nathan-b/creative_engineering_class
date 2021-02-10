#!/usr/bin/python2

import serial

class LightSensor:
    is_dark = False
    hysteresis = 0
    hist_wait = 10

    def new_val(self, val):
        new_dark = False
        changed = False
        if val == "dark":
            new_dark = True
        elif val == "light":
            new_dark = False

        if new_dark != self.is_dark:
            self.hysteresis += 1
            if self.hysteresis == self.hist_wait:
                if new_dark:
                    print("> dark")
                else:
                    print("> light")
                self.is_dark = new_dark
                self.hysteresis = 0
                changed = True
        else:
            self.hysteresis = 0

        return changed

    def get_dark_status(self):
        return self.is_dark

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.flush()
    ls = LightSensor()
    while True:
        if ser.in_waiting > 0:
            status = ser.readline().decode('utf-8', 'replace').rstrip()
            if ls.new_val(status):
                if ls.get_dark_status():
                    print("It got dark")
                else:
                    print("It got light")


