use std::io; // prelude
// https://github.com/mstorsjo/llvm-mingw/releases

fn main() {
    println!("Hello, world!");

    let mut guess = String::new();

    io::stdin().read_line(&mut guess).expect("read error.");

    println!("Your number: {}", guess);
}
