class Component:
    def __init__(
        self,
        canvas,
        x=0,
        y=0,
        width=1,
        height=1,
        background="white",
        foreground="black",
        padding=1,
        border_width=1,
        border_color="black",
        text=""
    ):
        self.__canvas = canvas
        self.__x = x
        self.__y = y
        self.__width = width
        self.__height = height
        self.__background = background
        self.__foreground = foreground
        self.__padding = padding
        self.__border_width = border_width
        self.__border_color = border_color
        self.__text = text
        self.__background_id = self.__canvas.create_rectangle(
            self.__x,
            self.__y,
            self.__x + self.__width + self.__padding,
            self.__y + self.__height + self.__padding,
            fill=self.__background,
            outline=self.__border_color,
            width=self.__border_width
        )
        self.__text_id = self.__canvas.create_text(
            self.__x + self.__padding,
            self.__y + self.__padding,
            anchor="nw",
            text=self.__text,
            fill=self.__foreground
        )

    def config(self, **kwargs) -> 'Component':
        self.__x = kwargs.get("x", self.__x)
        self.__y = kwargs.get("y", self.__y)
        self.__width = kwargs.get("width", self.__width)
        self.__height = kwargs.get("height", self.__height)
        self.__background = kwargs.get("background", self.__background)
        self.__foreground = kwargs.get("foreground", self.__foreground)
        self.__padding = kwargs.get("padding", self.__padding)
        self.__border_width = kwargs.get("border_width", self.__border_width)
        self.__border_color = kwargs.get("border_color", self.__border_color)
        self.__text = kwargs.get("text", self.__text)
        self.redraw()

        return self

    def delete(self):
        self.__canvas.delete(self.__background_id)
        self.__canvas.delete(self.__text_id)

    def update_text(self, text):
        self.__text = text
        self.redraw()

    def update_position(self, x, y):
        self.__x = x
        self.__y = y
        self.redraw()

    def update_foreground(self, color):
        self.__foreground = color
        self.redraw()

    def update_background(self, color):
        self.__background = color
        self.redraw()

    def update_border(self, color, width):
        self.__border_color = color
        self.__border_width = width
        self.redraw()

    def redraw(self):
        self.__canvas.coords(
            self.__background_id,
            self.__x,
            self.__y,
            self.__x + self.__width + self.__padding,
            self.__y + self.__height + self.__padding
        )
        self.__canvas.itemconfig(
            self.__background_id,
            fill=self.__background,
            outline=self.__border_color,
            width=self.__border_width
        )

        self.__canvas.coords(
            self.__text_id,
            self.__x + self.__padding,
            self.__y + self.__padding
        )
        self.__canvas.itemconfig(
            self.__text_id,
            text=self.__text,
            fill=self.__foreground
        )
