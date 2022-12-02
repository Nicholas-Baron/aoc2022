use std::error::Error;
use std::fs;

#[derive(PartialEq)]
enum RPS {
    Rock,
    Paper,
    Scissors,
}

fn to_rps(input: &str) -> RPS {
    match input {
        "A" => RPS::Rock,
        "B" => RPS::Paper,
        "C" => RPS::Scissors,
        "X" => RPS::Rock,
        "Y" => RPS::Paper,
        "Z" => RPS::Scissors,
        _ => panic!(),
    }
}

enum WLD {
    Win,
    Lose,
    Draw,
}

fn to_wld(input: &str) -> WLD {
    match input {
        "X" => WLD::Lose,
        "Y" => WLD::Draw,
        "Z" => WLD::Win,
        _ => panic!(),
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let filename = "input.txt";

    let round_scores: Vec<_> = fs::read_to_string(filename)?
        .lines()
        .map(|line| {
            let (opponent, outcome) = line.split_once(' ').unwrap();

            let selection_score = match (to_rps(opponent), to_wld(outcome)) {
                // Select Rock
                (RPS::Rock, WLD::Draw) | (RPS::Paper, WLD::Lose) | (RPS::Scissors, WLD::Win) => 1,

                // Select Paper
                (RPS::Rock, WLD::Win) | (RPS::Paper, WLD::Draw) | (RPS::Scissors, WLD::Lose) => 2,

                // Select Scissors
                (RPS::Rock, WLD::Lose) | (RPS::Paper, WLD::Win) | (RPS::Scissors, WLD::Draw) => 3,
            };

            let outcome_score = match to_wld(outcome) {
                WLD::Lose => 0,
                WLD::Draw => 3,
                WLD::Win => 6,
            };

            selection_score + outcome_score
        })
        .collect();

    println!("{}", round_scores.into_iter().sum::<u32>());

    Ok(())
}
