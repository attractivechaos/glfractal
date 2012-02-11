/* This program was originally written by John Tsiombikas
   <nuclear@siggraph.org>. The source code is acquired here:

     http://nuclear.mutantstargoat.com/articles/sdr_fract/

   which is released to the public domain. I am re-releasing it under the MIT
   license. Please consider to acknowledge John if you are using the source
   code from this program. Thank you.

   The precision emulation code is from this blog post:

     http://www.thasler.org/blog/?p=93
 */

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>

typedef struct {
	unsigned char r, g, b;
} rgbcolor_t;

static rgbcolor_t g_palette_neon[256];

static struct {
	int size[2], max_iter, action, bn, pos[2];
	double center[2], scale, zoom;
	GLhandleARB prog;
	rgbcolor_t *palette;
} gd = { {800, 600}, 256, 0, 1, {0, 0}, {-0.7, 0}, 2.4, 0.1, NULL, g_palette_neon };

/*********************
 * Mandelbrot shader *
 *********************/

static const char *mb_src = "\n\
	uniform sampler1D tex;\n\
	uniform vec2 centerx, centery;\n\
	uniform float ratio, scale;\n\
	uniform int max_iter;\n\
	vec2 ds_add(vec2 a, vec2 b) {\n\
		vec2 c;\n\
		float t1, t2, e;\n\
		t1 = a.x + b.x;\n\
		e = t1 - a.x;\n\
		t2 = ((b.x - e) + (a.x - (t1 - e))) + a.y + b.y;\n\
		c.x = t1 + t2;\n\
		c.y = t2 - (c.x - t1);\n\
		return c;\n\
	}\n\
	vec2 ds_sub(vec2 a, vec2 b) {\n\
		vec2 c;\n\
		float e, t1, t2;\n\
		t1 = a.x - b.x;\n\
		e = t1 - a.x;\n\
		t2 = ((-b.x - e) + (a.x - (t1 - e))) + a.y - b.y;\n\
		c.x = t1 + t2;\n\
		c.y = t2 - (c.x - t1);\n\
		return c;\n\
	}\n\
	vec2 ds_mul(vec2 a, vec2 b) {\n\
		vec2 c;\n\
		float c11, c21, c2, e, t1, t2;\n\
		float a1, a2, b1, b2, cona, conb, split = 8193.;\n\
		cona = a.x * split;\n\
		conb = b.x * split;\n\
		a1 = cona - (cona - a.x);\n\
		b1 = conb - (conb - b.x);\n\
		a2 = a.x - a1;\n\
		b2 = b.x - b1;\n\
		c11 = a.x * b.x;\n\
		c21 = a2 * b2 + (a2 * b1 + (a1 * b2 + (a1 * b1 - c11)));\n\
		c2 = a.x * b.y + a.y * b.x;\n\
		t1 = c11 + c2;\n\
		e = t1 - c11;\n\
		t2 = a.y * b.y + ((c2 - e) + (c11 - (t1 - e))) + c21;\n\
		c.x = t1 + t2;\n\
		c.y = t2 - (c.x - t1);\n\
		return c;\n\
	}\n\
	void main(void) {\n\
		int i;\n\
		vec2 zx, zy, cx, cy;\n\
		cx = vec2(ratio * (gl_TexCoord[0].x - 0.5) * scale, 0) + centerx;\n\
		cy = vec2((gl_TexCoord[0].y - 0.5) * scale, 0) + centery;\n\
		for (i = 0, zx = cx, zy = cy; i < max_iter; ++i) {\n\
			vec2 tmp = ds_mul(zx, zy);\n\
			vec2 ax = ds_add(ds_sub(ds_mul(zx, zx), ds_mul(zy, zy)), cx);\n\
			vec2 ay = ds_add(ds_add(tmp, tmp), cy);\n\
			tmp = ds_add(ds_mul(ax, ax), ds_mul(ay, ay));\n\
			if (tmp.x > 4.0) break;\n\
			zx = ax; zy = ay;\n\
		}\n\
		gl_FragColor = texture1D(tex, float(i&0xff) / 256.0);\n\
	}\n";

/********************
 * OpenGL callbacks *
 ********************/

static void cb_draw(void)
{
	glUniform2f(glGetUniformLocationARB(gd.prog, "centerx"), gd.center[0], gd.center[0] - (float)gd.center[0]);
	glUniform2f(glGetUniformLocationARB(gd.prog, "centery"), gd.center[1], gd.center[1] - (float)gd.center[1]);
	glUniform1f(glGetUniformLocationARB(gd.prog, "scale"), gd.scale);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-1, -1);
	glTexCoord2f(1, 0); glVertex2f(1, -1);
	glTexCoord2f(1, 1); glVertex2f(1, 1);
	glTexCoord2f(0, 1); glVertex2f(-1, 1);
	glEnd();
	glutSwapBuffers();
}

static void cb_mouse(int bn, int state, int x, int y)
{
	if (bn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		gd.bn = bn;
		gd.pos[0] = x;
		gd.pos[1] = y;
		gd.action = 1;
		glutPostRedisplay();
	}
}

static void cb_motion(int x, int y)
{
	if (gd.bn == GLUT_LEFT_BUTTON) {
		gd.center[0] -= (double)(x - gd.pos[0]) / gd.size[0] * gd.scale;
		gd.center[1] -= (double)(gd.pos[1] - y) / gd.size[1] * gd.scale;
		gd.pos[0] = x; gd.pos[1] = y;
		glutPostRedisplay();
	}
}

static void cb_keyboard(unsigned char key, int x, int y)
{
	if (key == 27 || key == 'q' || key == 'Q') {
		exit(0);
	} else if (key == '=' || key == '+' || key == '-') {
		double px = ((double)x / gd.size[0] - .5) * gd.scale * ((double)gd.size[0] / gd.size[1]) + gd.center[0];
		double py = (.5 - (double)y / gd.size[1]) * gd.scale + gd.center[1];
		if (key == '-') {
			gd.center[0] -= (px - gd.center[0]) * gd.zoom;
			gd.center[1] -= (py - gd.center[1]) * gd.zoom;
			gd.scale *= 1 + gd.zoom;
		} else {
			gd.center[0] += (px - gd.center[0]) * gd.zoom;
			gd.center[1] += (py - gd.center[1]) * gd.zoom;
			gd.scale *= 1 - gd.zoom;
		}
		glutPostRedisplay();
		gd.action = 0;
	} else if (key >= '0' && key <= '9') {
		gd.max_iter = (key - '0') << 8;
		glUniform1i(glGetUniformLocationARB(gd.prog, "max_iter"), gd.max_iter);
		glutPostRedisplay();
		gd.action = 0;
	}
}

/*****************
 * Main function *
 *****************/

int main(int argc, char **argv)
{
	GLhandleARB sdr;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(gd.size[0], gd.size[1]);
	glutCreateWindow("GLSL Mandelbrot");

	glutDisplayFunc(cb_draw);
	glutKeyboardFunc(cb_keyboard);
	glutMouseFunc(cb_mouse);
	glutMotionFunc(cb_motion);

	glBindTexture(GL_TEXTURE_1D, 1);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexImage1D(GL_TEXTURE_1D, 0, 3, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, gd.palette);
	glEnable(GL_TEXTURE_1D);

	sdr = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	glShaderSourceARB(sdr, 1, &mb_src, 0);
	glCompileShaderARB(sdr);
	gd.prog = glCreateProgramObjectARB();
	glAttachObjectARB(gd.prog, sdr);
	glLinkProgramARB(gd.prog);
	glUseProgramObjectARB(gd.prog);

	glUniform1i(glGetUniformLocationARB(gd.prog, "max_iter"), gd.max_iter);
	glUniform1f(glGetUniformLocationARB(gd.prog, "ratio"), (double)gd.size[0] / gd.size[1]);

	glutMainLoop();
	return 0;
}

/****************
 * Data section *
 ****************/

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
