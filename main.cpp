#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "ball.h"

#define width 200
#define height 60
#define num_blocks 20
#define num_rows 6

typedef struct {
	int x, y;
	bool exist;
} Tblock;

typedef struct {
	int x, y;
	int w; // ширина ракетки
} Tracket;


typedef struct {
	char nickname[100];
	int score;
} HighScore;

char mas[height][width + 1];
Tracket racket;
TBall ball;
Tblock blocks[num_blocks];
int hitCnt;
int maxHitCnt;

void autoMoveBall() {
	if (ball.alfa < 0) ball.alfa += M_PI * 2;
	if (ball.alfa > M_PI * 2) ball.alfa -= M_PI * 2;

	TBall bl = ball;
	float deltaX, deltaY;
	
	/*Эта вставка использует инструкции FPU для вычисления синуса и косинуса угла alfa, а затем умножает результаты на скорость мяча,
    чтобы получить изменения по осям X и Y.После выполнения ассемблерного кода, новые значения deltaX и deltaY используются для обновления позиции мяча.*/
	__asm {
		fld ball.alfa       // Загружаем угол alfa
		fsincos             // Вычисляем sin(alfa) и cos(alfa)
							// ST(0) = cos(alfa), ST(1) = sin(alfa)
		fmul ball.speed     // Умножаем cos(alfa) на speed
		fstp deltaX         // Сохраняем результат в deltaX и выталкиваем из стека
		fmul ball.speed     // Умножаем sin(alfa) на speed (который теперь находится на вершине стека)
		fstp deltaY         // Сохраняем результат в deltaY
	}
	moveBall(ball.x + deltaX, ball.y - deltaY);

	// Увеличение скорости мяча каждые 5 попаданий
	if (hitCnt % 10 == 0 && hitCnt != 0) {
		ball.speed += 0.01;
	}

	if ((mas[ball.iy][ball.ix] == '#') || (mas[ball.iy][ball.ix] == '@')) {
		if ((ball.ix != bl.ix) && (ball.iy != bl.iy)) { // сравнение нового положения со старым bl - старое
			if (mas[bl.iy][bl.ix] == mas[ball.iy][ball.ix]) {
				bl.alfa = bl.alfa + M_PI; // инвертируем направление
			}
			else {
				if (mas[bl.iy][ball.ix] == '#') { // если старый у и новый х - стена, то вертикальная поверхность
					bl.alfa = (2 * M_PI - bl.alfa) + M_PI;
				}
				else {
					bl.alfa = (2 * M_PI - bl.alfa);
				}
			}
		}
		else if (ball.iy == bl.iy) {
			bl.alfa = (2 * M_PI - bl.alfa) + M_PI; // отражение по вертикали
		}
		else {
			bl.alfa = (2 * M_PI - bl.alfa); // отражение по горизонтали
		}
		ball = bl;

	}
	else if (mas[ball.iy][ball.ix] == '=') {
		bl.alfa = (2 * M_PI - bl.alfa);
		for (int row = 0; row < num_rows; ++row) {
			for (int i = 0; i < num_blocks; ++i) {
				if (blocks[row * num_blocks + i].exist) {
					// Проверка столкновения мячика с блоком
					if (ball.iy == blocks[row * num_blocks + i].y && ball.ix >= blocks[row * num_blocks + i].x && ball.ix < blocks[row * num_blocks + i].x + 6) {
						blocks[row * num_blocks + i].exist = false;
						hitCnt++;
					}
				}
			}
		}
		ball = bl;

	}
}

void initRacket() {
	racket.w = 7;
	racket.x = (width - racket.w) / 2;
	racket.y = height - 1;
}

void putRacket() {
	for (int i = racket.x; i < racket.x + racket.w; ++i) {
		mas[racket.y][i] = '@';
	}
}

void initBlocks() {
	int gap_x = width / (num_blocks + 1);
	int gap_y = 3;
	for (int row = 0; row < num_rows; ++row) {
		for (int i = 0; i < num_blocks; ++i) {
			blocks[row * num_blocks + i].x = (i + 1) * gap_x;
			blocks[row * num_blocks + i].y = (row + 1) * gap_y;
			blocks[row * num_blocks + i].exist = true;
		}
	}
}

void putBlocks() {
	for (int row = 0; row < num_rows; ++row) {
		for (int i = 0; i < num_blocks; ++i) {
			if (blocks[row * num_blocks + i].exist) {
				// Проверка, чтобы блок не выходил за верхнюю границу
				if (blocks[row * num_blocks + i].y >= 1) {
					for (int j = blocks[row * num_blocks + i].x; j < blocks[row * num_blocks + i].x + 6; ++j) {
						// Проверка, чтобы блок не выходил за боковые границы
						if (j >= 1 && j < width - 1) {
							mas[blocks[row * num_blocks + i].y][j] = '=';
						}
					}
				}
			}
		}
	}
}

void init() {
	for (int i = 0; i < width; ++i)
	{
		mas[0][i] = '#';
	}

	mas[0][width] = '\0';

	strncpy(mas[1], mas[0], width + 1);

	for (int i = 1; i < width - 1; ++i)
	{
		mas[1][i] = ' ';
	}

	for (int i = 1; i < height; ++i)
	{
		strncpy(mas[i], mas[1], width + 1);
	}

	putBlocks();
}

void show() {
	for (int i = 0; i < height; ++i)
	{
		printf("%s", mas[i]);

		if (i == 3)
		{
			printf("      SCORE %i     ", hitCnt);
		}
		if (i == 4)
		{
			printf("      HISCORE %i     ", maxHitCnt);
		}
		if (i < height - 1)
		{
			printf("\n");
		}

	}
}

void moveRacket(int x)
{
	racket.x = x;
	if (racket.x < 1)
	{
		racket.x = 1;
	}
	if (racket.x + racket.w >= width)
	{
		racket.x = width - 1 - racket.w;
	}
}

void setCur(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void greeting()
{
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\t\t\t\t\t\t\t\t  ¶¶¶¶   ¶¶¶¶  ¶¶  ¶¶  ¶¶¶¶   ¶¶¶¶  ¶¶     ¶¶¶¶¶     ¶¶¶¶  ¶¶¶¶¶   ¶¶¶¶   ¶¶¶¶  ¶¶  ¶¶  ¶¶¶¶  ¶¶¶¶¶¶ ¶¶¶¶¶ \t\t\t\n");
	printf("\t\t\t\t\t\t\t\t ¶¶  ¶¶ ¶¶  ¶¶ ¶¶¶ ¶¶ ¶¶     ¶¶  ¶¶ ¶¶     ¶¶       ¶¶  ¶¶ ¶¶  ¶¶ ¶¶  ¶¶ ¶¶  ¶¶ ¶¶¶ ¶¶ ¶¶  ¶¶   ¶¶   ¶¶  ¶¶\t\t\t\n");
	printf("\t\t\t\t\t\t\t\t ¶¶     ¶¶  ¶¶ ¶¶ ¶¶¶  ¶¶¶¶  ¶¶  ¶¶ ¶¶     ¶¶¶¶     ¶¶¶¶¶¶ ¶¶¶¶¶  ¶¶     ¶¶¶¶¶¶ ¶¶ ¶¶¶ ¶¶  ¶¶   ¶¶   ¶¶  ¶¶\t\t\t\n");
	printf("\t\t\t\t\t\t\t\t ¶¶  ¶¶ ¶¶  ¶¶ ¶¶  ¶¶     ¶¶ ¶¶  ¶¶ ¶¶     ¶¶       ¶¶  ¶¶ ¶¶  ¶¶ ¶¶  ¶¶ ¶¶  ¶¶ ¶¶  ¶¶ ¶¶  ¶¶   ¶¶   ¶¶  ¶¶\t\t\t\n");
	printf("\t\t\t\t\t\t\t\t  ¶¶¶¶   ¶¶¶¶  ¶¶  ¶¶  ¶¶¶¶   ¶¶¶¶  ¶¶¶¶¶¶ ¶¶¶¶¶    ¶¶  ¶¶ ¶¶  ¶¶  ¶¶¶¶  ¶¶  ¶¶ ¶¶  ¶¶  ¶¶¶¶  ¶¶¶¶¶¶ ¶¶¶¶¶ \t\t\t\n");
}

int compareScores(const void* a, const void* b) {
	HighScore* scoreA = (HighScore*)a;
	HighScore* scoreB = (HighScore*)b;
	return scoreB->score - scoreA->score; // Сортировка по убыванию
}

void displayHighScores() {
	FILE* file = fopen("high_scores.txt", "r");
	HighScore scores[1000]; // Предполагаем, что у нас не более 1000 записей
	int count = 0;

	if (file) {
		while (fscanf(file, "%99s %d", scores[count].nickname, &scores[count].score) == 2) {
			++count;
		}
		fclose(file);

		qsort(scores, count, sizeof(HighScore), compareScores);

		system("cls");

		printf("\n===================\n");
		printf("Top 10 High Scores:\n");
		printf("===================\n");

		for (int i = 0; i < 10 && i < count; ++i) {
			printf("%s - %d\n", scores[i].nickname, scores[i].score);
		}
	}
	else {
		printf("\nNo high scores available.\n");
	}
}

void updateHighScores(const char* nickname, int score) {
	FILE* file = fopen("high_scores.txt", "a");
	if (file) {
		fprintf(file, "%s %d\n", nickname, score);
		fclose(file);
	}
}

int main() {
	int lives = 3;
	greeting();
	Sleep(2500);

	system("cls");

	printf("\nshow high scores : press 0");
	printf("\nstart game : press 1\n\n");
	char temp = _getch();

	if (temp == '1') {
		system("cls");
		char nickname[50];
		printf("\n\n\n\n\n\n\n\nWelcome to the ARKANOID game!\n");
		printf("Enter your nickname: ");
		scanf("%s", nickname);


		bool run = false;
		initRacket();
		initBall();
		initBlocks();

		hitCnt = 0;
		maxHitCnt = 0;

		do {
			setCur(0, 0);
			if (run)
			{
				autoMoveBall();
			}
			if (ball.iy > height)
			{
				lives--; // Уменьшаем количество жизней
				if (lives > 0) {
					run = false; // Останавливаем мячик
					moveBall(racket.x + racket.w / 2, racket.y - 1); // Позиционируем мячик по центру ракетки
					
				}
				else {
					printf("Game Over!\n");
					break; // Выходим из цикла, если жизни закончились
				}
				hitCnt = 0;
			}
			else if (hitCnt > maxHitCnt)
			{
				maxHitCnt = hitCnt; // Обновляем maxHitCnt только если hitCnt больше
			}

			init();
			putRacket();
			putBall();
			show();


			if (GetKeyState('A') < 0) moveRacket(racket.x - 1);
			if (GetKeyState('D') < 0) moveRacket(racket.x + 1);
			if (GetKeyState('W') < 0) run = true;
			if (!run)
			{
				moveBall(racket.x + racket.w / 2, racket.y - 1);
			}
			Sleep(5);
		} while (GetAsyncKeyState(VK_ESCAPE) >= 0);

		updateHighScores(nickname, maxHitCnt);
	}

	if (temp == '0') {
		displayHighScores();
	}

	return 0;
}
