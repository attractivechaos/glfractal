/* MIT license; Copyright (c) 2012 by Attractive Chaos <attractor@live.co.uk>

	 gcc -Wall -O2 glfractal.c -o glfractal -framework GLUT -framework OpenGL  # Mac OS X
	 gcc -Wall -O2 glfractal.c -o glfractal -lglut -lGL -lGLU -lX11 -lm        # Linux (not tried)

   With reference to:

     http://www.postulate.org/attractor/attractor.zip         (a good one, but dead unfortunately)
	 http://nuclear.mutantstargoat.com/articles/sdr_fract/
     http://rosettacode.org/wiki/Mandelbrot_set#C
*/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

typedef struct {
	unsigned char r, g, b;
} rgbcolor_t;

static rgbcolor_t g_palette_neon[256], g_palette_blues[256], g_palette_custom[256]; /* defined at the end of the file */

static struct {
	int width, height, max_iter, ns, np, pal_id;
	double xmin, xmax, ymin, ymax;
	unsigned char *buf;
	rgbcolor_t *palette[3];
} g_data = { 800, 600, 256, 1, 1, 0, -2, 1.2, -1.2, 1.2, NULL, {g_palette_neon, g_palette_blues, NULL} }; /* NB: 800/600=(1.2+2)/(1.2+1.2)=4/3 */

/********************
 * Mandelbrot plane *
 ********************/

void glf_mbrot_simple(unsigned char *buf, int max_iter, int w, int h, double xmin, double xmax, double ymin, double ymax, const rgbcolor_t palette[256])
{
	int i, j, k;
	double w1, h1, x0, y0;
	rgbcolor_t *p = (rgbcolor_t*)buf;
	w1 = 1. / w; h1 = 1. / h;
	for (j = 0; j < h; ++j) {
		y0 = ymin + (ymax - ymin) * j * h1;
		for (i = 0; i < w; ++i) {
			double x = x0 = xmin + (xmax - xmin) * i * w1, y = y0;
			for (k = 0; k < max_iter; ++k) {
				double z = x * y;
				x *= x; y *= y;
				if (x + y >= 4) break;
				x = x - y + x0;
				y = z + z + y0; 
			}
			*p++ = palette[k&0xff];
		}
	}
}

/**************************
 * GLUT related functions *
 **************************/

static void cb_draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glf_mbrot_simple(g_data.buf, g_data.max_iter, g_data.width, g_data.height, g_data.xmin, g_data.xmax, g_data.ymin, g_data.ymax, g_data.palette[g_data.pal_id]);
	glDrawPixels(g_data.width, g_data.height, GL_RGB, GL_UNSIGNED_BYTE, g_data.buf);
	glutSwapBuffers();
}

static void cb_key(unsigned char key, int x, int y)
{
	if (key == 27 || key == 'q' || key == 'Q') {
		free(g_data.buf);
		exit(0);
	} else if (isdigit(key)) {
		if (key == '0') key = 10 + '0';
		g_data.max_iter = (int)(key - '0') << 8;
		fprintf(stderr, "(MM) Max iteration: %d\n", g_data.max_iter);
		cb_draw();
	} else if (key == 'd' || key == 'D') {
		unsigned i, x = g_data.max_iter;
		unsigned char *p;
		double f[4];
		f[0] = g_data.xmin; f[1] = g_data.xmax; f[2] = g_data.ymin; f[3] = g_data.ymax;
		for (i = 0, p = (unsigned char*)&x; i < 4; ++i) fprintf(stderr, "%.2x", p[i]);
		for (i = 0, p = (unsigned char*)f; i < 32; ++i) fprintf(stderr, "%.2x", p[i]);
		fprintf(stderr, "\n");
	} else if (key == 's' || key == 'S') {
		char fn[32];
		FILE *fp;
		sprintf(fn, "screen%.3d.ppm", g_data.np++);
		if ((fp = fopen(fn, "wb")) != NULL) {
			fprintf(fp, "P6\n%d %d\n255\n", g_data.width, g_data.height);
			fwrite(g_data.buf, 1, 3 * g_data.width * g_data.height, fp);
			fclose(fp);
			fprintf(stderr, "(MM) Save screenshot to file '%s'.\n", fn);
		}
	} else if (key == '>' || key == '.') {
		g_data.max_iter += 256;
		cb_draw();
		fprintf(stderr, "(MM) Max iteration: %d\n", g_data.max_iter);
	} else if (key == '<' || key == ',') {
		if (g_data.max_iter >= 512) g_data.max_iter -= 256;
		cb_draw();
		fprintf(stderr, "(MM) Max iteration: %d\n", g_data.max_iter);
	} else if (key == 'p' || key == 'P') {
		g_data.pal_id = (g_data.pal_id + 1) % 3;
		if (g_data.palette[g_data.pal_id] == NULL)
			g_data.pal_id = (g_data.pal_id + 1) % 3;
		cb_draw();
	}
}

static void cb_mouse(int bn, int state, int x, int y)
{
	if (state == GLUT_UP && (bn == GLUT_LEFT_BUTTON || bn == GLUT_RIGHT_BUTTON)) {
		double xc = g_data.xmin + (g_data.xmax - g_data.xmin) * x / g_data.width;
		double yc = g_data.ymin + (g_data.ymax - g_data.ymin) * (g_data.height - y) / g_data.height;
		double xstep = (g_data.xmax - g_data.xmin) * (bn == 0? .5 : 2.) * .5;
		double ystep = (g_data.ymax - g_data.ymin) * (bn == 0? .5 : 2.) * .5;
		g_data.xmin = xc - xstep, g_data.xmax = xc + xstep;
		g_data.ymin = yc - ystep, g_data.ymax = yc + ystep;
		cb_draw();
	}
}

/*****************
 * Main function *
 *****************/

int main(int argc, char *argv[])
{
	/* parse command-line options */
	int c, is_full = 0;
	double yran, yc;
	fprintf(stderr, "\nKey bindings:\n\n");
	fprintf(stderr, "    LeftClick       zoom in by 2X\n");
	fprintf(stderr, "    RightClick      zoom out by 2X\n");
	fprintf(stderr, "    < or ,          decrease max iteration by 256\n");
	fprintf(stderr, "    > or .          increase max iteration by 256\n");
	fprintf(stderr, "    NUMBER          change max iteration to 256*NUMBER\n");
	fprintf(stderr, "    p or P          change to the next palette\n");
	fprintf(stderr, "    d or D          dump data to 'dataNNN.dat'\n");
	fprintf(stderr, "    s or S          save screenshot to 'screenNNN.ppm'\n");
	fprintf(stderr, "    ESC or q or Q   exit\n\n");
	fprintf(stderr, "Command line options:\n\n");
	fprintf(stderr, "    -i STR          hex string dumped by 'd' or 'D' [null]\n");
	fprintf(stderr, "    -m FILE         palette file                    [null]\n");
	fprintf(stderr, "    -w INT          image width in pixel            [800]\n");
	fprintf(stderr, "    -h INT          image height in pixel           [600]\n");
	fprintf(stderr, "    -f              full screen mode\n\n");
	while ((c = getopt(argc, argv, "fw:h:i:m:")) >= 0) {
		if (c == 'w') g_data.width  = (atoi(optarg) + 3) / 4 * 4;
		else if (c == 'h') g_data.height = (atoi(optarg) + 3) / 4 * 4;
		else if (c == 'f') is_full = 1;
		else if (c == 'i') {
			unsigned i, x;
			double f[4];
			unsigned char r[72], *p;
			strncpy((char*)r, optarg, 72);
			for (i = 0; i < 72; ++i)
				r[i] -= (r[i] >= '0' && r[i] <= '9')? '0' : (r[i] >= 'a' && r[i] <= 'f'? 'a' : 'A') - 10;
			for (i = 0, p = (unsigned char*)&x; i < 4; ++i) p[i] = r[i<<1]<<4 | r[i<<1|1];
			g_data.max_iter = x;
			for (i = 4, p = (unsigned char*)f; i < 36; ++i) p[i - 4] = r[i<<1]<<4 | r[i<<1|1];
			g_data.xmin = f[0]; g_data.xmax = f[1]; g_data.ymin = f[2]; g_data.ymax = f[3];
		} else if (c == 'm') {
			FILE *fp;
			int i, tmp[3];
			if ((fp = fopen(optarg, "r")) != NULL) {
				for (i = 0; i < 256; ++i) { /* FIXME: improve error checking */
					if (fscanf(fp, "%d%d%d", &tmp[0], &tmp[1], &tmp[2]) != 3) {
						fprintf(stderr, "(WW) Invalid palette at line %d\n", i + 1);
						break;
					}
					g_palette_custom[i].r = tmp[0]; g_palette_custom[i].g = tmp[1]; g_palette_custom[i].b = tmp[2];
				}
				fclose(fp);
				if (i == 256) g_data.palette[2] = g_palette_custom, g_data.pal_id = 2; /* default to the custom palette */
			} else fprintf(stderr, "(WW) Fail to read map file '%s'. Continue anyway.\n", optarg);
		}
	}
	g_data.buf = malloc(3 * g_data.width * g_data.height);

	/* Scale the Y axis to keep the right ratio */
	yran = (g_data.xmax - g_data.xmin) * g_data.height / g_data.width;
	yc = .5 * (g_data.ymax + g_data.ymin);
	g_data.ymax = yc + .5 * yran; g_data.ymin = yc - .5 * yran;

	/* Initialize OpenGL */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(g_data.width, g_data.height);
	if (is_full) {
		char mode[64];
		sprintf(mode, "%dx%d", g_data.width, g_data.height);
		glutGameModeString(mode);
		glutEnterGameMode();
	} else glutCreateWindow("Mandelbrot Viewer");

	glutDisplayFunc(cb_draw);
	glutKeyboardFunc(cb_key);
	glutMouseFunc(cb_mouse);
	glDisable(GL_DEPTH_TEST);
	
	glutMainLoop();
	return 0;
}

/**********************
 * Predefined palette *
 **********************/

static rgbcolor_t g_palette_neon[256] = {
	{  0,  0,  0},{  0,  0,  0},{  8,  0,  0},{ 16,  4,  4},{ 24,  4,  8},{ 32,  8, 12},{ 40, 12, 16},{ 48, 12, 20},
	{ 56, 16, 24},{ 64, 20, 28},{ 72, 20, 32},{ 80, 24, 36},{ 88, 28, 40},{ 96, 28, 44},{104, 32, 48},{112, 36, 52},
	{120, 36, 56},{128, 40, 60},{136, 40, 64},{144, 44, 68},{152, 48, 72},{160, 48, 76},{168, 52, 80},{176, 56, 84},
	{184, 56, 88},{192, 60, 92},{200, 64, 96},{208, 64,100},{216, 68,104},{224, 72,108},{232, 72,112},{240, 76,116},
	{252, 80,120},{248, 80,120},{240, 76,116},{232, 76,112},{224, 72,108},{216, 68,104},{208, 68,100},{200, 64, 96},
	{192, 60, 92},{184, 60, 88},{176, 56, 84},{168, 56, 80},{160, 52, 76},{152, 48, 72},{144, 48, 68},{136, 44, 64},
	{128, 40, 60},{120, 40, 60},{112, 36, 56},{104, 36, 52},{ 96, 32, 48},{ 88, 28, 44},{ 80, 28, 40},{ 72, 24, 36},
	{ 64, 20, 32},{ 56, 20, 28},{ 48, 16, 24},{ 40, 16, 20},{ 32, 12, 16},{ 24,  8, 12},{ 16,  8,  8},{  8,  4,  4},
	{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
	{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
	{  0,  0,  0},{  0,  4,  0},{  0, 12,  0},{  0, 20,  0},{  0, 28,  0},{  0, 36,  0},{  0, 44,  0},{  0, 52,  0},
	{  0, 60,  0},{  0, 68,  0},{  0, 76,  0},{  0, 84,  0},{  0, 92,  0},{  0,100,  0},{  0,108,  0},{  0,116,  0},
	{  0,124,  0},{  0,132,  0},{  0,140,  0},{  0,148,  0},{  0,156,  0},{  0,164,  0},{  0,172,  0},{  0,180,  0},
	{  0,188,  0},{  0,196,  0},{  0,204,  0},{  0,212,  0},{  0,220,  0},{  0,228,  0},{  0,236,  0},{  0,244,  0},
	{  0,252,  0},{  0,248,  0},{  0,240,  0},{  0,232,  0},{  0,224,  0},{  0,216,  0},{  0,208,  0},{  0,200,  0},
	{  0,192,  0},{  0,184,  0},{  0,176,  0},{  0,168,  0},{  0,160,  0},{  0,152,  0},{  0,144,  0},{  0,136,  0},
	{  0,128,  0},{  0,120,  0},{  0,112,  0},{  0,104,  0},{  0, 96,  0},{  0, 88,  0},{  0, 80,  0},{  0, 72,  0},
	{  0, 64,  0},{  0, 56,  0},{  0, 48,  0},{  0, 40,  0},{  0, 32,  0},{  0, 24,  0},{  0, 16,  0},{  0,  8,  0},
	{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
	{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
	{  0,  0,  0},{  4,  4,  0},{ 12, 12,  0},{ 20, 20,  0},{ 28, 28,  0},{ 36, 36,  0},{ 44, 44,  0},{ 52, 52,  0},
	{ 60, 60,  0},{ 68, 68,  0},{ 76, 76,  0},{ 84, 84,  0},{ 92, 92,  0},{100,100,  0},{108,108,  0},{116,116,  0},
	{124,124,  0},{132,132,  0},{140,140,  0},{148,148,  0},{156,156,  0},{164,164,  0},{172,172,  0},{180,180,  0},
	{188,188,  0},{196,196,  0},{204,204,  0},{212,212,  0},{220,220,  0},{228,228,  0},{236,236,  0},{244,244,  0},
	{252,252,  0},{248,248,  0},{240,240,  0},{232,232,  0},{224,224,  0},{216,216,  0},{208,208,  0},{200,200,  0},
	{192,192,  0},{184,184,  0},{176,176,  0},{168,168,  0},{160,160,  0},{152,152,  0},{144,144,  0},{136,136,  0},
	{128,128,  0},{120,120,  0},{112,112,  0},{104,104,  0},{ 96, 96,  0},{ 88, 88,  0},{ 80, 80,  0},{ 72, 72,  0},
	{ 64, 64,  0},{ 56, 56,  0},{ 48, 48,  0},{ 40, 40,  0},{ 32, 32,  0},{ 24, 24,  0},{ 16, 16,  0},{  8,  8,  0},
	{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
	{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
	{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
	{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0}
};

static rgbcolor_t g_palette_blues[256] = {
	{  0,  0,  0},{  0,  0,  0},{  0,  0,  4},{  0,  0, 12},{  0,  0, 16},{  0,  0, 24},{  0,  0, 32},{  0,  0, 36},
	{  0,  0, 44},{  0,  0, 48},{  0,  0, 56},{  0,  0, 64},{  0,  0, 68},{  0,  0, 76},{  0,  0, 80},{  0,  0, 88},
	{  0,  0, 96},{  0,  0,100},{  0,  0,108},{  0,  0,116},{  0,  0,120},{  0,  0,128},{  0,  0,132},{  0,  0,140},
	{  0,  0,148},{  0,  0,152},{  0,  0,160},{  0,  0,164},{  0,  0,172},{  0,  0,180},{  0,  0,184},{  0,  0,192},
	{  0,  0,200},{  0,  4,200},{  0, 12,200},{  0, 16,204},{  0, 24,204},{  0, 28,208},{  0, 36,208},{  0, 40,208},
	{  0, 48,212},{  0, 56,212},{  0, 60,216},{  0, 68,216},{  0, 72,216},{  0, 80,220},{  0, 84,220},{  0, 92,224},
	{  0,100,224},{  0,104,224},{  0,112,228},{  0,116,228},{  0,124,232},{  0,128,232},{  0,136,232},{  0,140,236},
	{  0,148,236},{  0,156,240},{  0,160,240},{  0,168,240},{  0,172,244},{  0,180,244},{  0,184,248},{  0,192,248},
	{  0,200,252},{  4,200,252},{ 12,200,252},{ 20,204,252},{ 28,204,252},{ 36,208,252},{ 44,208,252},{ 52,208,252},
	{ 60,212,252},{ 68,212,252},{ 76,216,252},{ 84,216,252},{ 92,216,252},{100,220,252},{108,220,252},{116,224,252},
	{124,224,252},{132,224,252},{140,228,252},{148,228,252},{156,232,252},{164,232,252},{172,232,252},{180,236,252},
	{188,236,252},{196,240,252},{204,240,252},{212,240,252},{220,244,252},{228,244,252},{236,248,252},{244,248,252},
	{252,252,252},{248,252,252},{244,252,252},{240,252,252},{232,252,252},{228,252,252},{224,252,252},{216,252,252},
	{212,252,252},{208,252,252},{200,252,252},{196,252,252},{192,252,252},{184,252,252},{180,252,252},{176,252,252},
	{168,252,252},{164,252,252},{160,252,252},{156,252,252},{148,252,252},{144,252,252},{140,252,252},{132,252,252},
	{128,252,252},{124,252,252},{116,252,252},{112,252,252},{108,252,252},{100,252,252},{ 96,252,252},{ 92,252,252},
	{ 84,252,252},{ 80,252,252},{ 76,252,252},{ 72,252,252},{ 64,252,252},{ 60,252,252},{ 56,252,252},{ 48,252,252},
	{ 44,252,252},{ 40,252,252},{ 32,252,252},{ 28,252,252},{ 24,252,252},{ 16,252,252},{ 12,252,252},{  8,252,252},
	{  0,252,252},{  0,248,252},{  0,244,252},{  0,240,252},{  0,232,252},{  0,228,252},{  0,224,252},{  0,216,252},
	{  0,212,252},{  0,208,252},{  0,200,252},{  0,196,252},{  0,192,252},{  0,184,252},{  0,180,252},{  0,176,252},
	{  0,168,252},{  0,164,252},{  0,160,252},{  0,156,252},{  0,148,252},{  0,144,252},{  0,140,252},{  0,132,252},
	{  0,128,252},{  0,124,252},{  0,116,252},{  0,112,252},{  0,108,252},{  0,100,252},{  0, 96,252},{  0, 92,252},
	{  0, 84,252},{  0, 80,252},{  0, 76,252},{  0, 72,252},{  0, 64,252},{  0, 60,252},{  0, 56,252},{  0, 48,252},
	{  0, 44,252},{  0, 40,252},{  0, 32,252},{  0, 28,252},{  0, 24,252},{  0, 16,252},{  0, 12,252},{  0,  8,252},
	{  0,  0,252},{  0,  0,248},{  0,  0,244},{  0,  0,240},{  0,  0,236},{  0,  0,232},{  0,  0,228},{  0,  0,224},
	{  0,  0,220},{  0,  0,216},{  0,  0,212},{  0,  0,208},{  0,  0,204},{  0,  0,200},{  0,  0,196},{  0,  0,192},
	{  0,  0,188},{  0,  0,184},{  0,  0,180},{  0,  0,176},{  0,  0,172},{  0,  0,168},{  0,  0,164},{  0,  0,160},
	{  0,  0,156},{  0,  0,152},{  0,  0,148},{  0,  0,144},{  0,  0,140},{  0,  0,136},{  0,  0,132},{  0,  0,128},
	{  0,  0,124},{  0,  0,120},{  0,  0,116},{  0,  0,112},{  0,  0,108},{  0,  0,104},{  0,  0,100},{  0,  0, 96},
	{  0,  0, 92},{  0,  0, 88},{  0,  0, 84},{  0,  0, 80},{  0,  0, 76},{  0,  0, 72},{  0,  0, 68},{  0,  0, 64},
	{  0,  0, 60},{  0,  0, 56},{  0,  0, 52},{  0,  0, 48},{  0,  0, 44},{  0,  0, 40},{  0,  0, 36},{  0,  0, 32},
	{  0,  0, 28},{  0,  0, 24},{  0,  0, 20},{  0,  0, 16},{  0,  0, 12},{  0,  0,  8},{  0,  0,  0},{  0,  0,  0}
};
