import tkinter as tk
from tkinter import ttk, scrolledtext
import serial
import threading
import time
import serial.tools.list_ports

class PIDControllerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("PID Controller")

        self.serial_port_manager = SerialPortManager(self)
        self.running = False

        self.create_widgets()
        self.refresh_ports()

    def create_widgets(self):
        self.port_label = tk.Label(self.root, text="Select Port:")
        self.port_label.grid(row=0, column=0)

        self.port_combobox = ttk.Combobox(self.root)
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

        self.canvas = tk.Canvas(self.root, width=200, height=200, bg="white")
        self.canvas.grid(row=0, column=4, rowspan=11)
        self.arc = self.canvas.create_arc(50, 50, 150, 150, start=90, extent=0, outline="blue", width=2)
        self.set_point_arc = self.canvas.create_arc(50, 50, 150, 150, start=90, extent=0, outline="red", width=2)

        # Add scrolled text box for serial output
        self.text_box = scrolledtext.ScrolledText(self.root, width=50, height=10, state='disabled')
        self.text_box.grid(row=11, column=0, columnspan=5, padx=10, pady=10)

    def refresh_ports(self):
        ports = self.get_serial_ports()
        self.port_combobox['values'] = ports
        self.root.after(1000, self.refresh_ports)  # 每隔1秒刷新一次端口列表

    def get_serial_ports(self):
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]

    def connect_serial(self):
        if self.serial_port_manager.is_running:
            self.disconnect_serial()
        else:
            selected_port = self.port_combobox.get()
            if selected_port:
                try:
                    self.serial_port_manager.set_name(selected_port)
                    self.serial_port_manager.set_baud(115200)
                    self.serial_port_manager.start()
                    self.connect_button.config(text="Disconnect")
                    self.connection_status.config(text="Connected", bg="green")
                    self.running = True
                    self.recursive_update_textbox()
                    self.read_initial_data()
                except serial.SerialException:
                    self.connection_status.config(text="Connection Failed", bg="red")
            else:
                self.connection_status.config(text="No Port Selected", bg="red")

    def disconnect_serial(self):
        self.serial_port_manager.stop()
        self.connect_button.config(text="Connect")
        self.connection_status.config(text="Not Connected", bg="grey")
        self.running = False

    def update_kp(self):
        if self.serial_port_manager.is_running:
            value = self.kp_scale.get()
            command = f"p={value}\n"
            print(f"Sending command: {command}")
            self.serial_port_manager.write(command.encode())

    def update_ki(self):
        if self.serial_port_manager.is_running:
            value = self.ki_scale.get()
            command = f"i={value}\n"
            print(f"Sending command: {command}")
            self.serial_port_manager.write(command.encode())

    def update_kd(self):
        if self.serial_port_manager.is_running:
            value = self.kd_scale.get()
            command = f"d={value}\n"
            print(f"Sending command: {command}")
            self.serial_port_manager.write(command.encode())

    def update_setpoint(self):
        if self.serial_port_manager.is_running:
            value = self.setpoint_entry.get()
            try:
                setpoint = float(value)
                if 0 <= setpoint <= 100:
                    scaled_value = int(setpoint)
                    command = f"s={scaled_value}\n"
                    print(f"Sending command: {command}")
                    self.serial_port_manager.write(command.encode())
                else:
                    print("Setpoint out of range (0-100)")
            except ValueError:
                print("Invalid setpoint value")

    def read_initial_data(self):
        line = self.serial_port_manager.read_line().decode('utf-8').strip()
        if line:
            print(f"Initial data: {line}")  # Debug print
            data = line.split(",")
            if len(data) == 6:
                set_point, curr_pos, kp, ki, kd, ready = data
                self.setpoint_display_value.config(text=f"{set_point}%")
                self.current_position_value.config(text=f"{curr_pos}%")
                self.kp_display_value.config(text=kp)
                self.ki_display_value.config(text=ki)
                self.kd_display_value.config(text=kd)
                if ready == "1":
                    self.led_status.config(text="ON", bg="green")
                else:
                    self.led_status.config(text="OFF", bg="yellow")
                self.update_arc(set_point, curr_pos)

    def update_arc(self, set_point, curr_pos):
        # 更新红色圆弧
        set_point_extent = (float(set_point) / 100) * 360
        self.canvas.itemconfig(self.set_point_arc, extent=set_point_extent)

        # 更新蓝色圆弧
        current_pos_extent = (float(curr_pos) / 100) * 360
        self.canvas.itemconfig(self.arc, extent=current_pos_extent)

    def recursive_update_textbox(self):
        serial_port_buffer = self.serial_port_manager.read_buffer()
        if serial_port_buffer:  # Ensure there's something to decode and insert
            self.text_box.config(state='normal')
            self.text_box.insert(tk.END, serial_port_buffer.decode("ascii"))
            self.text_box.see(tk.END)
            self.text_box.config(state='disabled')
        if self.serial_port_manager.is_running:
            self.root.after(100, self.recursive_update_textbox)

    def on_closing(self):
        self.running = False
        if self.serial_port_manager.is_running:
            self.serial_port_manager.stop()
        self.root.destroy()


class SerialPortManager:
    def __init__(self, app):
        self.is_running = False
        self.serial_port_name = None
        self.serial_port_baud = 9600
        self.serial_port = serial.Serial()
        self.serial_port_buffer = bytearray()
        self.line_buffer = ""
        self.app = app

    def set_name(self, serial_port_name):
        self.serial_port_name = serial_port_name

    def set_baud(self, serial_port_baud):
        self.serial_port_baud = serial_port_baud

    def start(self):
        self.is_running = True
        self.serial_port_thread = threading.Thread(target=self.thread_handler)
        self.serial_port_thread.start()

    def stop(self):
        self.is_running = False

    def thread_handler(self):
        while self.is_running:
            if not self.serial_port.isOpen():
                self.serial_port = serial.Serial(
                    port=self.serial_port_name,
                    baudrate=self.serial_port_baud,
                    bytesize=8,
                    timeout=2,
                    stopbits=serial.STOPBITS_ONE,
                )
            else:
                while self.serial_port.in_waiting > 0:
                    serial_port_byte = self.serial_port.read(1)
                    self.serial_port_buffer.append(int.from_bytes(serial_port_byte, byteorder='big'))
                    self.line_buffer += serial_port_byte.decode('utf-8')

                    if '\n' in self.line_buffer:
                        lines = self.line_buffer.split('\n')
                        for line in lines[:-1]:
                            self.update_app(line.strip())
                        self.line_buffer = lines[-1]

        if self.serial_port.isOpen():
            self.serial_port.close()

    def update_app(self, data_line):
        if data_line:
            print(f"Update app with data: {data_line}")  # Debug print
            data = data_line.split(",")
            if len(data) == 6:
                set_point, curr_pos, kp, ki, kd, ready = data
                self.app.setpoint_display_value.config(text=f"{set_point}%")
                self.app.current_position_value.config(text=f"{curr_pos}%")
                self.app.kp_display_value.config(text=kp)
                self.app.ki_display_value.config(text=ki)
                self.app.kd_display_value.config(text=kd)
                if ready == "1":
                    self.app.led_status.config(text="ON", bg="green")
                else:
                    self.app.led_status.config(text="OFF", bg="yellow")
                self.app.update_arc(set_point, curr_pos)

    def read_buffer(self):
        buffer = self.serial_port_buffer
        self.serial_port_buffer = bytearray()
        return buffer

    def write(self, data):
        if self.serial_port.isOpen():
            self.serial_port.write(data)

    def read_line(self):
        if self.serial_port.isOpen():
            return self.serial_port.readline()
        return b''

    def main_process(self, input_byte):
        try:
            character = input_byte.decode("ascii")
        except UnicodeDecodeError:
            pass
        else:
            print(character, end="")


if __name__ == "__main__":
    root = tk.Tk()
    app = PIDControllerApp(root)
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    root.mainloop()
