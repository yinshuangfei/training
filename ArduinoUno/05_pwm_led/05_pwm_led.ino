/*
 * @Author: Alan Yin
 * @Date: 2024-03-24 14:34:11
 * @LastEditTime: 2024-03-24 15:35:09
 * @LastEditors: Alan Yin
 * @FilePath: \Code\training\ArduinoUno\05_pwm_led\05_pwm_led.ino
 * @Description: PWM调光
 *
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
int potpin=0;//定义模拟接口0
int ledpin=11;//定义数字接口11（PWM 输出）
int val=0;// 暂存来自传感器的变量数值

void setup()
{
	pinMode(ledpin, OUTPUT);//定义数字接口11 为输出
	Serial.begin(115200);//设置波特率为9600
	//注意：模拟接口自动设置为输入
}
void loop()
{
	val=analogRead(potpin);// 读取传感器的模拟值并赋值给val
	Serial.println(val);//显示val 变量
	analogWrite(ledpin,val/4);// 打开LED 并设置亮度（PWM 输出最大值255）
	delay(10);//延时10ms
}
