#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
	#include <conio.h>
	#include <windows.h>
	
	#define SLEEP(x) Sleep(x)
#else		// I can't do this.
	#include <stdio.h>
	#include <unistd.h>
	
	#define SLEEP(x) usleep(1000*x)
#endif

#define MAXSCREENWIDTH	80
#define MAXSCREENHEIGHT	80
#define DEFAULTSCREENWIDTH	20
#define DEFAULTSCREENHEIGHT	20

#define ESC	0x1B
#define UP		72
#define DOWN	80
#define LEFT	75
#define RIGHT	77

#define LIM(x, y)	((y) <= 0 ? ((x) < 0 ? 0 : (x)) : ((x) > (y) ? (y) : (x)))

struct Snek{
	int x;
	int y;
	int length;
	char dir;
};

void setxy(int argc, char **argv, int *width, int *height);
void initBoard(char ***board, int width, int height, char set);
void printBoard(char **board, int height);
void placeFood(char **board, int width, int height);
void ggnore(char **board, int width, int height);

void initSnek
(char **board, char **underBoard,struct Snek *snek, int width, int height);
int moveSnek
(char **board, char **underBoard, struct Snek *snek, int width, int height);
void changeSnekDirection(struct Snek *snek, int character);

int main(int argc, char **argv){
	int i, j, kb = '0', width = 20, height = 20;
	char **board, **underBoard;
	struct Snek *snekHead;

	setxy(argc, argv, &width, &height);

	initBoard(&board, width, height, '.');
	initBoard(&underBoard, width, height, '$');
	initSnek(board, underBoard, snekHead, width, height);
	
	do{
		printBoard(board, height);
		if(kbhit())
			changeSnekDirection(snekHead, kb = getch());
		if(moveSnek(board, underBoard, snekHead, width, height)){
			ggnore(board, width, height);
			system("cls");
			printBoard(board, height);
			printf("\n%d,%d\n", snekHead->x, snekHead->y);
			break;
		}
		placeFood(board, width, height);

		/*putch(snekHead->dir);
		putch(snekHead->length);
		putch('\n');
		printBoard(underBoard, height);*/
		SLEEP(50);
		system("cls");
	} while(kb != ESC);
}

void setxy(int argc, char **argv, int *x, int *y){
	if(argc == 2)
		*y = atoi(*(argv+1));
	if(argc > 2){
		*x = atoi(*(argv+1));
		*y = atoi(*(argv+2));
	}
	
	if(*x <= 0 || *x > MAXSCREENWIDTH)
		*x = DEFAULTSCREENWIDTH;

	if(*y <= 0 || *y > MAXSCREENHEIGHT)
		*y = DEFAULTSCREENHEIGHT;
}

void initBoard(char ***board, int x, int y, char set){
	*board = (char **) malloc(y * sizeof(char *));
	
	int i, j;
	for(j = 0; j < y; j++){
		(*board)[j] = (char *) malloc((x+1) * sizeof(char));
		for(i = 0; i < x; i++)
			(*board)[j][i] = set;
		(*board)[j][x] = '\0';
	}
}

void printBoard(char **board, int y){
	int j;
	for(j = 0; j < y; j++){
		_cputs(*board++);
		putch('\n');
	}
}

int numGen(int num){
	if (num <= 0)
		return num ? -1 : 0;

	int random, limit = (RAND_MAX/num)*num;
	
	if(limit == 0)
		return 0;

	while((random = rand()) > limit)
		;
	return random % num;
}

void placeFood(char **board, int x, int y){
	int i, j;
	for(j = 0; j < y; j++)
		for(i = 0; i < x; i++)
			if(board[j][i] == 'o')
				return;
	
	while(board[j = numGen(y)][i = numGen(x)] == '#')
		;
	
	board[j][i] = 'o';
}

void ggnore(char **board, int x, int y){
	//exit(0);
	if(x > 8){
		int i;
		char *text = "GAME OVER";
		x>>=1;
		y>>=1;
		for(i = 0; *text != '\0'; i++)
			board[y][x+i-4] = *text++;
	}
}

void initSnek(char **board, char **underBoard, struct Snek *snek, int x, int y){
	snek->x = x>>1;
	snek->y = y>>1;
	snek->length = '1';
	snek->dir = 'U';
	
	board[snek->y][snek->x] = '#';
	underBoard[snek->y][snek->x] = snek->length;
}

void growSnek(char **board, char **underBoard, struct Snek *snek, int x, int y){
	(snek->length)++;
	int i, j;
	for(j = 0; j < y; j++)
		for(i = 0; i < x; i++)
			if(underBoard[j][i] > '0')
				underBoard[j][i]++;
}

void shrinkSnek
(char **board, char **underBoard, struct Snek *snek, int x, int y){
	int i, j;
	for(j = 0; j < y; j++)
		for(i = 0; i < x; i++){
			if(underBoard[j][i] > '0')
				underBoard[j][i]--;
			
			if(board[j][i] == '#' && underBoard[j][i] < '1')
				board[j][i] = '.';
		}
}

int moveSnek(char **board, char **underBoard, struct Snek *snek, int x, int y){
	shrinkSnek(board, underBoard, snek, x, y);

	if(	board[LIM((snek->y)-1, 0)][snek->x] == 'o' && snek->dir == 'U' ||
		board[snek->y][LIM((snek->x)-1, 0)] == 'o' && snek->dir == 'L' ||
		board[LIM((snek->y)+1, y-1)][snek->x] == 'o' && snek->dir == 'D' ||
		board[snek->y][LIM((snek->x)+1, x)] == 'o' && snek->dir == 'R'
	)
		growSnek(board, underBoard, snek, x, y);
	
	switch(snek->dir){
		case 'U':
			board[--(snek->y)][snek->x] = '#';
			break;
		
		case 'D':
			board[++(snek->y)][snek->x] = '#';
			break;
		
		case 'L':
			board[snek->y][--(snek->x)] = '#';
			break;
		
		case 'R':
			board[snek->y][++(snek->x)] = '#';
			break;
	}
	
	underBoard[snek->y][snek->x] = snek->length;
	
	/*if(	board[LIM((snek->y)-1, 0)][snek->x] == '#' && snek->dir == 'U' ||
		board[snek->y][LIM((snek->x)-1, 0)] == '#' && snek->dir == 'L' ||
		board[LIM((snek->y)+1, y-1)][snek->x] == '#' && snek->dir == 'D' ||
		board[snek->y][LIM((snek->x)+1, x)] == '#' && snek->dir == 'R'
	)*/

	if(	snek->y == 0 && snek->dir == 'U' ||
		snek->x == 0 && snek->dir == 'L' ||
		snek->y == y && snek->dir == 'D' ||
		snek->x == x && snek->dir == 'R'
	)
		return -1;
	
	if(	board[snek->y-1][snek->x] == '#' && snek->dir == 'U' ||
		board[snek->y][snek->x-1] == '#' && snek->dir == 'L' ||
		board[snek->y+1][snek->x] == '#' && snek->dir == 'D' ||
		board[snek->y][snek->x-1] == '#' && snek->dir == 'R'
	)
		return -1;
	return 0;
}

void changeSnekDirection(struct Snek *snek, int c){
	switch(c){
		case 'W': case 'w':
			if(snek->dir == 'L' || snek->dir == 'R')
				snek->dir = 'U';
			break;

		case 'S': case 's':
			if(snek->dir == 'L' || snek->dir == 'R')
			snek->dir = 'D';
			break;

		case 'A': case 'a':
			if(snek->dir == 'U' || snek->dir == 'D')
				snek->dir = 'L';
			break;
		
		case 'D': case 'd':
			if(snek->dir == 'U' || snek->dir == 'D')
				snek->dir = 'R';
			break;
	}
	
	if(c==224)
		switch(c=getch()){
			case UP:
				if(snek->dir == 'L' || snek->dir == 'R')
					snek->dir = 'U';
				break;

			case DOWN:
				if(snek->dir == 'L' || snek->dir == 'R')
				snek->dir = 'D';
				break;

			case LEFT:
				if(snek->dir == 'U' || snek->dir == 'D')
					snek->dir = 'L';
				break;
			
			case RIGHT:
				if(snek->dir == 'U' || snek->dir == 'D')
					snek->dir = 'R';
				break;
		}
}