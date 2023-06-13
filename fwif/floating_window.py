import tkinter as tk
from .keybindings import KeyBindings
from .component import Component


class FloatingWindow:
    def __init__(self, width=400, height=400, bg="white"):
        self.__root = tk.Tk()
        self.__root.tk.call('wm', 'attributes', '.', '-type', 'dialog')
        self.__root.bind("<FocusOut>", self.__on_focus_out)
        self.__root.bind("<KeyPress>", self.__on_key_press)

        self.__canvas = tk.Canvas(
            self.__root,
            width=width,
            height=height,
            bg=bg,
            highlightthickness=0
        )
        self.__canvas.pack()

        self.keybindings = KeyBindings()
        self.setup_keybindings()

        self.__threshold = 1
        self.__cumulative_keys = ""
        self.__last_after_id = None

    def create_component(self):
        return Component(self.__canvas)

    def __on_focus_out(self, event):
        self.__root.destroy()

    def setup_keybindings(self):
        self.keybindings.add('q', self.quit)

    def __execute_keybinding_function(self):
        function = self.keybindings.function(self.__cumulative_keys)
        self.__cumulative_keys = ""
        if function is not None:
            function()

    def __on_key_press(self, event):
        if self.__last_after_id is not None:
            self.__root.after_cancel(self.__last_after_id)

        # TODO: Allow modifier keys
        # IDEA: Convert cumulative_keys to a list of objects
        key = event.char
        if self.keybindings.any_starts_with(self.__cumulative_keys + key):
            self.__cumulative_keys += key
        else:
            self.__cumulative_keys = key

        # print(self.cumulative_keys)

        if self.keybindings.contains(self.__cumulative_keys):
            self.__execute_keybinding_function()

        self.__last_after_id = self.__root.after(
            self.__threshold * 1000,
            self.__execute_keybinding_function
        )

    def run(self):
        self.__root.mainloop()

    def quit(self):
        self.__root.destroy()
