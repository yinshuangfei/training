/*
 * @Author: Alan Yin
 * @Date: 2024-03-23 21:53:05
 * @LastEditTime: 2024-03-24 13:21:59
 * @LastEditors: Alan Yin
 * @FilePath: \Code\training\ArduinoUno\03_leds\03_leds.ino
 * @Description: 广告灯
 *
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
int BASE = 2;	//第一颗 LED 接的 I/O 脚
int NUM = 6;	//LED 的总数
int i = 0;

void setup()
{
	for (int i = BASE; i < BASE + NUM; i ++) {
		pinMode(i, OUTPUT);   //设定数字I/O脚为输出
	}
}

void loop()
{
	for (i = BASE; i < BASE + NUM; i ++) {
		digitalWrite(i, LOW);	//设定数字I/O脚出为"低"，即顺序熄灭
		delay(50);		//延时
	}

	for (i = BASE; i < BASE + NUM; i ++) {
		digitalWrite(i, HIGH);	//设定数字I/O脚输出为"低"，即顺序点亮
		delay(50);		//延时
	}
	delay(500);
}

