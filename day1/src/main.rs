use std::error::Error;
use std::fs;

fn main() -> Result<(), Box<dyn Error>> {
    let filename = "input.txt";

    let mut elves: Vec<Vec<u32>> = vec![vec![]];

    let raw_data = fs::read_to_string(filename)?;

    for line in raw_data.lines() {
        if line == "" {
            // Separation line
            elves.push(vec![]);
        } else {
            elves.last_mut().unwrap().push(line.parse()?);
        }
    }

    let mut calorie_counts: Vec<u32> = elves.into_iter().map(|elf| elf.into_iter().sum()).collect();

    calorie_counts.sort_unstable_by(|lhs, rhs| rhs.cmp(lhs));

    println!("{}", calorie_counts.iter().take(3).sum::<u32>());

    Ok(())
}
