#include <conio.h>
#include <vector>
#include <graphics.h>
#include <dos.h>       // libs
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

int lenght = 2;
int lenghtmax = 2; // controls snake's lenght

int x1 = 30;
int y1 = 170; // snake's location

char direc = '-';
char last = '-'; // direction

bool playing = true; // if you are alive

int fruitx;
int fruity;
bool eaten = false; // fruit position

vector<int> snakex;
vector<int> snakey; // where your tail needs to go

int i = 1;
int imax = 0; // some basic for() things

char score[50];
int score1; // your score

void fruit();
void graphic();
void move();
void start();
void update(); // all voids

int main()
{
	start();


	while (playing == true)
	{
		update();

		if (x1 >= 340 || x1 < 0 || y1 >= 340 || y1 < 10) // check if you are inside the map
		{
			playing = false; // you die 😛
		}

		if (lenghtmax > 3 || direc != '-' && direc != 'w' && direc != 'a' && direc != 's' && direc != 'd' && direc != 'W' && direc != 'A' && direc != 'S' && direc != 'D')
		{
			for (lenght = lenghtmax; lenght >= 0; lenght--)
			{
				if (x1 == snakex[i] && y1 == snakey[i])
				{
					playing = false; // check if you head hits your tail
				}

				i--;
			}

			i = imax;
		}
	}

	if (playing == false)
	{
		cleardevice();
		outtextxy(70, 170, "YOU LOST, YOUR ");
		outtextxy(188, 170, score);
		getch();
		getch();
		return 0; // game over things
	}
}

void start() // execute at the start of the game
{
	sprintf(score, "SCORE : %d ", score1);

	srand(time(NULL));
	initwindow(350, 354, "SnakeGame");

	setcolor(GREEN);
	setfillstyle(SOLID_FILL, GREEN);
	rectangle(0, 0, 340, 10);
	floodfill(5, 5, GREEN);

	setcolor(WHITE);
	outtextxy(10, 0, score);

	fruitx = rand() % 34;
	fruity = rand() % 34;
	fruity = fruity * 10;
	fruitx = fruitx * 10;

	if(fruity == 0){
		fruity = fruity+10;
	}


	setcolor(RED);
	setfillstyle(SOLID_FILL, RED);
	rectangle(fruitx, fruity, fruitx + 10, fruity + 10);
	floodfill(fruitx + 5, fruity + 5, RED);
}

void update() // execute every frame
{
	sprintf(score, "SCORE : %d ", score1);

	delay(150);

	snakex.push_back(x1);
	snakey.push_back(y1); // makes your tail know the way to your head

	fruit();

	move();

	graphic();

}

void move() // all move related things
{

	if (kbhit())
	{
		direc = getch();
	}

	if (direc == 'w' && last == 's' || direc == 'W' && last == 'S')
	{
		direc = last;
	}

	if (direc == 'a' && last == 'd' || direc == 'A' && last == 'D')
	{
		direc = last;
	}

	if (direc == 's' && last == 'w' || direc == 'S' && last == 'W')
	{
		direc = last;
	}

	if (direc == 'd' && last == 'a' || direc == 'D' && last == 'A')
	{
		direc = last;
	}

	last = direc;

	if (direc == 's' || direc == 'S')
	{
		y1 = y1 + 10;
	}

	if (direc == 'a' || direc == 'A')
	{
		x1 = x1 - 10;
	}

	if (direc == 'w' || direc == 'W')
	{
		y1 = y1 - 10;
	}

	if (direc == 'd' || direc == 'D')
	{
		x1 = x1 + 10;
	}
}

void graphic() // the graphic
{
	cleardevice();

	setcolor(GREEN);
	setfillstyle(SOLID_FILL, GREEN);
	rectangle(0,0, 350, 10);			// green line
	floodfill(5, 5, GREEN);

	setcolor(WHITE);
	outtextxy(10, 0, score); // score

	setcolor(RED);
	setfillstyle(SOLID_FILL, RED);
	rectangle(fruitx, fruity, fruitx + 10, fruity + 10); // fruit
	floodfill(fruitx + 5, fruity + 5, RED);

	setcolor(WHITE);
	setfillstyle(SOLID_FILL, WHITE);
	rectangle(x1, y1, x1 + 10, y1 + 10); // head
	floodfill(x1 + 5, y1 + 5, WHITE);

	for (int lenght = lenghtmax; lenght > 0; lenght--)
	{
		setcolor(WHITE);
		setfillstyle(SOLID_FILL, WHITE);
		rectangle(snakex[i], snakey[i], snakex[i] + 10, snakey[i] + 10); // tail
		floodfill(snakex[i] + 5, snakey[i] + 5, WHITE);
		i--;
	}

	imax++;
	i = imax;

}

void fruit() // fruit things
{
	if (x1 == fruitx && y1 == fruity)
	{
		eaten = true;

	}

	if (eaten == true)
	{
		eaten = false;
		lenghtmax++;
		score1++;
		lenght = lenghtmax;
		fruitx = rand() % 34;
		fruity = rand() % 34;
		fruity = fruity * 10;
		fruitx = fruitx * 10; // random location

		for (lenght = lenghtmax; lenght >= 0; lenght--)
		{
			if (fruitx == snakex[i] && fruity == snakey[i])
			{
				fruitx = rand() % 34;
				fruity = rand() % 34;
				fruity = fruity * 10;
				fruitx = fruitx * 10; // don't spawn the fruit on the snake
			}

			i--;
		}

		if (fruity == 0)
		{
			fruity = 10;
		}

		i = imax;

		setcolor(RED);
		setfillstyle(SOLID_FILL, RED);
		rectangle(fruitx, fruity, fruitx + 10, fruity + 10); // makes fruit
		floodfill(fruitx + 5, fruitx + 5, RED);
	}
}
