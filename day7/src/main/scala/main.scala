import scala.collection.mutable
import scala.io.Source

def fileToLines(filename: String): List[String] =
  val file = Source.fromFile(filename)
  val lines = file.getLines.toList
  file.close
  lines

def changeDirectory(currentDirectory: String, newDir: String): String =
  newDir match
    case ".." =>
      val lastSlash = currentDirectory.lastIndexOf("/")
      currentDirectory.substring(0, lastSlash)
    case "/" => newDir
    case _ => if (currentDirectory.endsWith("/")) currentDirectory + newDir else currentDirectory + "/" + newDir
@main def hello() =
  val filename = "input.txt"
  val lines = fileToLines(filename)

  var currentDirectory: String = ""
  val sizes: mutable.HashMap[String, Int] = mutable.HashMap.empty
  val seenFiles: mutable.HashSet[String] = mutable.HashSet.empty
  sizes.addOne("/", 0)

  for (line <- lines) {
    if (line.startsWith("$ ")) {
      val command = line.stripPrefix("$ ")
      if (command.startsWith("cd")) {
        currentDirectory = changeDirectory(currentDirectory, command.stripPrefix("cd "))
      }
    } else {
      val dirEntry = line.split(" ", 2)
      println(dirEntry.mkString(" ") + " in " + currentDirectory)
      if (dirEntry(0) == "dir") {
        sizes.addOne(changeDirectory(currentDirectory, dirEntry(1)), 0)
      } else if (!seenFiles.contains(currentDirectory + "/" + dirEntry(1))) {
        val size = dirEntry(0).toInt
        var path = "/"
        if (currentDirectory == path) {
          sizes(path) += size
          println("Incremented " + path + " by " + size)
        } else
          for (item <- currentDirectory.split('/')) {
            path = changeDirectory(path, item)
            sizes(path) += size
            println("Incremented " + path + " by " + size)
          }
        seenFiles.addOne(currentDirectory + "/" + dirEntry(1))
      } else {
        println("Saw " + dirEntry(1) + " again")
      }
    }
  }

  println(sizes.filter((_, size) => size <= 100000).map(_(1)).sum)

  val totalDiskSpace = 70_000_000
  val neededSpace = 30_000_000
  val usedSpace = sizes("/")
  println("Used " + usedSpace)
  val freeSpace = totalDiskSpace - usedSpace
  println("Unused " + freeSpace)
  val needToFree = neededSpace - freeSpace
  println("Need to free " + needToFree)
  println(sizes.map(_(1)).filter(_ >= needToFree).min)
