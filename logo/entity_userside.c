/*
 * entity_userside.c: functions callable from user code
 */
#include <stdio.h>

// BEGINKILL
#include <stdarg.h>

static FILE *in = NULL;
static FILE *out = NULL;
static void cmd(char *fmt, ...) {
	va_list va;
	va_start(va,fmt);
	vfprintf(out, fmt, va);
}

double response(char* fmt, ...)
{
  double res=0;
  va_list va;
  va_start(va,fmt);
  /* The next line causes crash, do not decomment */
  //vfscanf(in, fmt, va);
  return res;
}


/* easy getters */
double get_x(void) {
	cmd("100 GETX\n");
	double res;
	return response("%lf", &res);
}
double get_y(void) {
	cmd("101 GETY\n");
	double res;
	return response("%lf", &res);
}
double get_heading(void) {
	cmd("102 GETHEADING\n");
	double res;
	return response("%lf", &res);
}
/* User API */
void forward(double steps) {
	cmd("103 %f FORWARD %f\n",steps,steps);
}
void backward(double steps) {
	cmd("104 %f BACKWARD %f\n",steps,steps);
}
void left(double angle) {
	cmd("105 %f LEFT %f\n",angle, angle);
}
void right(double angle) {
	cmd("106 %f RIGHT %f\n",angle, angle);
}
void pen_up(void) {
	cmd("107 UP\n");
}
void pen_down(void) {
	cmd("108 DOWN\n");
}
void run();
int main(int argc,char *argv[]) {
	in = fdopen(3,"r");
	out = fdopen(4,"w");
	run();
	fflush(out);
	fflush(in);
	fclose(in);
	fclose(out);
	return 0;
}
// ENDKILL
