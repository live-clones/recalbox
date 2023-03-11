from enum import Enum, IntEnum

class Rotation(IntEnum):
    none = 0
    left = 1
    upsidedown = 2
    right = 3

    @staticmethod
    def fromInt(value: int):
        if value == 1:
            return Rotation.left
        elif value == 2:
            return Rotation.upsidedown
        elif value == 3:
            return Rotation.right
        return Rotation.none