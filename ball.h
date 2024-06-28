#ifndef BALL_H
#define BALL_H

#include <math.h>

typedef struct {
	double x, y;
	int ix, iy;
	double alfa;
	double speed;
} TBall;

void putBall();
void moveBall(double x, double y);
void initBall();

#endif // BALL_H

