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

    def contains(self, key):
        return key in self.__bindings

    def any_starts_with(self, key):
        for k in self.__bindings.keys():
            if k.startswith(key):
                return True
        return False
