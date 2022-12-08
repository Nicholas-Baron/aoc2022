import scala.io.Source

def fileToLines(filename: String): List[String] =
  val file = Source.fromFile(filename)
  val lines = file.getLines.toList
  file.close
  lines


def isVisible(forest: Array[Array[Int]],
              x: Int, y: Int): Boolean =
  if (x == 0 || y == 0 || x == forest.length - 1 || y == forest.head.length - 1) true
  else {
    val height = forest(x)(y)

    val canSeeLeft = !Range(1, x + 1).exists(delta => height <= forest(x - delta)(y))
    val canSeeRight = !Range(1, forest.length - x).exists(delta => height <= forest(x + delta)(y))
    val canSeeTop = !Range(1, y + 1).exists(delta => height <= forest(x)(y - delta))
    val canSeeBottom = !Range(1, forest.head.length - y).exists(delta => height <= forest(x)(y + delta))

    canSeeTop || canSeeBottom || canSeeLeft || canSeeRight
  }

def scenicScore(forest: Array[Array[Int]], x: Int, y: Int): Int = {
  val height = forest(x)(y)

  def treeCount(range: Range, endpoint: Int): Int =
    if (range.contains(endpoint)) range.length else range.length + 1

  val seeLeft = treeCount(
    range = (1 to x).takeWhile(delta => height > forest(x - delta)(y)),
    endpoint = x
  )

  val seeRight = treeCount(
    range = Range(1, forest.length - x).takeWhile(delta => height > forest(x + delta)(y)),
    endpoint = (forest.length - x) - 1
  )

  val seeTop = treeCount(
    range = (1 to y).takeWhile(delta => height > forest(x)(y - delta)),
    endpoint = y
  )

  val seeBottom = treeCount(
    range = Range(1, forest.head.length - y).takeWhile(delta => height > forest(x)(y + delta)),
    endpoint = (forest.head.length - y) - 1
  )

  seeLeft * seeTop * seeBottom * seeRight
}

@main def main(): Unit =
  val fileName = "input.txt"
  val input = fileToLines(fileName).map(line => line.chars().map(_ - '0').toArray).toArray

  println(input.map(_.mkString(" ")).mkString("\n"))
  println(input.length + " by " + input.head.length)
  var visibleCount = 0
  for (x <- input.indices)
    for (y <- input.head.indices)
      if (isVisible(input, x, y)) visibleCount += 1

  println(visibleCount)
  println(input.zipWithIndex.flatMap(
    (row: Array[Int], x) => row.zipWithIndex.map((_, y) => scenicScore(input, x, y))
  ).max)

