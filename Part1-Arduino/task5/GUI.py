import tkinter as tk
from tkinter import ttk
import serial
import threading
import time
import serial.tools.list_ports

class PIDControllerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("PID Controller")

        self.serial_port = None
        self.running = False

        self.create_widgets()

    def create_widgets(self):
        self.port_label = tk.Label(self.root, text="Select Port:")
        self.port_label.grid(row=0, column=0)

        self.port_combobox = ttk.Combobox(self.root, values=self.get_serial_ports())
        self.port_combobox.grid(row=0, column=1)

        self.connect_button = tk.Button(self.root, text="Connect", command=self.connect_serial)
        self.connect_button.grid(row=0, column=2)

        self.connection_status = tk.Label(self.root, text="Not Connected", bg="grey")
        self.connection_status.grid(row=0, column=3)

        self.kp_label = tk.Label(self.root, text="Kp")
        self.kp_label.grid(row=1, column=0)
        self.kp_scale = tk.Scale(self.root, from_=0, to=10, resolution=0.1, orient=tk.HORIZONTAL)
        self.kp_scale.grid(row=1, column=1)
        self.kp_button = tk.Button(self.root, text="Set Kp", command=self.update_kp)
        self.kp_button.grid(row=1, column=2)

        self.ki_label = tk.Label(self.root, text="Ki")
        self.ki_label.grid(row=2, column=0)
        self.ki_scale = tk.Scale(self.root, from_=0, to=10, resolution=0.1, orient=tk.HORIZONTAL)
        self.ki_scale.grid(row=2, column=1)
        self.ki_button = tk.Button(self.root, text="Set Ki", command=self.update_ki)
        self.ki_button.grid(row=2, column=2)

        self.kd_label = tk.Label(self.root, text="Kd")
        self.kd_label.grid(row=3, column=0)
        self.kd_scale = tk.Scale(self.root, from_=0, to=10, resolution=0.1, orient=tk.HORIZONTAL)
        self.kd_scale.grid(row=3, column=1)
        self.kd_button = tk.Button(self.root, text="Set Kd", command=self.update_kd)
        self.kd_button.grid(row=3, column=2)

        self.setpoint_label = tk.Label(self.root, text="Set Point")
        self.setpoint_label.grid(row=4, column=0)
        self.setpoint_entry = tk.Entry(self.root)
        self.setpoint_entry.grid(row=4, column=1)
        self.setpoint_button = tk.Button(self.root, text="Set Set Point", command=self.update_setpoint)
        self.setpoint_button.grid(row=4, column=2)

        self.current_position_label = tk.Label(self.root, text="Current Position:")
        self.current_position_label.grid(row=5, column=0)
        self.current_position_value = tk.Label(self.root, text="0%")
        self.current_position_value.grid(row=5, column=1)

        self.setpoint_display_label = tk.Label(self.root, text="Set Point Display:")
        self.setpoint_display_label.grid(row=6, column=0)
        self.setpoint_display_value = tk.Label(self.root, text="0%")
        self.setpoint_display_value.grid(row=6, column=1)

        self.kp_display_label = tk.Label(self.root, text="Kp Display:")
        self.kp_display_label.grid(row=7, column=0)
        self.kp_display_value = tk.Label(self.root, text="0")
        self.kp_display_value.grid(row=7, column=1)

        self.ki_display_label = tk.Label(self.root, text="Ki Display:")
        self.ki_display_label.grid(row=8, column=0)
        self.ki_display_value = tk.Label(self.root, text="0")
        self.ki_display_value.grid(row=8, column=1)

        self.kd_display_label = tk.Label(self.root, text="Kd Display:")
        self.kd_display_label.grid(row=9, column=0)
        self.kd_display_value = tk.Label(self.root, text="0")
        self.kd_display_value.grid(row=9, column=1)

        self.led_label = tk.Label(self.root, text="LED Status:")
        self.led_label.grid(row=10, column=0)
        self.led_status = tk.Label(self.root, text="OFF", bg="grey")
        self.led_status.grid(row=10, column=1)

    def get_serial_ports(self):
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]

    def connect_serial(self):
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
            self.connect_button.config(text="Connect")
            self.connection_status.config(text="Not Connected", bg="grey")
            self.running = False
        else:
            selected_port = self.port_combobox.get()
            if selected_port:
                try:
                    self.serial_port = serial.Serial(selected_port, 115200, timeout=1)
                    self.connect_button.config(text="Disconnect")
                    self.connection_status.config(text="Connected", bg="green")
                    self.running = True
                    self.start_serial_thread()
                except serial.SerialException:
                    self.connection_status.config(text="Connection Failed", bg="red")
            else:
                self.connection_status.config(text="No Port Selected", bg="red")

    def update_kp(self):
        if self.serial_port and self.serial_port.is_open:
            value = self.kp_scale.get()
            command = f"p={value} \n"
            print(f"Sending command: {command}")
            self.serial_port.write(command.encode())

    def update_ki(self):
        if self.serial_port and self.serial_port.is_open:
            value = self.ki_scale.get()
            command = f"i={value} \n"
            print(f"Sending command: {command}")
            self.serial_port.write(command.encode())

    def update_kd(self):
        if self.serial_port and self.serial_port.is_open:
            value = self.kd_scale.get()
            command = f"d={value} \n"
            print(f"Sending command: {command}")
            self.serial_port.write(command.encode())

    def update_setpoint(self):
        if self.serial_port and self.serial_port.is_open:
            value = self.setpoint_entry.get()
            try:
                setpoint = float(value)
                if 0 <= setpoint <= 100:
                    scaled_value = int(setpoint)
                    command = f"s={scaled_value} \n"
                    print(f"Sending command: {command}")
                    self.serial_port.write(command.encode())
                else:
                    print("Setpoint out of range (0-100)")
            except ValueError:
                print("Invalid setpoint value")

    def start_serial_thread(self):
        thread = threading.Thread(target=self.serial_thread)
        thread.daemon = True
        thread.start()

    def serial_thread(self):
        while self.running:
            try:
                line = self.serial_port.readline().decode('utf-8').strip()
                if line:
                    data = line.split(",")
                    if len(data) == 5:
                        set_point, curr_pos, kp, ki, kd = data
                        self.setpoint_display_value.config(text=f"{set_point}%")
                        self.current_position_value.config(text=f"{curr_pos}%")
                        self.kp_display_value.config(text=kp)
                        self.ki_display_value.config(text=ki)
                        self.kd_display_value.config(text=kd)
            except serial.SerialException:
                print("Serial read error")
            time.sleep(0.01)

    def on_closing(self):
        self.running = False
        if self.serial_port:
            self.serial_port.close()
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = PIDControllerApp(root)
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    root.mainloop()
