#include <Rainbowduino.h>

int density = 25;
int tail = 8;
int interval = 100;

int rainbow[6][3] = {
	{0xFF, 0x00, 0x00},
	{0xFF, 0x22, 0x00},
	{0xFF, 0xFF, 0x00},
	{0x00, 0xFF, 0x00},
	{0x00, 0x00, 0xFF},
	{0xFF, 0x00, 0x88}
};

int streams[16];
int streamColors[16];

void setup(){
	for(int i = 0; i < 16; i++)
		streams[i] = streamColors[i] = -1;
	Rb.init();
}

void loop(){
	for(int i = 0; i < 16; i++){
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
	int index = 0;
	int results[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	for(int i = 0; i < 16; i++){
		if(streams[i] == -1)
			results[index++] = i;
	}

	if(index > 0){
		int target = results[random(index)];
		streams[target] = 0;
		streamColors[target] = random(6);
	}
}

unsigned long RGBtoLong(unsigned long red, unsigned long green, unsigned long blue){
	return (red << 16) | (green << 8) | blue;
}

float getMultiplier(int sub){
	return ((((float)sub / (float)tail) - 1) * -1) / ((sub * tail) + 1);
}

void draw(){
	unsigned long result[8][8] = {};
	float multipliers[8][8] = {};

	for(int row = 0; row < 8; row++){
		for(int col = 0; col < 8; col++){
			if(streams[col] == row){
				multipliers[row][col] = 1;
				result[row][col] = RGBtoLong(rainbow[streamColors[col]][0], rainbow[streamColors[col]][1], rainbow[streamColors[col]][2]);
			}else if(streams[col] > row){
				int sub = streams[col] - row - 1;
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

	for(int row = 0; row < 8; row++){
		for(int col = 8; col < 16; col++){
			int realCol = col - 8;
			if(streams[col] == row)
				result[realCol][row] = RGBtoLong(rainbow[streamColors[col]][0], rainbow[streamColors[col]][1], rainbow[streamColors[col]][2]);
			else if(streams[col] > row){
				int sub = streams[col] - row - 1;
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
						int shade = 0xFF * multiplier;
						result[realCol][row] = RGBtoLong(shade, shade, shade);
					}
				}
			}
		}
	}

	for(int row = 0; row < 8; row++){
		for(int col = 0; col < 8; col++)
			Rb.setPixelXY(row, col, result[row][col]);
	}
}
