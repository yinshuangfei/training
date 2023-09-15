/*
 * @Author: Alan Yin
 * @Date: 2023-07-22 16:38:45
 * @LastEditTime: 2023-07-23 00:40:29
 * @LastEditors: Alan Yin
 * @FilePath: /learning/f02-guess_number/src/main.rs
 * @Description:
 *
 * Copyright (c) 2023 by HT706, All Rights Reserved.
 */

/*
内容：

- cargo创建项目：
    cargo new <project_name>;

- 导入标准库：
    use std::io;
    读取标准输入；
    默认情况下，Rust 设定了若干个会自动导入到每个程序作用域中的标准库内容，这组内容被称
为预导入（preclude） 内容。如果你需要的类型不在预导入内容中，就必须使用 use 语句显式地
将其引入作用域。

- 变量声明：
    let: 创建变量；在 Rust 中，变量默认是不可变的；
    mut: mutable，可变的；使用 mut 来使一个变量可变；

- 变量类型：
    String: 字符串；
    i32: 32位数字;
    u32: 32位无符号数字;
    i64: 64位数字;
    u64: 64位无符号数字;
    i8, i16, i128, u8, u16, u128;

- 字符串操作：
    trim: 去除字符串开头和结尾的空白字符；
    parse: 将字符串转换成其他类型;(通过 let guess: u32 指定)


- 标准化输出：
    println!格式化字符串；
    使用占位符{}。当打印变量的值时，变量名可以写进大括号中。当打印表达式的执行结果时，
格式化字符串（format string）中大括号中留空，格式化字符串后跟逗号分隔的需要打印的表达式
列表，其顺序与每一个空大括号占位符的顺序一致。

- 引用
    &，多处代码访问同一处数据，而无需在内存中多次拷贝；它像变量一样，默认是不可变的。因
此，需要写成 &mut guess 来使其可变，而不是 &guess;

- crate库：（大木箱）
    库 crate 可以包含任意能被其他程序使用的代码，但是不能自执行；Cargo 对外部 crate 的
运用需要修改 Cargo.toml 文件，引入一个依赖。定义版本号中，0.8.5 事实上是 ^0.8.5 的简
写，它表示任何至少是 0.8.5 但小于 0.9.0 的版本。Cargo 认为这些版本与 0.8.5 版本的公有
API 相兼容，这样的版本指定确保了我们可以获取能使本章代码编译的最新的补丁（patch）版本。
任何大于等于 0.9.0 的版本不能保证和接下来的示例采用了相同的 API。
    虽然只声明了 rand 一个依赖，然而 Cargo 还是额外获取了 rand 所需要的其他 crates，
因为 rand 依赖它们来正常工作。
    Cargo.lock, 当第一次构建项目时，Cargo 计算出所有符合要求的依赖版本并写入
Cargo.lock 文件。当将来构建项目时，Cargo 会发现 Cargo.lock 已存在并使用其中指定的版本，
而不是再次计算所有的版本。这使得你拥有了一个自动化的可重现的构建。换句话说，项目会持续使
用 0.8.5 直到你显式升级. 由于 Cargo.lock 文件对于“可重复构建”非常重要，因此它通常会和
项目中的其余代码一样纳入到版本控制系统中。
    当你 确实 需要升级 crate 时，Cargo 提供了这样一个命令，update，它会忽略 Cargo.lock
文件，并计算出所有符合 Cargo.toml 声明的最新版本。Cargo 接下来会把这些版本写入
Cargo.lock 文件。不过，Cargo 默认只会寻找大于 0.8.5 而小于 0.9.0 的版本。如果想要使用
0.9.0 版本的 rand 或是任何 0.9.x 系列的版本，需要手动修改Cargo.toml 文件对应的版本定义。

- trait：
    特征；

- 范围表达式：
    start..=end，使用 1..=100 来表示 1 到 100；

- 代码语句：
    match: 一个 match 表达式由 分支（arms） 构成。一个分支包含一个模式（pattern）
和代码，表达式开头的值与分支模式相匹配时执行该代码；
    loop: 循环语句；使用 break 跳出循环；continue 继续；
 */

// io 库是 prelude
use std::{io, cmp::Ordering};
// Rng 是一个 trait，包含了 gen_range 函数
use rand::Rng;

fn main() {
    // 不可变变量
    let max = 100;
    let secret_num = rand::thread_rng().gen_range(1..=100);

    println!("guess the secret number, it won't large then {max}!");

    let mut guess;

    loop {
        println!("please input your number ... ");

        // 可变变量
        // new() 为 String的静态方法，变量 guess 绑定到一个新的String实例上
        guess = String::new();

        io::stdin()
            // 执行函数
            // & 表示这个参数是一个 引用（reference）

            // read_line 会将用户输入附加到传递给它的字符串中，不过它也会返回一个类型为
            // Result 的值, Result 是一种枚举类型, 成员是 Ok 和 Err。如果 Result 实
            // 例的值是 Err，expect 会导致程序崩溃，并显示当做参数传递给 expect 的信息；
            // 如果 Result 实例的值是 Ok，expect 会获取 Ok 中的值并原样返回。
            .read_line(&mut guess)

            // 不调用 expect，程序也能编译，不过会出现警告。
            .expect("Can't read line");

        let guess:u32 = match guess.trim().parse() {
            Ok(num) => num,
            // _ 是一个通配符值，本例中用来匹配所有 Err 值
            Err(_) => continue,
        };

        println!("You guess: {guess}");

        match guess.cmp(&secret_num) {
            Ordering::Less => println!("Too small!"),
            Ordering::Greater => println!("Too big!"),
            Ordering::Equal => {
                println!(
                    "Yes {guess} is the secret number, you are the {}-{}-{}!!!",
                    "THE",
                    "GREAT",
                    "ONE"
                );
                break;
            }
        }
    }
}
