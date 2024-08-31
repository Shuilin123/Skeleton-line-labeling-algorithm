#include <stdio.h>  
#include <stdlib.h>  
#include <math.h>  
#include <complex.h>  
  
#define WIDTH 800  
#define HEIGHT 600  
#define MAX_ITER 1000  
#define ESCAPE_RADIUS 2.0  
  
void mandelbrot_svg(const char *filename) {  
    FILE *file = fopen(filename, "w");  
    if (!file) {  
        perror("Unable to open file");  
        exit(1);  
    }  
  
    fprintf(file, "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n", WIDTH, HEIGHT);  
  
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
  
            double hue = (double)iter / MAX_ITER;  
            int color = (int)(255 * hue);  
            if (color > 255) color = 255;  
            fprintf(file, "<rect x=\"%d\" y=\"%d\" width=\"1\" height=\"1\" style=\"fill:rgb(%d,0,%d);\" />\n", x, HEIGHT - y - 1, color, 255 - color);  
        }  
    }  
  
    fprintf(file, "</svg>\n");  
    fclose(file);  
}  
  
int main() {  
    mandelbrot_svg("mandelbrot.svg");  
    return 0;  
}
