/* (blob.plot) files containing the set of extracted blobs.
 * The GNUplot file can be plotted with :
 *     plot "blob.plot" lc variable with lines         
 *  author: shuilin 903103831@qq.com
 * Licensed under the Tianjin University of Technology and Education (c) 2024-2037  
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdint.h>
#include "stb_image.h"
#include "blob.h" 
#include <time.h>
#define LINE_MAX 24
static const uint8_t palette[8][3] = {
        {0xff,0x00,0x00},
        {0x00,0xff,0x00},
        {0xff,0xff,0x00},
        {0x00,0x00,0xff},
        {0xff,0x00,0xff},
        {0x00,0xff,0xff},
        {0xff,0xff,0xff},
        {0x7f,0x00,0x7f}
};
typedef union {
    uint8_t bytes[4];
    uint32_t value;
}DATA;
// Create a bitmap with a 3-dimensional data matrix
int bmp_create(const char *fileName, uint32_t width, uint32_t height, uint8_t bmp[width][height][3]){
    FILE *fp;
    uint32_t i, j;
    DATA l_width, l_height, l_bfSize, l_biSizeImage;
    uint32_t width_r  =  (width * 24 / 8 + 3) / 4 * 4;
    uint32_t bfSize = width_r * height + 54 + 2;
    uint32_t biSizeImage = width_r * height;
    l_width.value = width;
    l_height.value = height;
    l_bfSize.value = bfSize;
    l_biSizeImage.value = biSizeImage;
    uint8_t bmp_head_map[54] = {
            //bmp file header: 14 byte
            0x42, 0x4d,
            // bmp pixel size: width * height * 3 + 54
            l_bfSize.bytes[0], l_bfSize.bytes[1], l_bfSize.bytes[2], l_bfSize.bytes[3],
            0, 0 , 0, 0,
            54, 0 , 0, 0,    /* 14+40=54 */
            /* bmp map info: 40 byte */
            40, 0, 0, 0,
            //width
            l_width.bytes[0], l_width.bytes[1], l_width.bytes[2], l_width.bytes[3],
            //height
            l_height.bytes[0], l_height.bytes[1], l_height.bytes[2], l_height.bytes[3],
            1, 0,
            24, 00,             /* 24 bit: R[8]/G[8]/B[8] */

            0, 0, 0, 0,     //biCompression:0
            l_biSizeImage.bytes[0], l_biSizeImage.bytes[1], l_biSizeImage.bytes[2], l_biSizeImage.bytes[3],
            0, 0, 0, 0,     //biXPelsPerMeter: 60 0F 00 00
            0, 0, 0, 0,     //biYPelsPerMeter
            0, 0, 0, 0,     //biClrUsed
            0, 0, 0, 0      //biClrImportant
    };
    /* write in binary format */
    fp = fopen(fileName, "wb+");
    if(fp == NULL)
    {
        printf("%s: file create failed!\n", fileName);
        return -1;
    }
    fwrite(bmp_head_map, sizeof(bmp_head_map), 1, fp);

    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++)
            fprintf(fp, "%c%c%c", bmp[j][i][2], bmp[j][i][1], bmp[j][i][0]); /* BGR */
        //4 byte align
        for(j = 0; j < width_r-width*3; j++)
            fprintf(fp, "%c", 0);
    }
    fprintf(fp, "%c%c", 0, 0); //PhotoShop two byte "0"
    if(fclose(fp))
    {
        printf("file close failed!\n");
        return -1;
    }
    fp = NULL;
    return 0;
}
//Read the *.plot file to convert to bmp picture 
int countour_write_png(int width, int height, const char *filename_in,const char *filename_out){
	uint8_t bmp[width][height][3];
	memset(bmp,0,sizeof(3*width*height));
	static const int palette_len = sizeof(palette) / sizeof(palette[0]);
	FILE *fp;
	int line_len = 0; // The length of each line of the file
	char buffer[LINE_MAX] = {0}; // Row data cache
	fp = fopen(filename_in, "r");
	if (NULL == fp) {
		printf("open %s failed.\n", filename_out);
		return -1;
	}
	while(fgets(buffer,LINE_MAX,fp)){
		line_len = strlen(buffer);
		int nums[3]={0},i=0,j=0,flag=1;
		while(!isdigit(buffer[i])){//Skip the previous space 
			i++;
		}
		for(;i<line_len;i++){
			if(isdigit(buffer[i])){
				flag=1;
			   	nums[j]=10*nums[j]+(buffer[i]-'0');
			}else{
				if(flag){
					j++;
					flag=0;
				}
			}
		}
	    if(nums[0]!=0){//file handling
		    int index = (nums[2]-1) % palette_len;
		    bmp[nums[0]][nums[1]][0]  = palette[index][0];
            bmp[nums[0]][nums[1]][1] = palette[index][1];
            bmp[nums[0]][nums[1]][2] = palette[index][2];
		} 
	}
    if(0!=bmp_create(filename_out,width,height,bmp)){
     	fprintf(stderr, "failed to write %s\n",filename_out);
		return -1;	
	}
    fclose(fp);
    return 0;
}
//write contour as GNUplot data
void contour_write_plot(contour_t *contour, int16_t label, FILE *out){
    int i;
    int16_t *ptr = contour->points;
    for(i=contour->count; i>0; i--, ptr+=2)
    {
        fprintf(out, "%5d    %5d    %5d\n", ptr[0], ptr[1], label);
    }
}
//write blob contours in a GNUplot file
int blob_write_plot(blob_t *blobs, int count, const char *filename){
    int i;
    FILE *out = fopen(filename, "wb");
    if(NULL == out)
    {
        fprintf(stderr, "failed to open %s : %s\n", filename, strerror(errno));
        return 0;
    }

    for(i=0; i<count; i++)
    {
        contour_write_plot(&blobs[i].external, 2 * blobs[i].label, out);
        fprintf(out, "\n");
        if(NULL != blobs[i].internal)
        {
            int j;
            for(j=0; j<blobs[i].internal_count; j++)
            {
                contour_write_plot(blobs[i].internal + j,  (2 * blobs[i].label) + 1, out);
                fprintf(out, "\n");
            }
        }
    }
    fclose(out);
    return 1;
}

//simple dummy threshold
void threshold(uint8_t *source, int width, int height, uint8_t v){
    int i;
    for(i=width*height; i>0; i--)
    {
        uint8_t c = (*source >= v) ? 1 : 0;//二值化 
        *source++ = c;
    }
}

int main(int argc, char **argv){
	int ret,width  = 0,height = 0,idx, opt;
    clock_t start, finish;
    uint8_t *image = NULL;
    
    label_t *label  = NULL;
    int16_t label_width = 0;
    int16_t label_height = 0;
    
    blob_t *blobs;

    int16_t roi_x, roi_y, roi_w, roi_h;
    roi_x = roi_y = 0;
    roi_w = roi_h = -1;
    char file_name[100],out_file[100];
    printf("Step1. please enter the image to be processed(.\\input\\dummy1.png):");
    scanf("%s",file_name);
    printf("Setp2. please enter the path and name of the processed image(.\\result\\dummy1.png):");
    scanf("%s",out_file);
	printf("Processing....\n");
	start = clock(); 
    image = stbi_load(file_name, &width, &height, NULL, 1);//读取文件 
    if(NULL == image){
        fprintf(stderr, "failed to read image : %s\n", file_name);
        return EXIT_FAILURE;
    }

    threshold(image, width, height,128);//128 

    blobs = NULL;
    int blob_count = 0;
    
    if(roi_w < 0) { roi_w = width; }
    if(roi_h < 0) { roi_h = height; }
    
    ret = EXIT_FAILURE;
    
    if(find_blobs(roi_x, roi_y, roi_w, roi_h, image, width, height, &label, &label_width, &label_height, &blobs, &blob_count, 1) )
    {
        if(blob_write_plot(blobs, blob_count, "blob.plot")){
            ret = EXIT_SUCCESS;
            countour_write_png(width,height,"./blob.plot",out_file);
            
        }
    }
    finish = clock();
	double Total_time = (double)(finish - start) / CLOCKS_PER_SEC; //单位换算成秒
	printf("Algorithm running time is %f s.\n", Total_time);
    system(out_file);
    printf("End of processing.");
    destroy_blobs(blobs, blob_count);
    if(NULL != label)
    {
        free(label);
    }
    free(image);
    return ret;
}
