import random
from fwif import FloatingWindow


def random_color():
    r = random.randint(0, 255)
    g = random.randint(0, 255)
    b = random.randint(0, 255)

    # Convert RGB to hexadecimal representation
    hex_code = '#{:02x}{:02x}{:02x}'.format(r, g, b)
    return hex_code


class DynamicListApp(FloatingWindow):
    def __init__(self):
        super().__init__()
        self.items = []
        self.selected_index = -1

    def setup_keybindings(self):
        super().setup_keybindings()
        self.keybindings.add('a', self.add_item)
        self.keybindings.add('dd', self.delete_item)
        self.keybindings.add('x', self.delete_item)
        self.keybindings.add('dG', self.delete_from_selection_to_last)
        self.keybindings.add('dgg', self.delete_from_selection_to_first)
        self.keybindings.add('D', self.delete_all)
        self.keybindings.add('gg', self.select_first)
        self.keybindings.add('G', self.select_last)
        self.keybindings.add('k', self.move_selection_up)
        self.keybindings.add('j', self.move_selection_down)
        self.keybindings.add('K', self.move_item_up)
        self.keybindings.add('J', self.move_item_down)
        self.keybindings.add('c', self.change_color)

    def change_color(self):
        if self.selected_index >= 0 and len(self.items) > 0:
            item = self.items[self.selected_index]
            item.update_background(random_color())
            item.redraw()

    def delete_from_selection_to_first(self):
        if self.selected_index >= 0 and len(self.items) > 0:
            for i in range(self.selected_index + 1):
                item = self.items.pop(0)
                item.delete()
            self.selected_index = 0
            self.update_items()

    def delete_from_selection_to_last(self):
        if self.selected_index >= 0 and len(self.items) > 0:
            for i in range(self.selected_index, len(self.items)):
                item = self.items.pop()
                item.delete()
            self.selected_index = min(self.selected_index, len(self.items) - 1)
            self.update_items()

    def select_first(self):
        if len(self.items) > 0:
            self.selected_index = 0
            self.update_items()

    def select_last(self):
        if len(self.items) > 0:
            self.selected_index = len(self.items) - 1
            self.update_items()

    def add_item(self):
        item = self.create_component().config(
            x=10,
            y=30 * (len(self.items) + 1),
            width=200,
            height=20,
            background=random_color(),
            border_color="black",
            text=""
        )
        self.items.append(item)
        self.selected_index = len(self.items) - 1
        self.update_items()

    def delete_all(self):
        for item in self.items:
            item.delete()
        self.items = []
        self.selected_index = -1

    def delete_item(self):
        if self.selected_index >= 0 and len(self.items) > 0:
            item = self.items.pop(self.selected_index)
            item.delete()
            self.selected_index = min(self.selected_index, len(self.items) - 1)
            self.update_items()

    def move_selection_down(self):
        if self.selected_index < len(self.items) - 1:
            self.selected_index += 1
            self.update_items()

    def move_selection_up(self):
        if self.selected_index > 0:
            self.selected_index -= 1
            self.update_items()

    def move_item_down(self):
        if self.selected_index < len(self.items) - 1:
            self.items[
                self.selected_index
            ], self.items[
                self.selected_index + 1
            ] = self.items[
                self.selected_index + 1
            ], self.items[
                self.selected_index
            ]

            self.selected_index += 1
            self.update_items()

    def move_item_up(self):
        if self.selected_index > 0:
            self.items[
                self.selected_index
            ], self.items[
                self.selected_index - 1
            ] = self.items[
                self.selected_index - 1
            ], self.items[
                self.selected_index
            ]

            self.selected_index -= 1
            self.update_items()

    def update_items(self):
        for i, item in enumerate(self.items):
            item.update_position(10, 30 * (i + 1))
            if i == self.selected_index:
                item.update_border("black", 5)
            else:
                item.update_border("black", 0)
            item.redraw()


def main():
    app = DynamicListApp()
    app.run()


if __name__ == '__main__':
    main()
