/*
 * @Author: Alan Yin
 * @Date: 2023-07-22 23:04:50
 * @LastEditTime: 2023-07-23 00:45:05
 * @LastEditors: Alan Yin
 * @FilePath: /learning/f03-variable/src/main.rs
 * @Description:
 *
 * Copyright (c) 2023 by Alan Yin, All Rights Reserved.
 */

/*
内容：

- 关键字：
    不能作为变量或者函数名；
    as - 强制类型转换，消除特定包含项的 trait 的歧义，或者对 use 语句中的项重命名
    async - 返回一个 Future 而不是阻塞当前线程
    await - 暂停执行直到 Future 的结果就绪
    break - 立刻退出循环
    const - 定义常量或不变裸指针（constant raw pointer）
    continue - 继续进入下一次循环迭代
    crate - 在模块路径中，代指 crate root
    dyn - 动态分发 trait 对象
    else - 作为 if 和 if let 控制流结构的 fallback
    enum - 定义一个枚举
    extern - 链接一个外部函数或变量
    false - 布尔字面值 false
    fn - 定义一个函数或 函数指针类型 (function pointer type)
    for - 遍历一个迭代器或实现一个 trait 或者指定一个更高级的生命周期
    if - 基于条件表达式的结果分支
    impl - 实现自有或 trait 功能
    in - for 循环语法的一部分
    let - 绑定一个变量
    loop - 无条件循环
    match - 模式匹配
    mod - 定义一个模块
    move - 使闭包获取其所捕获项的所有权
    mut - 表示引用、裸指针或模式绑定的可变性
    pub - 表示结构体字段、impl 块或模块的公有可见性
    ref - 通过引用绑定
    return - 从函数中返回
    Self - 定义或实现 trait 的类型的类型别名
    self - 表示方法本身或当前模块
    static - 表示全局变量或在整个程序执行期间保持其生命周期
    struct - 定义一个结构体
    super - 表示当前模块的父模块
    trait - 定义一个 trait
    true - 布尔字面值 true
    type - 定义一个类型别名或关联类型
    union - 定义一个 union 并且是 union 声明中唯一用到的关键字
    unsafe - 表示不安全的代码、函数、trait 或实现
    use - 引入外部空间的符号
    where - 表示一个约束类型的从句
    while - 基于一个表达式的结果判断是否进行循环

- 变量：
    变量默认是不可改变的（immutable）；

- 强制转型：
    as: 使用 as 可以使数值转型；

- 常量：
    常量关键字为const；常量不能使用 mut 修饰；
    常量只能被设置为常量表达式，而不可以是其他任何只能在运行时计算出的值；
    Rust 对常量的命名约定是在单词之间使用全大写加下划线；
    将遍布于应用程序中的硬编码值声明为常量，能帮助后来的代码维护人员了解值的意图。如果将
来需要修改硬编码值，也只需修改汇聚于一处的硬编码值。

- 隐藏（Shadowing）：
    第二个变量“遮蔽”了第一个变量，此时任何使用该变量名的行为中都会视为是在使用第二个变
量，直到第二个变量自己也被隐藏或第二个变量的作用域结束。
    mut 与隐藏的另一个区别是，当再次使用 let 时，实际上创建了一个新变量，我们可以改变值
的类型，并且复用这个名字。

- 数据类型：
    标量（scalar）：代表一个单独的值；
        * 整型：i8, u8, ...(16, 32, 64), i128, u128, isize, usize, 有符号数以补码形
式存储; isize 和 usize 表示和体系结构的位宽有关；默认 i32;
        * 浮点型：f32, f64; 默认 f64;
        * 布尔类型
        * 字符类型: Rust 的 char 类型的大小为四个字节 (four bytes)，并代表了一个
Unicode 标量值（Unicode Scalar Value），这意味着它可以比 ASCII 表示更多内容;
    复合（compound）：
        * 元组：元组长度固定：一旦声明，其长度不会增大或缩小。
        * 数组：数组中的每个元素的类型必须相同；数组长度是固定的。
    Rust 是 静态类型（statically typed）语言，也就是说在编译时就必须知道所有变量的类
型。当多种类型均有可能时，必须增加类型注解，以供编译器识别。

- 溢出（256 赋值为 u8）：
    当在 debug 模式编译时，Rust 检查这类问题并使程序 panic，这个术语被 Rust 用来表明
程序因错误而退出。
    在 release 模式中构建时，Rust 不会检测会导致 panic 的整型溢出。相反发生整型溢出时，
Rust 会进行一种被称为二进制补码的操作。256 变为 0， 257 变为 1。
    为了显式地处理溢出的可能性，可以使用这几类标准库提供的原始数字类型方法：
        * 所有模式下都可以使用 wrapping_* 方法进行 wrapping，如 wrapping_add；
        * 如果 checked_* 方法出现溢出，则返回 None值；
        * 用 overflowing_* 方法返回值和一个布尔值，表示是否出现溢出；
        * 用 saturating_* 方法在值的最小值或最大值处进行饱和处理；

- 数值运算：
    加法、减法、乘法、除法和取余；

 */

fn const_mut() {
    // 常量
    const PI: f64 = 3.14;

    // 不可变变量
    let r = 5;

    // 可变变量
    let mut area: f64 = r as f64 * r as f64 * PI;
    println!("the radius is {}, area is {}", r, area);

    area = 3.0 * 3.0 * PI;
    println!("the radius is {}, area is {}", 3, area);
}

fn shadowing() {
    let x = 5;
    let x = x + 1;

    {
        let x = x * 2;
        println!("The value of x in the inner scope is: {x}");
    }

    println!("The value of x is: {x}");
}

fn show_number() {
    // 指定类型
    println!("{} is {}", 55, 55u8);

    // 十进制, 允许使用‘_’作为分隔符方便读数
    println!("{} is {}", 98222, 98_222);

    // 十六进制
    println!("{} is {}", 0xff, 255);

    // 八进制
    println!("{} is {}", 0o77, 63);

    // 二进制
    println!("{} is {}", 0b1111_0000, 240);

    // 单字节(仅限于u8)
    println!("{} is {}", b'A', 65);
}

fn show_bool() {
    let t = true;
    let f: bool = false; // with explicit type annotation
    println!("we are {} and {}", t, f)
}

fn show_char() {
    // 在 Rust 中，带变音符号的字母（Accented letters），中文、日文、韩文等字符，
    // emoji（绘文字）以及零长度的空白字符都是有效的 char 值。Unicode 标量值包含从
    // U+0000 到 U+D7FF 和 U+E000 到 U+10FFFF 在内的值。
    let c = 'z';
    let z: char = 'ℤ'; // with explicit type annotation
    let heart_eyed_cat = '😻';

    println!("{} {} {}", c, z, heart_eyed_cat)
}

fn show_compound() {
    // 元组中的每一个位置都有一个类型，而且这些不同值的类型也不必是相同的。
    let tup: (i32, f64, u8) = (500, 6.4, 1);

    // 为了从元组中获取单个值，可以使用模式匹配（pattern matching）来解构（destructure）
    // 元组值
    let (_, y, _) = tup;
    println!("The value of y is: {y}");

    // 也可以使用点号（.）后跟值的索引来直接访问它们。
    let five_hundred = tup.0;
    println!("The value of five_hundred is: {five_hundred}");

    // 不带任何值的元组有个特殊的名称，叫做 单元（unit） 元组。这种值以及对应的类型都写
    // 作 ()，表示空值或空的返回类型。如果表达式不返回任何其他值，则会隐式返回单元值。

    let a = [1, 2, 3, 4, 5];
    // 在方括号中包含每个元素的类型，后跟分号，再后跟数组元素的数量
    let b: [i32; 5] = [1, 2, 3, 4, 5];
    // 通过在方括号中指定初始值加分号再加元素个数的方式来创建一个每个元素都为相同值的数组
    let c = [3; 5];

    println!("{} {} {}", a[0], b[1], c[2]);
}

fn main() {
    const_mut();

    shadowing();

    show_number();

    show_bool();

    show_char();

    show_compound();
}
