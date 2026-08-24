#include "BluetoothSerial.h"
#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>
#include <cmath>

const float PI = 3.1415;

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
const float disang = atan(0.4/9.81) * 180/PI;

const int kp_pitch = 10;
const int kp_roll = 10;
const int kp_yaw=7;
const int kp_hor = 10;

float pitch = 0;
float roll = 0;
float yaw = 0;
float h = 0;

void setup(){
	Serial.begin(115200);
	SerialBT.begin("ESP_Drone");

	Wire.begin(21,22);
	
	m1.attach(dm1);
	m2.attach(dm2);
	m3.attach(dm3);
	m4.attach(dm4);

	mpu.initialize();

	if (!mpu.testConnection()){
		while(1);
	}

	for(int i=0; i<=100; i++){
		m1.writeMicroseconds(1200+i*2);
		m2.writeMicroseconds(1200+i*2);
		m3.writeMicroseconds(1200 + i*2);
		m4.writeMicroseconds(1200 + i*2);
		delay(20);
	}
	
}

void loop(){
	int16_t axr,ayr,azr,gxr,gyr,gzr;

	mpu.getMotion6(&axr,&ayr, &azr, &gxr, &gyr, &gzr);

	static unsigned long lastTime = micros(); 

        unsigned long now = micros();
        
        float dt = (now-lastTime) / 1000000.0;

        lastTime = now;

	
	float ax = axr*(9.80665/16384);
	float ay = ayr*(9.80665/16384);
	float az = azr*(9.80665/16384);

	float gx = gxr/131.0;
	float gy = gyr/131.0;
	float gz = gzr/131.0;
	
	pitch += gy*dt;
	roll += gx*dt;
	yaw += gz*dt;
	h += az*dt;

	char command = 'D';
	
	if (SerialBT.available()){
		command = SerialBT.read();
	}
	
	float dispitch = 0;
	float disroll = 0;
	float disyaw = 0;
	float dishacc = 9.80665;
	
	if (command=='L'){
		disroll = disang;
	}else if (command=='R'){
		disroll = -disang;
	}else if (command=='F'){
		dispitch = -disang;
	}else if (command == 'B'){
		dispitch = disang;
	}else if (command == 'U'){
		dishacc += disacc;
	}else if (command == 'Y'){
		disyaw = disang;
	}else if (command == 'Z'){
		disyaw = -disang;
	}

	float he = dishacc - h;
	float re = disroll - roll;
	float pe = dispitch - pitch;
	float ye = disyaw - yaw;

	int p_h = kp_hor*he;
	int p_r = kp_roll*re;
	int p_p = kp_pitch*pe;
	int p_y = kp_yaw*ye;

	int m1t = thrust + p_h - p_r - p_p + p_y;
	int m2t = thrust + p_h + p_r - p_p - p_y;
	int m3t = thrust + p_h - p_r + p_p - p_y;
	int m4t = thrust + p_h + p_r - p_p + p_y;

	m1t = constrain(m1t, 1000, 2000);
	m2t = constrain(m2t, 1000, 2000);
	m3t = constrain(m3t, 1000, 2000);
	m4t = constrain(m4t, 1000, 2000);

	m1.writeMicroseconds(m1t);
	m2.writeMicroseconds(m2t);
	m3.writeMicroseconds(m3t);
	m4.writeMicroseconds(m4t);

	delay(10);
}
