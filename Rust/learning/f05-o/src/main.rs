/*
 * @Author: Alan Yin
 * @Date: 2023-07-23 17:31:38
 * @LastEditTime: 2023-07-23 17:48:34
 * @LastEditors: Alan Yin
 * @FilePath: /learning/f05-o/src/main.rs
 * @Description:
 *
 * Copyright (c) 2023 by HT706, All Rights Reserved.
 */

/*
内容：

- 控制流：
    if：表达式；
 */

fn show_if() {
    let number = 3;

    // 代码中的条件必须是 bool 值
    // 使用过多的 else if 表达式会使代码显得杂乱无章，所以如果有多于一个 else if 表达
    // 式，最好重构代码。
    if number % 4 == 0 {
        println!("number is divisible by 4");
    } else if number % 3 == 0 {
        println!("number is divisible by 3");
    } else if number % 2 == 0 {
        println!("number is divisible by 2");
    } else {
        println!("number is not divisible by 4, 3, or 2");
    }
}

fn main() {
    show_if();
}
