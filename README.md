# fwif

> This is a failed attempt to creating my own implementation of something like [rofi](https://github.com/davatorium/rofi). The main goal was to create a simple framework that enables me to easily build floating GUIs for any tool or utility I might need in my dev workflow. The reason it's a "failed attempt" is because of its shitty performance. If you think you can help I'd be forever grateful.

fwif (floating window framework) is a Python framework that enables easy development of floating window applications using X11.
It provides a high-level abstraction for building applications entirely in Python, while leveraging inter-process communication (IPC) to interact with the underlying window layer implemented in C.

## Features

- **Designed for Tiling Window Manager Users:** fwif sets the necessary window manager (WM) attributes for your applications to make them appear as floating windows in tiling window managers like i3, qtile, etc.

- **Keyboard-driven:** Your fwif applications are keyboard-driven, you can easily associate keybindings with the different functions of your application.

- **Simple UI:** fwif offers a straightforward user interface design approach. Components in fwif are just basic rectangles, making it effortless to combine them to achieve the desired user interface.

- **Flexible Component System:** Each component in fwif is represented by a JSON object, allowing you to define the structure and properties of your UI elements in a hierarchical manner.

- **Efficient Key Press Handling:** fwif employs an efficient key press handling mechanism inspired by Vim. Define key bindings for specific actions or functions, and fwif will intelligently interpret user input.

## Getting Started

To get started with fwif, follow these steps:

1. Install the framework by running the following command:
   ```
   pip install fwif
   ```

2. Import the `BaseApp` class from the framework in your Python code:
   ```python
   from fwif import BaseApp
   ```

3. Create a subclass of `BaseApp` to define your custom application. 

4. Override the `setup_keybindings` method to add your custom keybindings:
   ```python
   class MyApp(BaseApp):
    def __init__(self):
        super().__init__()

    def setup_keybindings(self):
        super().setup_keybindings()
        self.keybindings.add('h', self.hello)

    def hello(self):
        print("Hello, World!")
   ```

5. Run your application by instantiating your subclass and calling the `run()` method:
   ```python
   app = MyApp()
   app.run()
   ```

For more details, please refer to the [example](https://github.com/youssef-attai/fwif/blob/main/examples/dynamic_list_app.py) provided.

## Contributing

Contributions are welcome! If you find any issues, have suggestions, or want to contribute new features or improvements, please feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](https://github.com/youssef-attai/fwif/blob/main/LICENSE) file for more information.

