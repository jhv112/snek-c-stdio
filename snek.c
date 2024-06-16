#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define MAXSCREENWIDTH  80
#define MAXSCREENHEIGHT 80
#define DEFAULTSCREENWIDTH  20
#define DEFAULTSCREENHEIGHT 20
#define REFRESHRATE 50

#define ESC 0x1B
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

struct Snek {
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

void initSnek(
    char **board,
    char **underBoard,
    struct Snek *snek,
    int width,
    int height
);

int moveSnek(
    char **board,
    char **underBoard,
    struct Snek *snek,
    int width,
    int height
);

void changeSnekDirection(struct Snek *snek, int input);

int main(int argc, char **argv){
    int i, j, kb = '0', width = 20, height = 20;
    char **board, **underBoard;
    struct Snek *snekHead;

    setxy(argc, argv, &width, &height);

    initBoard(&board, width, height, '.');
    initBoard(&underBoard, width, height, '$');
    initSnek(board, underBoard, snekHead, width, height);

    do {
        printBoard(board, height);

        if(kbhit())
            changeSnekDirection(snekHead, kb = getch());

        if(moveSnek(board, underBoard, snekHead, width, height))
            break;

        placeFood(board, width, height);
        Sleep(REFRESHRATE);
        system("cls");
    } while(kb != ESC);

    ggnore(board, width, height);
    system("cls");
    printBoard(board, height);
}

void setxy(int argc, char **argv, int *x, int *y) {
    if(argc == 2)
        *y = atoi(*(argv+1));

    if(argc > 2) {
        *x = atoi(*(argv+1));
        *y = atoi(*(argv+2));
    }

    if(*x < 2 || *x > MAXSCREENWIDTH)
        *x = DEFAULTSCREENWIDTH;

    if(*y < 2 || *y > MAXSCREENHEIGHT)
        *y = DEFAULTSCREENHEIGHT;
}

void initBoard(char ***board, int x, int y, char set) {
    *board = (char **) malloc((y+3) * sizeof(char *));

    int i, j;
    for(j = 0; j < y+3; j++){
        (*board)[j] = (char *) malloc((x+3) * sizeof(char));

        for(i = 0; i < x+1; i++)
            (*board)[j][i] = set;

        (*board)[j][x+1] = '\0';
        (*board)[j][x+2] = '\0';
    }
}

void printBoard(char **board, int y) {
    board++;

    int j;
    for(j = 0; j < y; j++){
        _cputs(board++[1]);
        putch('\n');
    }
}

int numGen(int num) {
    if (num <= 0)
        return num ? -1 : 0;

    int limit = (RAND_MAX/num)*num;

    if(limit == 0)
        return 0;

    int random;
    while((random = rand()) > limit)
        ;

    return random % num;
}

void placeFood(char **board, int x, int y) {
    int i, j;
    for(j = 1; j < y; j++)
        for(i = 1; i < x; i++)
            if(board[j][i] == 'o')
                return;

    while(board[(j = numGen(y-1))+1][(i = numGen(x-1))+1] == '#')
        ;

    board[j+1][i+1] = 'o';
}

void ggnore(char **board, int x, int y) {
    if(x > 8){
        char *text = "GAME OVER";

        x>>=1;
        y>>=1;

        int i;
        for(i = 0; *text != '\0'; i++)
            board[y][x+i-4] = *text++;
    }
}

void initSnek(
    char **board,
    char **underBoard,
    struct Snek *snek,
    int x,
    int y
) {
    snek->x = x>>1;
    snek->y = y>>1;
    snek->length = '1';
    snek->dir = 'U';

    board[snek->y][snek->x] = '#';
    underBoard[snek->y][snek->x] = snek->length;
}

void growSnek(
    char **board,
    char **underBoard,
    struct Snek *snek,
    int x,
    int y
) {
    (snek->length)++;

    int i, j;
    for(j = 0; j < y+1; j++)
        for(i = 0; i < x+1; i++)
            if(underBoard[j][i] > '0')
                underBoard[j][i]++;
}

void shrinkSnek(
    char **board,
    char **underBoard,
    struct Snek *snek,
    int x,
    int y
) {
    int i, j;
    for(j = 0; j < y+2; j++)
        for(i = 0; i < x+1; i++) {
            if(underBoard[j][i] > '0')
                underBoard[j][i]--;

            if(board[j][i] == '#' && underBoard[j][i] < '1')
                board[j][i] = '.';
        }
}

int moveSnek(char **board, char **underBoard, struct Snek *snek, int x, int y) {
    if(
           board[snek->y-1][snek->x] == '#' && snek->dir == 'U'
        || board[snek->y][snek->x-1] == '#' && snek->dir == 'L'
        || board[snek->y+1][snek->x] == '#' && snek->dir == 'D'
        || board[snek->y][snek->x+1] == '#' && snek->dir == 'R'
    )
        return -1;

    shrinkSnek(board, underBoard, snek, x, y);

    if(
           board[(snek->y)-1][snek->x] == 'o' && snek->dir == 'U'
        || board[snek->y][(snek->x)-1] == 'o' && snek->dir == 'L'
        || board[(snek->y)+1][snek->x] == 'o' && snek->dir == 'D'
        || board[snek->y][(snek->x)+1] == 'o' && snek->dir == 'R'
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

    if(
           snek->y == 1 && snek->dir == 'U'
        || snek->x == -1 && snek->dir == 'L'
        || snek->y == y+2 && snek->dir == 'D'
        || snek->x == x+1 && snek->dir == 'R'
    )
        return -1;

    return 0;
}

void changeSnekDirection(struct Snek *snek, int c) {
    switch(c) {
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
        switch(c=getch()) {
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