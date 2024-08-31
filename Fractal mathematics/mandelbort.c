#include <stdio.h>  
#include <stdlib.h>  
#include <math.h>  
#include <complex.h>  
  
#pragma pack(push, 1)  
typedef struct {  
    unsigned short type;                 // 图像类型，必须是0x4D42，即字符'BM'  
    unsigned int size;                   // 文件大小  
    unsigned short reserved1;            // 保留字，不考虑  
    unsigned short reserved2;            // 保留字，不考虑  
    unsigned int offset;                 // 从文件头到实际位图数据的偏移  
} BITMAPFILEHEADER;  
  
typedef struct {  
    unsigned int size;                   // 本结构体的大小  
    int width;                           // 图像的宽度  
    int height;                          // 图像的高度  
    unsigned short planes;               // 颜色平面数，必须为1  
    unsigned short bit_count;            // 每像素位数  
    unsigned int compression;            // 压缩类型  
    unsigned int size_image;             // 图像的大小，以字节为单位  
    int x_pixels_per_meter;              // 水平分辨率  
    int y_pixels_per_meter;              // 垂直分辨率  
    unsigned int colors_used;            // 使用的颜色数  
    unsigned int colors_important;       // 重要颜色数  
} BITMAPINFOHEADER;  
#pragma pack(pop)  
  
#define WIDTH 800  
#define HEIGHT 600  
#define MAX_ITER 1000  
#define ESCAPE_RADIUS 2.0  
  
void mandelbrot_bmp(const char *filename) {  
    FILE *file = fopen(filename, "wb");  
    if (!file) {  
        perror("Unable to open file");  
        exit(1);  
    }  
  
    BITMAPFILEHEADER file_header;  
    BITMAPINFOHEADER info_header;  
  
    file_header.type = 0x4D42; // 'BM'  
    file_header.size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + WIDTH * HEIGHT * 3;  
    file_header.reserved1 = 0;  
    file_header.reserved2 = 0;  
    file_header.offset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  
  
    info_header.size = sizeof(BITMAPINFOHEADER);  
    info_header.width = WIDTH;  
    info_header.height = HEIGHT;  
    info_header.planes = 1;  
    info_header.bit_count = 24; // 24-bit bitmap  
    info_header.compression = 0;  
    info_header.size_image = WIDTH * HEIGHT * 3;  
    info_header.x_pixels_per_meter = 2835;  
    info_header.y_pixels_per_meter = 2835;  
    info_header.colors_used = 0;  
    info_header.colors_important = 0;  
  
    fwrite(&file_header, sizeof(BITMAPFILEHEADER), 1, file);  
    fwrite(&info_header, sizeof(BITMAPINFOHEADER), 1, file);  
  
    unsigned char *pixel_data = (unsigned char *)malloc(WIDTH * HEIGHT * 3);  
    if (!pixel_data) {  
        perror("Unable to allocate memory for pixel data");  
        fclose(file);  
        exit(1);  
    }  
  
    for (int y = 0; y < HEIGHT; y++) {  
        for (int x = 0; x < WIDTH; x++) {  
            double real = (x - WIDTH / 1.5) / (WIDTH / 4.0);  
            double imag = (y - HEIGHT / 2) / (HEIGHT / 4.0);  
            complex double c = real + I * imag;  
            complex double z = 0;  
            int iter = 0;  
  
            while (cabs(z) < ESCAPE_RADIUS && iter < MAX_ITER) {  
                z = cpow(z, 2) + c;  
                iter++;  
            }  
  
            unsigned char color = (unsigned char)(255 * iter / MAX_ITER);  
            pixel_data[(y * WIDTH + x) * 3] = color; // Red  
            pixel_data[(y * WIDTH + x) * 3 + 1] = 0; // Green  
            pixel_data[(y * WIDTH + x) * 3 + 2] = (unsigned char)(255 - color); // Blue  
        }  
    }  
  
    fwrite(pixel_data, WIDTH * HEIGHT * 3, 1, file);  
    free(pixel_data);  
    fclose(file);  
}  
  
int main() {  
    mandelbrot_bmp("mandelbrot.bmp");  
    return 0;  
}
