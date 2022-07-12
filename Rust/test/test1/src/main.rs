use std::io;            // prelude
use std::cmp::Ordering; // 枚举类型
use rand::Rng;          // trait

fn main() {
    println!("Hello, this is a secret number game!");

    let secret_num = rand::thread_rng().gen_range(1, 101);

    loop {
        println!("please give your number");
        let mut guess = String::new();
        io::stdin().read_line(&mut guess).expect("read error.");
    
        // shadow old guess
        let guess: u32 = match guess.trim().parse() {
            Ok(num) => num,
            Err(_) => continue,
        };
    
        println!("Your number: {}", guess);
        match guess.cmp(&secret_num) {
            Ordering::Less => println!("Too small!"),
            Ordering::Greater => println!("Too big!"),
            Ordering::Equal => {
                println!("You get it!");
                break;
            }
        }
    }
}
