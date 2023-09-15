/*
 * @Author: Alan Yin
 * @Date: 2023-07-22 12:23:51
 * @LastEditTime: 2023-07-22 16:45:21
 * @LastEditors: Alan Yin
 * @FilePath: /learning/f01-startup/01-compile.rs
 * @Description:
 *
 * Copyright (c) 2023 by HT706, All Rights Reserved.
 */

/*
内容：

- 编译：
    rustc filename.rs；
- 模块定义：
    pub 标记；
- 标准输出：
    println! 宏；
*/

mod foo;

fn main() {
    println!("Hello, nice meeting you!");
    foo::hello();
}