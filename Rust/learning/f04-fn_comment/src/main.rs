/*
 * @Author: Alan Yin
 * @Date: 2023-07-23 16:36:59
 * @LastEditTime: 2023-07-23 17:29:33
 * @LastEditors: Alan Yin
 * @FilePath: /learning/f04-o/src/main.rs
 * @Description:
 *
 * Copyright (c) 2023 by Alan Yin, All Rights Reserved.
 */

/*
内容：
- 函数命名：
    Rust 代码中的函数和变量名使用 snake case 规范风格。在 snake case 中，所有字母都是
小写并使用下划线分隔单词

- 语句和表达式：
    语句（Statements）：是执行一些操作但不返回值的指令，let y = 6;
    表达式（Expressions）：计算并产生一个值，5 + 6，函数调用是一个表达式，表达式可以是
语句的一部分；用大括号创建的一个新的块作用域也是一个表达式；表达式的结尾没有分号。如果在
表达式的结尾加上分号，它就变成了语句，而语句不会返回值。
 */

//! # My Crate
//!
//! `my_crate` is a collection of utilities to make performing certain
//! calculations more convenient.

fn show_expression() {
    let y = {
        let x = 3;

        // 结尾没有分号，表达式的结尾没有分号。如果在表达式的结尾加上分号，它就变成了语
        // 句，而语句不会返回值。
        x + 1
    };

    println!("The value of y is: {y}");
}

fn have_return (a: i32, b: i32) -> i32 {
    a + b

    // 使用单位类型 () 表示不返回值。
    // return ();
}

/// Adds one to the number given.
///
/// # Examples
///
/// ```
/// let arg = 5;
/// let answer = my_crate::add_one(arg);
///
/// assert_eq!(6, answer);
/// ```
fn show_comment() {
    // 注释类型1

    /* 注释类型2 */

    // 文档注释
    /*
    文档注释（documentation comments），他们会生成 HTML 文档。这些 HTML 展示公有 API
    文档注释的内容。
    文档注释使用三斜杠 /// 而不是两斜杆以支持 Markdown 注解来格式化文本。文档注释就位于
    需要文档的项的之前
    */

    // 文档注释风格
    /*
    文档注释风格 //! 为包含注释的项，而不是位于注释之后的项增加文档。这通常用于 crate
    根文件（通常是 src/lib.rs）或模块的根文件为 crate 或模块整体提供文档。
    注意 //! 的最后一行之后没有任何代码。因为他们以 //! 开头而不是 ///，这是属于包含此
    注释的项而不是注释之后项的文档。在这个情况下时 src/lib.rs 文件，也就是 crate 根文
    件。这些注释描述了整个 crate。
    */
}

fn main() {
    show_expression();

    println!("The sum of a and b is: {}", have_return(111, 333));

    show_comment();
}
