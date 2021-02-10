#!/usr/bin/python2
import Tkinter as tk

class AlarmWindow(tk.Frame):
    def on_press(self):
        self.quit()

    def __init__(self, parent):
        tk.Frame.__init__(self, parent)
        self.pack()
        self.button = tk.Button(self, text="Stand up and shake a leg!", font=(None, 24), command=self.on_press, width=300, height=300)
        self.button.pack()
        

def display_alarm():
    root = tk.Tk()
    root.title("Get moving!")
    root.geometry("800x600")
    app = AlarmWindow(root)
    app.mainloop()
    app.destroy()
    root.destroy()
    
    
display_alarm()

print("And now for round 2...")

display_alarm()

