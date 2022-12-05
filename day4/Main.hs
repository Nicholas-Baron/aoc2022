import Data.List (elemIndex)
import Debug.Trace (traceShow)

count :: (a -> Bool) -> [a] -> Integer
count _ [] = 0
count pred (x : xs) = (if pred x then 1 else 0) + count pred xs

-- Does the lhs fully contain the rhs?
fullyContains :: Ord a => [a] -> [a] -> Bool
fullyContains lhs rhs = (head lhs <= head rhs) && (last lhs >= last lhs)

overlapAtAll :: Ord a => [a] -> [a] -> Bool
overlapAtAll lhs = any (`elem` lhs)

main = do
  input <- readFile "input.txt"

  let rawLines = lines input
      ranges = map parseRanges rawLines

  print $ count (uncurry fullyContains) ranges
  print $ count (uncurry overlapAtAll) ranges

parseRanges :: String -> ([Int], [Int])
parseRanges input = (parseList lhs, parseList rhs)
  where
    (lhs, rhs) = splitOn ',' input
    parseList input = let (start, end) = splitOn '-' input in [read start .. read end]

    splitOn :: Eq a => a -> [a] -> ([a], [a])
    splitOn delim input =
      let Just index = elemIndex delim input
          (lhs, rhs) = splitAt index input
       in (lhs, tail rhs)
