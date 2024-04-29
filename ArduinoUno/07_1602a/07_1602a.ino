/*
 * @Author: Alan Yin
 * @Date: 2024-03-24 22:33:11
 * @LastEditTime: 2024-03-27 00:34:39
 * @LastEditors: Alan Yin
 * @FilePath: \Code\training\ArduinoUno\07_1602a\07_1602a.ino
 * @Description:
 *
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

/**
 * VSS - 一般接地
 * VDD - 接电源（+5V）
 * V0  - 液晶显示器对比度调整端，接正电源时对比度最弱，接地电源时对比度最高（对比度过高
 * 	 时会产生“鬼影”，使用时可以通过一个10K的电位器调整对比度）。
 * RS  - RS为寄存器选择，高电平1时选择数据寄存器、低电平0时选择指令寄存器
 * R/W - R/W为读写信号线，高电平(1)时进行读操作，低电平(0)时进行写操作
 * E   - E(或EN)端为使能(enable)端，
 * 	 写操作时，下降沿使能;
 * 	 读操作时，E高电平有效;
 * DB0 - 低4位三态、双向数据总线 0位（最低位）
 * DB1 - 低4位三态、双向数据总线 1位
 * DB2 - 低4位三态、双向数据总线 2位
 * DB3 - 低4位三态、双向数据总线 3位
 * DB4 - 高4位三态、双向数据总线 4位
 * DB5 - 高4位三态、双向数据总线 5位
 * DB6 - 高4位三态、双向数据总线 6位
 * DB7 - 高4位三态、双向数据总线 7位（最高位）（也是busy flag）
 * BLA - 背光电源正极
 * BLK - 背光电源负极
 *
 */
int DB0 = 2;
int DB1 = DB0 + 1;
int DB2 = DB1 + 1;
int DB3 = DB2 + 1;
int DB4 = DB3 + 1;
int DB5 = DB4 + 1;
int DB6 = DB5 + 1;
int DB7 = DB6 + 1;

int RS = 10;
int RW = 11;
int EN = 12;

#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ENTRY_MODE 0x06
#define LCD_SURSOR_SHIFT 0x18
#define LCD_FUNCTION_SET 0x38
#define LCD_SET_CGRAM_ADDR 0x40
#define LCD_SET_DDRAM_ADDR 0x80

uint8_t LCD_DISPLAY_CONTROL = 0x0C;

// const uint8_t data_pins[] = {DB7, DB6, DB5, DB4, DB3, DB2, DB1, DB0};
const uint8_t data_pins[] = {DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7};

const char* message = "Hello world!";
uint8_t duck[] = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};

void send_pins(uint8_t data)
{
	for (int i = 0; i < 8; i++) {
		digitalWrite(data_pins[i], (data >> i) & 0x01);
	}
}

uint8_t read_pins() {
	uint8_t sta = 0;
	for (uint8_t i = 0; i < 8; i++) {
		sta |= (uint8_t)digitalRead(data_pins[i]) << i;
	}
	return sta;
}

// 等待LCD1602准备好
void wait_ready()
{
	uint8_t sta;
	// 读取LCD状态RS=0，RW=1
	digitalWrite(RS, LOW);
	digitalWrite(RW, HIGH);

	// 将引脚设置为输入模式
	for (uint8_t i = 0; i < 8; i++) {
		pinMode(data_pins[i], INPUT);
	}

	// 读取引脚状态
	do {
		digitalWrite(EN, HIGH);
		sta = read_pins();
		digitalWrite(EN, LOW);
	} while (sta & 0x80);  // 读取Bit7的Busy Flag

	// 将引脚重新设置为输出
	for (uint8_t i = 0; i < 8; i++) {
		pinMode(data_pins[i], OUTPUT);
	}
}

// 写命令
void write_cmd(uint8_t cmd)
{
	wait_ready();
	// 写命令RS=0，RW=0
	digitalWrite(RS, LOW);
	digitalWrite(RW, LOW);
	send_pins(cmd);
	// 发送命令需要EN高脉冲
	digitalWrite(EN, HIGH);
	digitalWrite(EN, LOW);
}

// 写数据到DDRAM
void write_data(uint8_t dat)
{
	wait_ready();
	// 写数据RS=1，RW=0
	digitalWrite(RS, HIGH);
	digitalWrite(RW, LOW);
	send_pins(dat);
	// 发送数据需要EN高脉冲
	digitalWrite(EN, HIGH);
	digitalWrite(EN, LOW);
}

// // 打印字符
// void print(const char cha)
// {
// 	write_data(cha);
// }

// // 打印字符串
// void print(const char* str)
// {
// 	while (*str) {
// 		write_data(*str++);  // 先取指针的值，再将指针地址自增
// 	}
// }

// void init_pin_mode()
// {
// 	pinMode(DB0, OUTPUT);
// 	pinMode(DB1, OUTPUT);
// 	pinMode(DB2, OUTPUT);
// 	pinMode(DB3, OUTPUT);
// 	pinMode(DB4, OUTPUT);
// 	pinMode(DB5, OUTPUT);
// 	pinMode(DB6, OUTPUT);
// 	pinMode(DB7, OUTPUT);
// 	pinMode(RS , OUTPUT);
// 	pinMode(RW , OUTPUT);
// 	pinMode(EN , OUTPUT);
// }

void lcd_init()
{
	pinMode(EN, OUTPUT);
	pinMode(RS, OUTPUT);
	pinMode(RW, OUTPUT);

	// 设置模式
	write_cmd(0x38);
	// 关闭显示
	write_cmd(0x0f);
	// 清屏
	write_cmd(0x01);
	// 进入模式
	write_cmd(0x06);
}

// 清空屏幕
void lcd_clear() {
    write_cmd(LCD_CLEAR);
}

// 光标以及光标指针回到初始状态
void lcd_home() {
    write_cmd(LCD_HOME);
}

// 关闭LCD
void display_off() {
    LCD_DISPLAY_CONTROL &= ~0x04;
    write_cmd(LCD_DISPLAY_CONTROL);
}

// 打开LCD
void display_on() {
    LCD_DISPLAY_CONTROL |= 0x04;
    write_cmd(LCD_DISPLAY_CONTROL);
}

// 关闭光标
void cursor_off() {
    LCD_DISPLAY_CONTROL &= ~0x02;
    write_cmd(LCD_DISPLAY_CONTROL);
}

// 打开光标
void cursor_on() {
    LCD_DISPLAY_CONTROL |= 0x02;
    write_cmd(LCD_DISPLAY_CONTROL);
}

// 关闭光标闪烁
void blink_off() {
    LCD_DISPLAY_CONTROL &= ~0x01;
    write_cmd(LCD_DISPLAY_CONTROL);
}

// 打开光标闪烁
void blink_on() {
    LCD_DISPLAY_CONTROL |= 0x01;
    write_cmd(LCD_DISPLAY_CONTROL);
}

// 向左滚动一列
void scroll_left() {
    write_cmd(LCD_SURSOR_SHIFT);
}

// 向右滚动一列
void scroll_right() {
    write_cmd(LCD_SURSOR_SHIFT | 0x04);
}

// 设置光标位置
void set_cursor(uint8_t x, uint8_t y) {
    uint8_t addr;
    if (y == 0)
        addr = 0x00 + x;  // 第一行字符地址从 0x00开始
    else
        addr = 0x40 + x;  // 第二行字符地址从 0x40开始
    write_cmd(LCD_SET_DDRAM_ADDR | addr);  // 设置DDRAM地址
}

// 打印字符
void print(const char cha) {
    write_data(cha);
}

// 打印字符串
void print(const char* str) {
    while (*str) {
        write_data(*str++);  // 先取指针的值，再将指针地址自增
    }
}

// 自定义一个新的CGRAM字符
void creat_char(uint8_t num, uint8_t* bit_map) {
	// 设置CGRAM地址
	write_cmd(LCD_SET_CGRAM_ADDR + 8 * num);
	for (uint8_t i = 0; i < 8; i++) {
		write_data(bit_map[i]);
	}
}

void setup()
{
	lcd_init();
	creat_char(0, duck);
	set_cursor(0, 0);
	print(message);
	print('*');
	set_cursor(2, 1);
	print("Have a nice day! By Mr.Addict!");
	set_cursor(0, 1);
	write_data(0);
}

void loop()
{
	// val = Serial.read();
	// if (val <= '9' && val >= '0') {
	// 	val = val - '0';
	// 	Serial.println(val);
	// } else if (val >= 'a') {
	// 	val = val - 'a' + 10;
	// 	Serial.println(val);
	// }

	// if (val != -1 && val != old_val) {
	// 	old_val = val;
	// 	Serial.println("set pin mode --> ");
	// 	Serial.println(old_val);
	// 	pinMode(old_val, OUTPUT);
	// }
	// Serial.println(val);
	// Serial.println(old_val);

	// digitalWrite(old_val, HIGH);	//点亮数字13 口LED。
	// delay(500);
	// digitalWrite(old_val, LOW);	//熄灭数字13 口LED
	// delay(500);


	delay(500);
	delay(500);
}
