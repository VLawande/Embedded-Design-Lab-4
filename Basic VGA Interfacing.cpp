#include "address_map_arm.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>


using namespace std;

const unsigned int LW_BRIDGE_BASE = 0xC8000000;
const unsigned int FINAL_PHYSICAL_ADDRESS = 0xFFFEC700;
const unsigned int LW_BRIDGE_SPAN = FINAL_PHYSICAL_ADDRESS - LW_BRIDGE_BASE;


void video_text(int, int, char *, char *);
void video_box(int, int, int, int, short, char *);
int resample_rgb(int, int);
int get_data_bits(int);
void video_circle(int, int, int, short, char *);

#define STANDARD_X 320
#define STANDARD_Y 240
#define INTEL_BLUE 0x0071C5
#define GREEN 0x002760

int screen_x;
int screen_y;
int res_offset;
int col_offset;

char *Initialize(int *fd) {
	*fd = open( "/dev/mem", (O_RDWR | O_SYNC));
	if (*fd == -1){ 
		cout << "ERROR: could not open /dev/mem..." << endl;
		exit(1);
	}
	char *virtual_base = (char *)mmap (NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, *fd, LW_BRIDGE_BASE);
	if (virtual_base == MAP_FAILED){
		cout << "ERROR: mmap() failed..." << endl;
		close (*fd);
		exit(1);
	}   
	return virtual_base;
}

void Finalize(char *pBase, int fd){
	if (munmap (pBase, LW_BRIDGE_SPAN) != 0)
	{
		cout <<"ERROR:  munmap() failed.." <<endl;
		exit(1);
	}
	close (fd);
}

int main(void) {
	int fd;
	char *pBase = Initialize(&fd);

	volatile int * video_resolution = (int *)(pBase + (PIXEL_BUF_CTRL_BASE - LW_BRIDGE_BASE) + 0x8); 
	cout<<video_resolution<<endl;
	screen_x = *video_resolution & 0xFFFF;
	screen_y = (*video_resolution >> 16) & 0xFFFF;

	volatile int * rgb_status = (int *)(pBase+(RGB_RESAMPLER_BASE-LW_BRIDGE_BASE)); 

	int db = get_data_bits(*rgb_status & 0x3F);

	res_offset = (screen_x == 160) ? 1 : 0;
	col_offset = (db == 8) ? 1 : 0;

	char text_top_row[40] = "EECE 2160-Fall-2021";
	char text_bottom_row[40] = "Viraj Lawande and Matas Suziedelis";

	char text_space[40] = "                                  ";

	for (int a =0; a<80; a++){
		for (int b=0; b<60; b++){
			video_text(a, b, text_space, pBase);	
		}
	}

	video_text(30, 29, text_top_row, pBase);
	video_text(23, 30, text_bottom_row, pBase);

	short background_color = resample_rgb(db, INTEL_BLUE);

	//this one is the background
	video_box(0, 0, STANDARD_X, STANDARD_Y, 0, pBase);

	//this one is the green box
	video_box(21*4-2, 27*4+2, 58*4 + 1, 33*4 - 3, GREEN, pBase);
	//this one is the blue box
	video_box(21*4, 28*4, 58*4 - 1, 32*4 - 1, background_color, pBase);

	Finalize(pBase, fd);
	return 0;
}

void video_text(int x,int y,char * text_ptr, char * pBase) {
	int offset;
	volatile char * character_buffer = (char *)(pBase+(FPGA_CHAR_BASE-LW_BRIDGE_BASE));
	offset = (y << 7) + x;
	while(* (text_ptr)){
		*(character_buffer + offset) = *(text_ptr);
		++text_ptr;
		++offset;
	}
}

void video_box(int x1,int y1,int x2,int y2,short pixel_color, char * pBase) {
	int pixel_buf_ptr = *(int*)(pBase + (PIXEL_BUF_CTRL_BASE-LW_BRIDGE_BASE));
	int pixel_ptr, row, col;
	int x_factor = 0x1 << (res_offset + col_offset);
	int y_factor = 0x1 << (res_offset);
	x1 = x1 / x_factor;
	x2 = x2 / x_factor;
	y1 = y1 / y_factor;
	y2 = y2 / y_factor;

	for(row = y1; row <= y2; row++) {
		for(col = x1; col <= x2; ++col) {
			pixel_ptr = pixel_buf_ptr +(row << (10 - res_offset - col_offset)) + (col << 1);
			*(short *)(pBase + (pixel_ptr-LW_BRIDGE_BASE)) = pixel_color;
		}
	}
}

int resample_rgb(int num_bits,int color) {
	if(num_bits == 8) {
		color = (((color >> 16) & 0x000000E0) | ((color >> 11) & 0x0000001C) |((color >> 6) & 0x00000003));
		color = (color << 8) | color;
	} else if(num_bits == 16) {
		color = (((color >> 8) & 0x0000F800) | ((color >> 5) & 0x000007E0) |((color >> 3) & 0x0000001F));
	}
	return color;
}

int get_data_bits(int mode) {
	switch(mode) {
		case 0x0: return 1;
		case 0x7: return 8;
		case 0x11: return 8;
		case 0x12: return 9;
		case 0x14: return 16;
		case 0x17: return 24;
		case 0x19: return 30;
		case 0x31: return 8;
		case 0x32: return 12;
		case 0x33: return 16;
		case 0x37: return 32;
		case 0x39: return 40;
	}
}