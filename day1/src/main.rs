use std::error::Error;
use std::fs;
use std::str::FromStr;

fn main() -> Result<(), Box<dyn Error>> {
    let filename = "input.txt";

    let mut calorie_counts = fs::read_to_string(filename)?.lines().try_fold(
        vec![0],
        |mut acc, line| -> Result<_, <u32 as FromStr>::Err> {
            if line.is_empty() {
                acc.push(0);
            } else {
                *acc.last_mut().unwrap() += u32::from_str(line)?;
            }
            Ok(acc)
        },
    )?;

    calorie_counts.sort_unstable_by(|lhs, rhs| rhs.cmp(lhs));

    println!("{}", calorie_counts.iter().take(3).sum::<u32>());

    Ok(())
}
