import os
import subprocess

write_pipe_path = "/tmp/python_to_c"
read_pipe_path = "/tmp/c_to_python"

# Start the C program as a subprocess
fwif = subprocess.Popen(
    ["./fwif"],
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
)

# NOTE: The C program will create the named pipes

# Open the named pipe for writing
try:
    os.mkfifo(write_pipe_path)
except FileExistsError:
    pass
write_pipe_fd = os.open(write_pipe_path, os.O_WRONLY)
print("Main: Named pipe opened successfully for writing")

# Open the named pipe for reading
read_pipe_fd = os.open(read_pipe_path, os.O_RDONLY)
print("Main: Named pipe opened successfully for reading")


class App:
    def __init__(self, name):
        self.name = name
        self.components = []
        self.padding = 10

    def add_component(self):
        component = {
            "x": self.padding,
            "y": len(self.components) * (50 + self.padding),
            "width": 100,
            "height": 50,
        }
        self.components.append(component)

    def delete_component(self):
        self.components.pop()

    def view(self):
        return {
            "components": self.components,
        }

    def handle_key(self, key):
        if key == b"a":
            self.add_component()
        elif key == b"d":
            self.delete_component()


app = App("main")

while True:
    # Read from pipe
    read_data = os.read(read_pipe_fd, 1024)

    # If window is closed
    if read_data == b'':
        print("Python: No data received")
        break

    # If "q" is pressed
    if read_data == b'q':
        print("Python: Quit event received")
        break

    app.handle_key(read_data)

    # Write application view data to pipe
    write_data = str(app.view())
    os.write(write_pipe_fd, write_data.encode())


# Close the C program
c_program.kill()

# Close the named pipe
os.close(write_pipe_fd)
print("Python: Named pipe closed successfully for writing")

# Close the named pipe
os.close(read_pipe_fd)
print("Python: Named pipe closed successfully for reading")
