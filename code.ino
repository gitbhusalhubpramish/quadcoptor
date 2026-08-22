#include "BluetoothSerial.h"
#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>
#include <cmath>

Servo m1, m2, m3, m4;
const int dm1 = 24;
const int dm2 = 23;
const int dm3 = 25;
const int dm4 = 26;

/*
24cw    23ccw
25ccw    26cw
*/

MPU6050 mpu;
BluetoothSerial SerialBT;

int thrust = 1500;//this has to be fixed after drone is made and by checking which valeu is best
const int disacc = 0.4;
const float disang = atan(40/9.81);

void setup(){
	Serial.begin(115200);
	SerialBT.begin("ESP_Drone");

	Wire.begin(21,22);
	
	m1.attach(dm1);
	m2.attach(dm2);
	m3.attach(dm3);
	m4.attach(dm4);

	mpu.initlialize();

	if (!mpu.testConnection()){
		while(1);
	}

	for(int i; i<=100; i++){
		m1.writeMicroseconds(1200+i*2);
		m2.writeMicroseconds(1200+i*2);
		m3.writeMicroseconds(1200 + i*2);
		m4.writeMicroseconds(1200 + i*2);
		delay(20);
	}
	
}

void loop(){
	int16_t ax,ay,az,gx,gy,gz;

	mpu.getMotion6(&ax,&ay, &az, &gx, &gy, &gz);
	
	ax,ay,az = ax*(9.80665/16348), ay*(9.80665/16348), az*(9.80665/16348);
	gx,gy,gz = gx/131, gy/131, gz/131;

	char command = "D";
	
	if (SerialBT.aviable()){
		command = SerialBT.read();
	}
	
	int dispitch = 0;
	int disroll = 0;
	int disyaw = 0;
	int dishacc = 9.80665;
	if (command=="L"){
		disroll = disang;
	}else if (command=="R"){
		disroll = -disang;
	}else if (command=="F"){
		dispitch = -disang;
	}else if (command == "B"){
		dispitch = disang;
	}else if (command == "U"){
		dishacc += disacc
	}

	
	delay(10);
}
