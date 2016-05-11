#include <stdio.h>
#include <stdlib.h>
#include "declaration.h"
void Read_DMEMORY(){
	FILE *dimage = fopen("dimage.bin", "rb");
	 int i;
	
	fseek(dimage, 0L, SEEK_END); /* go to the end of file */
	fileSize_d = ftell(dimage);    /* find where it is */
	fseek(dimage, 0L, SEEK_SET); /* go to the beginning */
	
	
	 fread(d_data  , 4, fileSize_d /4, dimage);

	for(i = 0; i < 2; i++){
		d_data[i] = d_data[i] << 24 | d_data[i] >> 8 << 24 >> 8 | d_data[i] >> 16 << 24 >> 16 | d_data[i] >> 24;
	}
	for(i = 2; i < 2+d_data[1]; i++){
		d_data[i] = d_data[i] << 24 | d_data[i] >> 8 << 24 >> 8 | d_data[i] >> 16 << 24 >> 16 | d_data[i] >> 24;
	}
	reg[29] = d_data[0];
	for(i = 0; i < d_data[1]; i++){
		d_memory[i*4] = d_data[i+2] >> 24;
		d_memory[i*4 + 1] = d_data[i+2] << 8 >> 24;
		d_memory[i*4 + 2] = d_data[i+2] << 16 >> 24;
		d_memory[i*4 + 3] = d_data[i+2] << 24 >> 24;
	}
	fclose(dimage);

}
void Read_IMEMORY(){
	FILE *iimage = fopen("iimage.bin", "rb");
	
	fseek(iimage, 0L, SEEK_END); /* go to the end of file */
	fileSize_i = ftell(iimage);    /* find where it is */
	fseek(iimage, 0L, SEEK_SET); /* go to the beginning */
	/* now allocate a buffer of the size */
	
	//mem_code_i = malloc(fileSize_i);
	fread(i_memory, 4, fileSize_i/4, iimage);
	int i;
	for(i = 0; i < 2; i++){
		i_memory[i] = i_memory[i] << 24 | i_memory[i] >> 8 << 24 >> 8 | i_memory[i] >> 16 << 24 >> 16 | i_memory[i] >> 24;
	}
	for(i = 2; i < 2+i_memory[1]; i++){
		i_memory[i] = i_memory[i] << 24 | i_memory[i] >> 8 << 24 >> 8 | i_memory[i] >> 16 << 24 >> 16 | i_memory[i] >> 24;
	}
	for(; i < 1024; i++)
		i_memory[i] = 0;
	PC = i_memory[0];
	fclose(iimage);
	
}