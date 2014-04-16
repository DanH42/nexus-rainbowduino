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
			if(streams[col] == row){
				multipliers[row][col] = 1;
				result[row][col] = RGBtoLong(rainbow[streamColors[col]][0], rainbow[streamColors[col]][1], rainbow[streamColors[col]][2]);
			}else if(streams[col] > row){
				char sub = streams[col] - row - 1;
				if(sub <= tail){
					float multiplier = getMultiplier(sub);
					multipliers[row][col] = multiplier;
					unsigned long color = RGBtoLong(rainbow[streamColors[col]][0] * multiplier,
					                                rainbow[streamColors[col]][1] * multiplier,
					                                rainbow[streamColors[col]][2] * multiplier);
					result[row][col] = color;
				}
			}
		}
	}

	for(byte row = 0; row < 8; row++){
		for(char col = 8; col < 16; col++){
			char realCol = col - 8;
			if(streams[col] == row)
				result[realCol][row] = RGBtoLong(rainbow[streamColors[col]][0], rainbow[streamColors[col]][1], rainbow[streamColors[col]][2]);
			else if(streams[col] > row){
				char sub = streams[col] - row - 1;
				if(sub <= tail){
					float multiplier = getMultiplier(sub);
					if(result[realCol][row] == 0){
						unsigned long color = RGBtoLong(rainbow[streamColors[col]][0] * multiplier,
						                                rainbow[streamColors[col]][1] * multiplier,
						                                rainbow[streamColors[col]][2] * multiplier);
						result[realCol][row] = color;
					}else{
						if(multipliers[realCol][row] > multiplier)
							multiplier = multipliers[realCol][row];
						char shade = 0xFF * multiplier;
						result[realCol][row] = RGBtoLong(shade, shade, shade);
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
