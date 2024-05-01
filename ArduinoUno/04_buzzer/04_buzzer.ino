/*
 * @Author: Alan Yin
 * @Date: 2024-03-24 13:28:47
 * @LastEditTime: 2024-03-24 14:32:17
 * @LastEditors: Alan Yin
 * @FilePath: \Code\training\ArduinoUno\04_buzzer\04_buzzer.ino
 * @Description: 蜂鸣器
 *
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

int buzzer = 8;//设置控制蜂鸣器的数字IO脚
int i = 0;
int val, old_val;

void setup()
{
	pinMode(buzzer,OUTPUT);//设置数字IO脚模式，OUTPUT为输出
	Serial.begin(115200);
}

void loop()
{
	Serial.println("Make your choice: 1, 2");

	while ((val = Serial.read()) == -1) {
		delay(100);
	}

	if (val != old_val) {
		char *str = (char *)malloc(64);
		sprintf(str, "your choice: '%d'", val - '0');
		Serial.println(str);
		free(str);
		old_val = val;
	}

	switch (val) {
	case '1':
		sound_1();
		break;
	case '2':
		sound_2();
		break;
	default:
		break;
	}
}

void sound_1()
{
	int cmd = 'q';
	while ((cmd = Serial.read()) != 'q') {
		//输出一个频率的声音
		for (i = 0; i < 80; i++) {
			digitalWrite(buzzer,HIGH);//发声音
			delay(1);//延时1ms
			digitalWrite(buzzer,LOW);//不发声音
			delay(1);//延时ms
		}

		//输出另一个频率的声音
		for (i = 0; i < 100; i++) {
			digitalWrite(buzzer,HIGH);//发声音
			delay(2);//延时2ms
			digitalWrite(buzzer,LOW);//不发声音
			delay(2);//延时2ms
		}
	}
}

void sound_2()
{
	int cmd = 'q', f = 0;
	while ((cmd = Serial.read()) != 'q') {
		// delay(100);//延时ms

		if (cmd != -1) {
			Serial.println(cmd - '0', 10);
			f = cmd - '0';
		}

		//输出一个频率的声音
		for (i = 0; i < 80; i++) {
			digitalWrite(buzzer,HIGH);//发声音
			delay(f);//延时1ms
			digitalWrite(buzzer,LOW);//不发声音
			delay(1);//延时ms
		}
	}
}