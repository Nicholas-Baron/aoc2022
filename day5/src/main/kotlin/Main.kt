import java.io.File

fun part1Rearragement(stacks: List<MutableList<Char>>, moveOrders: List<String>) {
    for (line in moveOrders) {
        val movement = line.split(' ').mapNotNull { it.toIntOrNull() }.toList()

        val source = stacks[movement[1] - 1]
        val destination = stacks[movement[2] - 1]
        val amount = movement[0]

        for (count in 1..amount)
            destination.add(source.removeLast())
    }
}

fun part2Rearragement(stacks: MutableList<MutableList<Char>>, moveOrders: List<String>) {
    for (line in moveOrders) {
        val movement = line.split(' ').mapNotNull { it.toIntOrNull() }.toList()

        val source = stacks[movement[1] - 1]
        val destination = stacks[movement[2] - 1]
        val amount = movement[0]

        destination.addAll(source.takeLast(amount))
        stacks[movement[1] - 1] = source.dropLast(amount).toMutableList()
    }
}

fun main() {

    val fileName = "input.txt"

    val file = File(fileName)
    val contents = file.readLines()

    val startState = contents.takeWhile { line -> !line.contains("move") && line.isNotEmpty() }

    val numberLocations = mutableMapOf<Int, Int>()

    for ((i, ch) in startState.last().withIndex()) {
        if (ch == ' ') continue
        numberLocations[i] = ch.digitToInt() - 1
    }

    val numStacks = numberLocations.values.max() + 1
    println("Found $numStacks stacks")

    val stacks = MutableList(numStacks) { mutableListOf<Char>() }

    for (line in startState.reversed().drop(1)) {
        for ((index, slot) in numberLocations) {
            val crate = line[index]
            if (crate.isLetter())
                stacks[slot].add(crate)
        }
    }

    part2Rearragement(stacks, contents.filter { it.contains("move") })

    println(stacks.map {
        it.last { it.isLetter() }
    }.fold(StringBuilder()) { acc, ch -> acc.append(ch); acc }.toString())
}