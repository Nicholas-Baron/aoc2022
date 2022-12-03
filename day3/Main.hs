import Data.Char (isUpper, ord)
import Data.List (find)
import Data.Maybe (fromJust)

findBadge :: [String] -> Char
findBadge [] = error "empty group"
findBadge (bag:bags) = fromJust $ find (\x -> all (\b -> x `elem` b) bags) bag

bothCompartmentItem :: String -> Char
bothCompartmentItem str = findCommonItem lhs rhs
  where
    findCommonItem :: String -> String -> Char
    findCommonItem set [] = error "no common item"
    findCommonItem set (x:xs) = if x `elem` set then x else findCommonItem set xs

    lhs = take (length str `div` 2) str
    rhs = drop (length str `div` 2) str

priorities :: Char -> Int
priorities c = if isUpper c then (ord c - ord 'A') + 27 else (ord c - ord 'a') + 1

chunksOf3 :: [a] -> [[a]]
chunksOf3 [] = []
chunksOf3 input = let (lhs, rhs) = splitAt 3 input in (lhs : chunksOf3 rhs)

main = do
    input <- readFile "input.txt"
    let 
      bags = lines input
      itemsToRearrange = map bothCompartmentItem bags
    print $ sum $ map priorities itemsToRearrange

    let badges = map findBadge $ chunksOf3 bags
    print $ chunksOf3 bags
    print $ sum $ map priorities badges
