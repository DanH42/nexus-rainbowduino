#include <Rainbowduino.h>

char density = 25;
char tail = 8;
int interval = 100;

int rainbow[6][3] = {
	{0xFF, 0x00, 0x00},
	{0xFF, 0x22, 0x00},
	{0xFF, 0xFF, 0x00},
	{0x00, 0xFF, 0x00},
	{0x00, 0x00, 0xFF},
	{0xFF, 0x00, 0x88}
};

char streams[16];
char streamColors[16];
boolean directions[16];

void setup(){
	for(char i = 0; i < 16; i++)
		streams[i] = streamColors[i] = -1;
	Rb.init();
}

void loop(){
	for(char i = 0; i < 16; i++){
		if(streams[i] != -1){
			if(streams[i] < 7 + tail)
				streams[i] = streams[i] + 1;
			else
				streams[i] = -1;
		}
	}

	if(random(100) <= density)
		addStream();

	draw();
	delay(interval);
}

void addStream(){
	char index = 0;
	char results[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	for(char i = 0; i < 16; i++){
		if(streams[i] == -1)
			results[index++] = i;
	}

	if(index > 0){
		char target = results[random(index)];
		streams[target] = 0;
		streamColors[target] = random(6);
		directions[target] = random(2) == 0;
	}
}

unsigned long RGBtoLong(unsigned long red, unsigned long green, unsigned long blue){
	return (red << 16) | (green << 8) | blue;
}

float getMultiplier(char sub){
	return ((((float)sub / (float)tail) - 1) * -1) / ((sub * tail) + 1);
}

void draw(){
	unsigned long result[8][8] = {};
	float multipliers[8][8] = {};

	for(byte row = 0; row < 8; row++){
		for(byte col = 0; col < 8; col++){
			char realRow = row;
			if(directions[col])
				realRow = (realRow - 7) * -1;
			if(streams[col] == row){
				multipliers[realRow][col] = 1;
				result[realRow][col] = RGBtoLong(rainbow[streamColors[col]][0], rainbow[streamColors[col]][1], rainbow[streamColors[col]][2]);
			}else if(streams[col] > row){
				char sub = streams[col] - row - 1;
				if(sub <= tail){
					float multiplier = getMultiplier(sub);
					multipliers[realRow][col] = multiplier;
					unsigned long color = RGBtoLong(rainbow[streamColors[col]][0] * multiplier,
					                                rainbow[streamColors[col]][1] * multiplier,
					                                rainbow[streamColors[col]][2] * multiplier);
					result[realRow][col] = color;
				}
			}
		}
	}

	for(byte row = 0; row < 8; row++){
		for(char col = 8; col < 16; col++){
			char realRow = col - 8;
			char realCol = row;
			if(directions[col])
				realCol = (realCol - 7) * -1;
			if(streams[col] == row){
				if(result[realRow][realCol] == 0)
					result[realRow][realCol] = RGBtoLong(rainbow[streamColors[col]][0], rainbow[streamColors[col]][1], rainbow[streamColors[col]][2]);
				else
					result[realRow][realCol] = RGBtoLong(255, 255, 255);
			}else if(streams[col] > row){
				char sub = streams[col] - row - 1;
				if(sub <= tail){
					float multiplier = getMultiplier(sub);
					if(result[realRow][realCol] == 0){
						unsigned long color = RGBtoLong(rainbow[streamColors[col]][0] * multiplier,
						                                rainbow[streamColors[col]][1] * multiplier,
						                                rainbow[streamColors[col]][2] * multiplier);
						result[realRow][realCol] = color;
					}else{
						if(multipliers[realRow][realCol] > multiplier)
							multiplier = multipliers[realRow][realCol];
						char shade = 0xFF * multiplier;
						result[realRow][realCol] = RGBtoLong(shade, shade, shade);
					}
				}
			}
		}
	}

	for(byte row = 0; row < 8; row++){
		for(byte col = 0; col < 8; col++)
			Rb.setPixelXY(row, col, result[row][col]);
	}
}
