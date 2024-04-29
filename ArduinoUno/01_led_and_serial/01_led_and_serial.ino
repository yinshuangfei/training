/*
 * @Author: Alan Yin
 * @Date: 2024-03-23 21:53:05
 * @LastEditTime: 2024-03-24 13:32:07
 * @LastEditors: Alan Yin
 * @FilePath: \Code\training\ArduinoUno\01_led_and_serial\01_led_and_serial.ino
 * @Description:
 *
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

int val;	//定义变量val
int ledpin=13;	//定义数字接口13

void setup()
{
	Serial.begin(115200);	//设置波特率为9600，这里要跟软件设置相一致。当接入特定设备（如：蓝牙）时，我们也要跟其他设备的波特率达到一致。
	pinMode(ledpin, OUTPUT);	//设置数字13 口为输出接口，Arduino 上我们用到的I/O 口都要进行类似这样的定义。
}

void loop()
{
	val = Serial.read();//读取PC 机发送给Arduino 的指令或字符，并将该指令或字符赋给val

	//判断接收到的指令或字符是否是“R”。
	if (val=='R') {//如果接收到的是“R”字符
		digitalWrite(ledpin,HIGH);//点亮数字13 口LED。
		delay(500);
		digitalWrite(ledpin,LOW);//熄灭数字13 口LED
		delay(500);
		Serial.println("Hello World!");//显示“Hello World！”字符串
	}
}
