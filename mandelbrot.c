#include <stdio.h>  
#include <stdlib.h>  
#include <math.h>  
#include <complex.h>  
  
#pragma pack(push, 1)  
typedef struct {  
    unsigned short type;                 // ͼ�����ͣ�������0x4D42�����ַ�'BM'  
    unsigned int size;                   // �ļ���С  
    unsigned short reserved1;            // �����֣�������  
    unsigned short reserved2;            // �����֣�������  
    unsigned int offset;                 // ���ļ�ͷ��ʵ��λͼ���ݵ�ƫ��  
} BITMAPFILEHEADER;  
  
typedef struct {  
    unsigned int size;                   // ���ṹ��Ĵ�С  
    int width;                           // ͼ��Ŀ��  
    int height;                          // ͼ��ĸ߶�  
    unsigned short planes;               // ��ɫƽ����������Ϊ1  
    unsigned short bit_count;            // ÿ����λ��  
    unsigned int compression;            // ѹ������  
    unsigned int size_image;             // ͼ��Ĵ�С�����ֽ�Ϊ��λ  
    int x_pixels_per_meter;              // ˮƽ�ֱ���  
    int y_pixels_per_meter;              // ��ֱ�ֱ���  
    unsigned int colors_used;            // ʹ�õ���ɫ��  
    unsigned int colors_important;       // ��Ҫ��ɫ��  
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
