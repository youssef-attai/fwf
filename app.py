import random
import json
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


class Color:
    def __init__(self, r: int, g: int, b: int):
        self.r: int = r
        self.g: int = g
        self.b: int = b


class Component:
    def __init__(
            self,
            x: int,
            y: int,
            width: int,
            height: int,
            background: Color,
            foreground: Color,
            border_color: Color,
            border_width: int,
            text: str
    ):
        self.x: int = x
        self.y: int = y
        self.width: int = width
        self.height: int = height
        self.background: Color = background
        self.foreground: Color = foreground
        self.border_color: Color = border_color
        self.border_width: int = border_width
        self.text: str = text
        self.children: list['Component'] = []

    def add_child(self, child):
        self.children.append(child)

    def remove_child(self, child):
        self.children.remove(child)

    def to_json(self):
        return {
            "x": self.x,
            "y": self.y,
            "width": self.width,
            "height": self.height,
            "background_color": {
                "r": self.background.r,
                "g": self.background.g,
                "b": self.background.b
            },
            "foreground_color": {
                "r": self.foreground.r,
                "g": self.foreground.g,
                "b": self.foreground.b
            },
            "border_color": {
                "r": self.border_color.r,
                "g": self.border_color.g,
                "b": self.border_color.b
            },
            "border_width": self.border_width,
            "text": self.text,
            "children": [child.to_json() for child in self.children]
        }


class KeyBindings:
    def __init__(self):
        self.__bindings = {}

    def add(self, key, function):
        self.__bindings[key] = function

    def remove(self, key):
        if key in self.__bindings:
            del self.__bindings[key]

    def function(self, key):
        return self.__bindings.get(key)

    def any_starts_with_except(self, key):
        for k in self.__bindings.keys():
            if k.startswith(key) and k != key:
                return True
        return False


class PressedBuffer:
    def __init__(self):
        self.__buffer = ""

    def press(self, key):
        self.__buffer += key

    def clear(self):
        self.__buffer = ""

    @ property
    def keys(self):
        return self.__buffer


class BaseApp:
    def __init__(self):
        self.components = []

        self.keybindings = KeyBindings()
        self.setup_keybindings()

    def view(self):
        return {"components": [component.to_json() for component in self.components]}

    def setup_keybindings(self):
        self.keybindings.add('q', self.quit)

    def run(self):
        while True:
            # Read from pipe
            print("Main: Waiting for data from pipe")
            read_data = os.read(read_pipe_fd, 1024)

            # If window is closed
            if read_data == b'':
                self.quit()

            # Decode the data
            try:
                data = read_data.decode()
                print("Main: Data received from pipe: " + data)
                json_data = json.loads(data)

                # If the data is a key press
                if "key" in json_data:
                    key = json_data["key"]

                    # If the key is in the key bindings
                    func = self.keybindings.function(key)
                    if func is not None:
                        func()

            except UnicodeDecodeError:
                pass

            # Write application view data to pipe
            write_data = str(self.view())
            os.write(write_pipe_fd, write_data.encode())

    def quit(self):
        print("Main: Quitting")
        fwif.kill()
        os.close(write_pipe_fd)
        os.close(read_pipe_fd)
        exit(0)


class App(BaseApp):
    def __init__(self):
        super().__init__()
        self.selected_index = -1

    def setup_keybindings(self):
        super().setup_keybindings()
        self.keybindings.add('a', self.add_item)
        self.keybindings.add('d', self.delete_all)
        self.keybindings.add('dd', self.delete_item)
        self.keybindings.add('x', self.delete_item)
        self.keybindings.add('dG', self.delete_from_selection_to_last)
        self.keybindings.add('dgg', self.delete_from_selection_to_first)
        self.keybindings.add('gg', self.select_first)
        self.keybindings.add('G', self.select_last)
        self.keybindings.add('k', self.move_selection_up)
        self.keybindings.add('j', self.move_selection_down)
        self.keybindings.add('K', self.move_item_up)
        self.keybindings.add('J', self.move_item_down)
        self.keybindings.add('c', self.change_color)

    def change_color(self):
        if self.selected_index >= 0 and len(self.components) > 0:
            component = self.components[self.selected_index]
            component.background = self.random_color()
            self.update_items()

    def delete_from_selection_to_first(self):
        if self.selected_index >= 0 and len(self.components) > 0:
            for _ in range(self.selected_index + 1):
                self.components.pop(0)
            self.selected_index = 0
            self.update_items()

    def delete_from_selection_to_last(self):
        if self.selected_index >= 0 and len(self.components) > 0:
            for _ in range(self.selected_index, len(self.components)):
                self.components.pop()
            self.selected_index = min(
                self.selected_index, len(self.components) - 1)
            self.update_items()

    def select_first(self):
        if len(self.components) > 0:
            self.selected_index = 0
            self.update_items()

    def select_last(self):
        if len(self.components) > 0:
            self.selected_index = len(self.components) - 1
            self.update_items()

    def add_item(self):
        item = Component(
            x=10,
            y=30 * (len(self.components) + 1),
            width=200,
            height=20,
            background=self.random_color(),
            foreground=Color(0, 0, 0),
            border_color=Color(0, 0, 0),
            border_width=1,
            text=""
        )
        self.components.append(item)
        self.selected_index = len(self.components) - 1
        self.update_items()

    def delete_all(self):
        self.components = []
        self.selected_index = -1
        self.update_items()

    def delete_item(self):
        if self.selected_index >= 0 and len(self.components) > 0:
            self.components.pop(self.selected_index)
            self.selected_index = min(
                self.selected_index, len(self.components) - 1)
            self.update_items()

    def move_selection_down(self):
        if self.selected_index < len(self.components) - 1:
            self.selected_index += 1
            self.update_items()

    def move_selection_up(self):
        if self.selected_index > 0:
            self.selected_index -= 1
            self.update_items()

    def move_item_down(self):
        if self.selected_index < len(self.components) - 1:
            self.components[
                self.selected_index
            ], self.components[
                self.selected_index + 1
            ] = self.components[
                self.selected_index + 1
            ], self.components[
                self.selected_index
            ]

            self.selected_index += 1
            self.update_items()

    def move_item_up(self):
        if self.selected_index > 0:
            self.components[
                self.selected_index
            ], self.components[
                self.selected_index - 1
            ] = self.components[
                self.selected_index - 1
            ], self.components[
                self.selected_index
            ]

            self.selected_index -= 1
            self.update_items()

    def update_items(self):
        for i, component in enumerate(self.components):
            component: Component
            component.x = 10
            component.y = 30 * (i + 1)
            if i == self.selected_index:
                component.border_width = 5
            else:
                component.border_width = 0

    def random_color(self):
        r = random.randint(0, 255)
        g = random.randint(0, 255)
        b = random.randint(0, 255)

        return Color(r, g, b)
