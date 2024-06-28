#include "ball.h"

extern char mas[][200 + 1];
extern TBall ball;

void putBall() {
	mas[ball.iy][ball.ix] = '*';
}

void moveBall(double x, double y) {
	ball.x = x;
	ball.y = y;
	ball.ix = (int)round(ball.x);
	ball.iy = (int)round(ball.y);
}

void initBall() {
	moveBall(2, 2);
	ball.alfa = -1;
	ball.speed = 0.5;
}
