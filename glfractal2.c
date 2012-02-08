/* This program was originally written by John Tsiombikas
   <nuclear@siggraph.org>. The source code is acquired here:

     http://nuclear.mutantstargoat.com/articles/sdr_fract/

   which is released to the public domain. I am re-releasing it under the MIT
   license. Please do acknowledge John if you are using the source code from
   this program. Thank you.
 */

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>

typedef struct {
	unsigned char r, g, b;
} rgbcolor_t;

static char *mbrot_src = "\n\
	uniform sampler1D tex;\n\
	uniform vec2 center;\n\
	uniform float scale;\n\
	uniform int iter;\n\
	void main() {\n\
		vec2 z, c;\n\
		c.x = 1.33333333 * (gl_TexCoord[0].x - 0.5) * scale - center.x;\n\
		c.y = (gl_TexCoord[0].y - 0.5) * scale - center.y;\n\
		int i;\n\
		z = c;\n\
		for(i=0; i<iter; i++) {\n\
			float x = (z.x * z.x - z.y * z.y) + c.x;\n\
			float y = (z.y * z.x + z.x * z.y) + c.y;\n\
			if((x * x + y * y) > 4.0) break;\n\
			z.x = x;\n\
			z.y = y;\n\
		}\n\
		gl_FragColor = texture1D(tex, (i == iter ? 0.0 : float(i)) / 100.0);\n\
	}\n";

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

GLhandleARB setup_shader(const char *src)
{
	GLhandleARB sdr, prog;
	sdr = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	glShaderSourceARB(sdr, 1, &src, 0);
	glCompileShaderARB(sdr);
	prog = glCreateProgramObjectARB();
	glAttachObjectARB(prog, sdr);
	glLinkProgramARB(prog);
	glUseProgramObjectARB(prog);
	return prog;
}

void set_uniform1f(GLhandleARB prog, const char *name, float val) {
	int loc = glGetUniformLocationARB(prog, name);
	if(loc != -1) {
		glUniform1f(loc, val);
	}
}

void set_uniform2f(GLhandleARB prog, const char *name, float v1, float v2) {
	int loc = glGetUniformLocationARB(prog, name);
	if(loc != -1) {
		glUniform2f(loc, v1, v2);
	}
}

void set_uniform1i(GLhandleARB prog, const char *name, int val) {
	int loc = glGetUniformLocationARB(prog, name);
	if(loc != -1) {
		glUniform1i(loc, val);
	}
}

void draw(void);
void key_handler(unsigned char key, int x, int y);
void bn_handler(int bn, int state, int x, int y);
void mouse_handler(int x, int y);

GLhandleARB prog;
float cx = 0.7, cy = 0.0;
float scale = 2.2;
int iter = 70;
const float zoom_factor = 0.025;

int main(int argc, char **argv)
{
	/* initialize glut */
	glutInitWindowSize(800, 600);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Mindlapse :: GLSL Mandelbrot");

	glutDisplayFunc(draw);
	glutIdleFunc(glutPostRedisplay);
	glutKeyboardFunc(key_handler);
	glutMouseFunc(bn_handler);
	glutMotionFunc(mouse_handler);

	/* load the 1D palette texture */
	glBindTexture(GL_TEXTURE_1D, 1);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	
	glTexImage1D(GL_TEXTURE_1D, 0, 3, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, g_palette_blues);

	glEnable(GL_TEXTURE_1D);

	/* load and set the mandelbrot shader */
	prog = setup_shader(mbrot_src);
	set_uniform1i(prog, "iter", iter);

	glutMainLoop();
	return 0;
}

void draw(void) {
	set_uniform2f(prog, "center", cx, cy);
	set_uniform1f(prog, "scale", scale);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glEnd();

	glutSwapBuffers();
}

void key_handler(unsigned char key, int x, int y) {
	switch(key) {
	case 27:
	case 'q':
	case 'Q':
		exit(0);

	case '=':
		if(1) {
			iter += 10;
		} else {
	case '-':
			iter -= 10;
			if(iter < 0) iter = 0;
		}
		printf("iterations: %d\n", iter);
		set_uniform1i(prog, "iter", iter);
		break;

	default:
		break;
	}
}

int which_bn;
float px, py;

void bn_handler(int bn, int state, int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	px = 2.0 * ((float)x / (float)xres - 0.5);
	py = 2.0 * ((float)y / (float)yres - 0.5);
	which_bn = bn;

	if(which_bn == 3) {
		scale *= 1 - zoom_factor * 2.0;
	} else if(which_bn == 4) {
		scale *= 1 + zoom_factor * 2.0;;
	}
}

void mouse_handler(int x, int y) {
	int xres = glutGet(GLUT_WINDOW_WIDTH);
	int yres = glutGet(GLUT_WINDOW_HEIGHT);
	float fx = 2.0 * ((float)x / (float)xres - 0.5);
	float fy = 2.0 * ((float)y / (float)yres - 0.5);

	if(which_bn == 1) {
		cx += (fx - px) * scale / 2.0;
		cy -= (fy - py) * scale / 2.0;
	} else if(which_bn == 0) {
		scale *= (fy - py < 0.0) ? 1 - zoom_factor : 1 + zoom_factor;
	}

	px = fx;
	py = fy;
}
