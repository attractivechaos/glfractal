/*
	gcc -Wall -O2 glfractal.c -framework GLUT -framework OpenGL -Wall -o glfractal
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

static struct {
	int width, height, max_iter, tmp_iter;
	double xmin, xmax, ymin, ymax;
	unsigned char *buf;
} g_data = { 800, 600, 256, 0, -2, 1, -1.2, 1.2, NULL };

typedef struct {
	unsigned char r, g, b;
} rgbcolor_t;

/* Palette: blues.map from FractInt */
const rgbcolor_t g_palette[256] = {
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

/********************
 * Mandelbrot plane *
 ********************/

void glf_mbrot_simple(unsigned char *buf, int max_iter, int w, int h, double xmin, double xmax, double ymin, double ymax, const rgbcolor_t palette[256])
{
	int i, j, k;
	double w1, h1, x0, y0;
	rgbcolor_t *p = (rgbcolor_t*)buf;
	w1 = 1. / w; h1 = 1. / h;
	for (j = 0, y0 = ymin; j < h; ++j) {
		y0 = ymin + (ymax - ymin) * j * h1;
		for (i = 0, x0 = xmin; i < w; ++i) {
			double x = x0 = xmin + (xmax - xmin) * i * w1, y = y0;
			for (k = 0; k < max_iter; ++k) {
				double x2 = x * x, y2 = y * y;
				if (x2 + y2 >= 4) break;
				y = 2 * x * y + y0;
				x = x2 - y2 + x0;
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
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glf_mbrot_simple(g_data.buf, g_data.max_iter, g_data.width, g_data.height, g_data.xmin, g_data.xmax, g_data.ymin, g_data.ymax, g_palette);
	glDrawPixels(g_data.width, g_data.height, GL_RGB, GL_UNSIGNED_BYTE, g_data.buf);
	glutSwapBuffers();
}

static void cb_key(unsigned char key, int x, int y)
{
	if (key == 27 || key == 'q' || key == 'Q') {
		free(g_data.buf);
		exit(0);
	} else if (isdigit(key)) {
		if (g_data.tmp_iter < 100)
			g_data.tmp_iter = g_data.tmp_iter * 10 + (key - '0');
	} else if (key == 13) {
		g_data.max_iter = g_data.tmp_iter * 256;
		g_data.tmp_iter = 0;
		fprintf(stderr, "Max iteration: %d\n", g_data.max_iter);
		cb_draw();
	}
}

static void cb_special(int key, int x, int y)
{
	if (key == GLUT_KEY_RIGHT) {
		g_data.max_iter += 256;
		cb_draw();
		fprintf(stderr, "Max iteration: %d\n", g_data.max_iter);
	} else if (key == GLUT_KEY_LEFT) {
		if (g_data.max_iter >= 512) g_data.max_iter -= 256;
		cb_draw();
		fprintf(stderr, "Max iteration: %d\n", g_data.max_iter);
	}
}

static void cb_mouse(int bn, int state, int x, int y)
{
	if (state == 1 && (bn == 0 || bn == 2)) { /* release of the left/right button */
		double xc = g_data.xmin + (g_data.xmax - g_data.xmin) * x / g_data.width;
		double yc = g_data.ymin + (g_data.ymax - g_data.ymin) * (g_data.height - y) / g_data.height;
		double xstep = (g_data.xmax - g_data.xmin) * (bn == 0? .25 : 4.);
		double ystep = (g_data.ymax - g_data.ymin) * (bn == 0? .25 : 4.);
		g_data.xmin = xc - xstep, g_data.xmax = xc + xstep;
		g_data.ymin = yc - ystep, g_data.ymax = yc + ystep;
		cb_draw();
		fprintf(stderr, "Window: (%g,%g;%g,%g)\n", g_data.xmin, g_data.xmax, g_data.ymin, g_data.ymax);
	}
}

/*****************
 * Main function *
 *****************/

int main(int argc, char *argv[])
{
	/* parse command-line options */
	int c;
	fprintf(stderr, "\nKey bindings:\n\n");
	fprintf(stderr, "    LeftClick       zoom in by 2X\n");
	fprintf(stderr, "    RightClick      zoom out by 2X\n");
	fprintf(stderr, "    LEFT            decrease max iteration by 256\n");
	fprintf(stderr, "    RIGHT           increase max iteration by 256\n");
	fprintf(stderr, "    Number+Return   change max iteration to i*256\n");
	fprintf(stderr, "    ESCAPE/q/Q      exit\n");
	fprintf(stderr, "\n");
	while ((c = getopt(argc, argv, "w:h:")) >= 0) {
		if (c == 'w') g_data.width  = (atoi(optarg) + 3) / 4 * 4;
		else if (c == 'h') g_data.height = (atoi(optarg) + 3) / 4 * 4;
	}
	g_data.buf = calloc(1, 3 * g_data.width * g_data.height);

	/* Initialize OpenGL */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(g_data.width, g_data.height);
	glutCreateWindow("Mandelbrot Viewer");

	glutDisplayFunc(cb_draw);
	glutSpecialFunc(cb_special);
	glutKeyboardFunc(cb_key);
	glutMouseFunc(cb_mouse);
	glDisable(GL_DEPTH_TEST);
	
	glutMainLoop();
	return 0;
}
