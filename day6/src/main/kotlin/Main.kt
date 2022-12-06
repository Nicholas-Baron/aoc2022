import java.io.File

// Marker: a `lookback` number of characters that are unique
fun findMarker(datastream: String, lookback: Int): Int? {
    val seenCharacters = ArrayDeque<Char>(lookback)

    for ((idx, c) in datastream.withIndex()) {
        if (seenCharacters.size < lookback) {
            seenCharacters.add(c)
            continue
        }

        if (HashSet(seenCharacters).size == seenCharacters.size) {
            // Found start of message
            return idx
        }

        seenCharacters.removeFirst()
        seenCharacters.add(c)
    }

    return null
}

fun main() {
    val filename = "input.txt"
    val datastream = File(filename).readText()

    println(findMarker(datastream, lookback = 4))
    println(findMarker(datastream, lookback = 14))
}