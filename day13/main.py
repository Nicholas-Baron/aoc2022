from ast import literal_eval
from collections.abc import Sequence
from functools import cmp_to_key
from sys import argv, exit
from typing import Optional


def compare_lists(
    lhs: Sequence[int | list[int]], rhs: Sequence[int | list[int]]
) -> Optional[bool]:
    for l, r in zip(lhs, rhs):
        if isinstance(l, int) and isinstance(r, int):
            if l != r:
                return l < r
        elif isinstance(l, list) and isinstance(r, list):
            decision = compare_lists(l, r)
            if decision is not None:
                return decision
            if len(l) < len(r):
                return True
            if len(r) < len(l):
                return False
        else:
            decision = compare_lists(
                l if isinstance(l, list) else [l], r if isinstance(r, list) else [r]
            )
            if decision is not None:
                return decision

    if len(lhs) != len(rhs):
        return len(lhs) < len(rhs)
    return None


if len(argv) != 2:
    print(f"Usage: {argv[0]} <input filename>")
    exit()

index = 1
result = 0
filename = argv[1]
packets: list[list] = []
with open(filename) as f:
    while f.readable():
        lhs_raw = f.readline()
        rhs_raw = f.readline()
        if lhs_raw == "" or rhs_raw == "":
            break

        lhs = literal_eval(lhs_raw)
        rhs = literal_eval(rhs_raw)
        f.readline()

        packets.append(lhs)
        packets.append(rhs)

        print(f"comparing {lhs} and {rhs}")

        if compare_lists(lhs, rhs):
            print(f"Adding index {index}")
            result += index

        index += 1

print(result)

packets += [[[2]], [[6]]]


def cmp_lists(lhs, rhs):
    result = compare_lists(lhs, rhs)
    if result is None:
        return 0
    elif result == True:
        return -1
    else:
        return 1


packets.sort(key=cmp_to_key(cmp_lists))

print(packets)

first_divider = packets.index([[2]]) + 1
second_divider = packets.index([[6]]) + 1

print(first_divider * second_divider)
