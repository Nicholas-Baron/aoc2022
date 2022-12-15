from enum import Enum, auto
from sys import argv, exit


def parse_tuple(raw: str) -> tuple[int, int]:
    print(f"'{raw}'")
    lhs, rhs = raw.split(",", maxsplit=1)
    return int(lhs), int(rhs)


class Tile(Enum):
    AIR = auto()
    ROCK = auto()
    SAND = auto()


slide_dict = dict[tuple[int, int], Tile]


def make_slide(all_points: list[list[tuple[int, int]]]) -> tuple[slide_dict, int]:
    slide: dict[tuple[int, int], Tile] = {}
    lowest_y = 0
    for points in all_points:
        last_point = None
        for point in points:
            if last_point is None:
                last_point = point
                continue

            is_vertical = last_point[0] == point[0]
            constant = last_point[0] if is_vertical else last_point[1]
            bounds = (
                (last_point[1], point[1]) if is_vertical else (last_point[0], point[0])
            )
            small_end = min(*bounds)
            large_end = max(*bounds)
            print(f"last_point={last_point}, point={point}, is_vertical={is_vertical}")
            last_point = point
            lowest_y = max(lowest_y, large_end if is_vertical else constant)

            for i in range(small_end, large_end + 1):
                location = (constant, i) if is_vertical else (i, constant)
                slide[location] = Tile.ROCK

    return slide, lowest_y


# true if the sand was added
def add_sand(slide: slide_dict, lowest_y: int) -> bool:
    position = [500, 0]

    can_move_to = lambda pos: slide.get(pos, Tile.AIR) is Tile.AIR

    collided = False
    while not collided:
        position[1] += 1
        if position[1] > lowest_y:
            return False
        if can_move_to(tuple(position)):
            continue

        position[0] -= 1
        if can_move_to(tuple(position)):
            continue

        position[0] += 2
        if can_move_to(tuple(position)):
            continue

        # failed all moves
        collided = True
        position[0] -= 1
        position[1] -= 1

    slide[(position[0], position[1])] = Tile.SAND
    return True


# Read file
if len(argv) != 2:
    print(f"Usage: {argv[0]} <input filename>")
    exit()

all_points = []
with open(argv[1]) as f:
    for line in f.readlines():
        all_points.append(
            [parse_tuple(tup) for tup in line.split(" -> ") if tup.strip() != ""]
        )

slide, lowest_y = make_slide(all_points)

count = 0
fell_to_abyss = False
while not fell_to_abyss:
    fell_to_abyss = not add_sand(slide, lowest_y)
    count += 1

print(count - 1)
