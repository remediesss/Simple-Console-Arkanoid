#ifndef BALL_H
#define BALL_H

#include <math.h>

// Declare the structure of the ball here if it's not declared in another common header
typedef struct {
	double x, y;
	int ix, iy;
	double alfa;
	double speed;
} TBall;

// Function declarations
void putBall();
void moveBall(double x, double y);
void initBall();

#endif // BALL_H

