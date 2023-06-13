# fwif

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/youssef-attai/fwif/blob/main/LICENSE)

fwif (Floating Window Framework) is a powerful Python framework built on top of [Tkinter](https://docs.python.org/3/library/tkinter.html). It allows users to create highly configurable, keyboard-driven graphical user interfaces (GUIs) that are ideal for scratchpad-style applications. With this framework, you can easily build custom alternatives to the scratchpads used with [i3](https://i3wm.org/) or create your own keyboard-centric applications.

## Features

- Keyboard-driven: Your fwif applications are keyboard-driven, you can easily associate keybindings with the different functions of your application.
- Floating window support: The framework sets the necessary window manager (WM) attributes for your applications to behave like floating windows by default. This is especially useful when integrating your applications with tiling window managers like i3.

## Getting Started

To get started with fwif, follow these steps:

1. Install the framework by running the following command:
   ```
   pip install fwif
   ```

2. Import the BaseApp class from the framework in your Python code:
   ```python
   from fwif import FloatingWindow
   ```

3. Create a subclass of BaseApp to define your custom application. 

4. Override the `setup_keybindings` method to add your custom keybindings:
   ```python
   class MyApp(FloatingWindow):
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

For more details and examples, please refer to the [example](https://github.com/youssef-attai/fwif/blob/main/example/) provided.

## Contributing

Contributions are welcome! If you find any issues, have suggestions, or want to contribute new features or improvements, please feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](https://github.com/youssef-attai/fwif/blob/main/LICENSE) file for more information.

