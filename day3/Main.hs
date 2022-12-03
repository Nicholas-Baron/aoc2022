import Data.Char (isUpper, ord)
import Data.List (find)
import Data.Maybe (mapMaybe)

findBadge :: [String] -> Maybe Char
findBadge [] = Nothing
findBadge (bag : bags) = find (\x -> all (\b -> x `elem` b) bags) bag

bothCompartmentItem :: String -> Maybe Char
bothCompartmentItem str = findCommonItem lhs rhs
  where
    findCommonItem :: Eq a => [a] -> [a] -> Maybe a
    findCommonItem lhs = find (`elem` lhs)

    (lhs, rhs) = splitAt (length str `div` 2) str

priorities :: Char -> Int
priorities c = if isUpper c then (ord c - ord 'A') + 27 else (ord c - ord 'a') + 1

chunksOf :: Int -> [a] -> [[a]]
chunksOf _ [] = []
chunksOf n input = let (lhs, rhs) = splitAt n input in (lhs : chunksOf n rhs)

main = do
  input <- readFile "input.txt"

  let bags = lines input
      itemsToRearrange = mapMaybe bothCompartmentItem bags
      badges = mapMaybe findBadge $ chunksOf 3 bags

  print $ sum $ map priorities itemsToRearrange
  print $ sum $ map priorities badges
