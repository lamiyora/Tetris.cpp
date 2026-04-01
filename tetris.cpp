#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <conio.h>

using namespace std;

const int HEIGHT = 20; // y 高度
const int WIDTH = 10; // x

struct block 
{ 
    int x; 
    int y;
};

struct tetriminoPos // 在board的座標
{
    block blocksPos[4];
};

struct tetrimino
{
    int type;
    int rotation;
    int pivotPosX;
    int pivotPosY;
};

// define shape
int Shape[7][4][2] = 
{
    { {-1, 0}, {0, 0}, {1, 0}, {2, 0} }, // I
    { {0, 0}, {1, 0}, {0, 1}, {1, 1} },  // O
    { {-1, 0}, {0, 0}, {1, 0}, {0, 1} }, // T
    { {0, 0}, {1, 0}, {-1, 1}, {0, 1} }, // S 
    { {-1, 0}, {0, 0}, {0, 1}, {1, 1} }, // Z
    { {-1, 0}, {0, 0}, {1, 0}, {1, 1} }, // J
    { {-1, 0}, {0, 0}, {1, 0}, {-1, 1} } // L
};


void initBoard(char board[HEIGHT][WIDTH])
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            board[i][j] = '_';
        }
    }
}

tetrimino summonShape(int x, int y)
{
    tetrimino t;
    t.pivotPosX = x;
    t.pivotPosY = y;
    t.type = rand() % 7;
    t.rotation = 0;
    // t.type = 1; // 生成O (debug) 
    return t;
}

tetriminoPos getShapePos(tetrimino t)
{
    tetriminoPos s;

    for  (int i = 0; i < 4; i++)
    {
        int rx = Shape[t.type][i][0];
        int ry = Shape[t.type][i][1];

        if (t.type != 1)  // if not O
        {
            int rotate = t.rotation % 4; // 轉幾次
            for (int j = 0; j < rotate; j++)
            {
                int temp = rx; // x
                rx = -ry;
                ry = temp;
            }
        }

        s.blocksPos[i].x = rx + t.pivotPosX;
        s.blocksPos[i].y = ry + t.pivotPosY;
    }
    return s; // 回傳在board的座標
}


void printBoard(char board[HEIGHT][WIDTH], tetrimino t)
{
    tetriminoPos s = getShapePos(t); // s = {{},{},{},{}}

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            bool isBlock = false;

            for (int k = 0; k < 4; k++)
            {
                if (i == s.blocksPos[k].y && j == s.blocksPos[k].x)
                {
                    isBlock = true;
                    break;
                }
            }

            if (isBlock) cout << '#';
            else cout << board[i][j];
        }
        cout << endl;
    }
}

bool isValidMove(char board[HEIGHT][WIDTH], tetrimino t, int dx, int dy)
{
    tetriminoPos s = getShapePos(t);
    
    for (int i = 0; i < 4; i++)
    {
        int x = s.blocksPos[i].x;
        int y = s.blocksPos[i].y;

        int newX = x + dx;
        int newY = y + dy;

        if (newX < 0 || newX >= WIDTH) return false;
        else if (newY < 0 || newY >= HEIGHT) return false;
        else if (board[newY][newX] == '#') return false;
    }
    return true;
}

tetrimino tryMove(char board[HEIGHT][WIDTH], tetrimino t, int dx, int dy)
{
    tetriminoPos s = getShapePos(t);

    if (isValidMove(board, t, dx, dy))
    {
        t.pivotPosX += dx;
        t.pivotPosY += dy;
    }
    return t;
}

void writeToBoard(char board[HEIGHT][WIDTH], tetrimino t)
{
    tetriminoPos s = getShapePos(t);
    for (int i = 0; i < 4; i++)
    {
        board[s.blocksPos[i].y][s.blocksPos[i].x] = '#'; // 將此方塊寫死在目前座標
    }
}

bool isLineFull(char currLine[WIDTH])
{
    for (int i = 0; i < WIDTH; i++)
    {
        if (currLine[i] != '#')
            return false;
    }
    return true;
}

void clearLine(char board[HEIGHT][WIDTH], int rowIndex)
{
    while (rowIndex > 0)
    {
        for (int i = 0; i < WIDTH; i++)
        {
            board[rowIndex][i] = board[rowIndex-1][i];
        }
        rowIndex--;
    }
    // clear top row
    for (int i = 0; i < WIDTH; i++)
    {
        board[0][i] = '_';
    }
}

void gameOver(int score)
{
    cout << "GAME OVER!\n";
    cout << "SCORE: " << score;
}

int main()
{
    srand(time(NULL)); // random seed
    char board[HEIGHT][WIDTH];
    tetrimino t;
    t = summonShape(5,0); // 生成新方塊
    initBoard(board);
    int score = 0;

    while (true)
    {
        if (_kbhit())
        {
            char key = _getch();
            if (key == 'a')
                t = tryMove(board, t, -1, 0);
            else if (key == 'd')
                t = tryMove(board, t, 1, 0);
            else if (key == 's')
                t = tryMove(board, t, 0, 1);

            else if (key == 'w')
            {
                tetrimino newT = t;
                newT.rotation++;
                if (isValidMove(board, newT, 0, 0)) // position now
                    t = newT;
            }
                
                
        }

        // 向下
        if (isValidMove(board, t, 0, 1))
        {
            t = tryMove(board, t, 0, 1);
        }
        else
        {
            if (t.pivotPosY <= 0) // 碰到頂 & 不能向下
                break;
            writeToBoard(board, t);
            t = summonShape(5, 0);
        }

        // 逐行檢查
        for (int i = 0; i < HEIGHT; i++)
        {
            if (isLineFull(board[i]))
            {
                clearLine(board, i);
                score += 100;
                i--;   // 重新檢查同一行（因為掉下來了）
            }
        }
            
        printf("\x1b[H");   // 回到左上角
        printBoard(board, t);
        _sleep(200);
    }
    gameOver(score);
    return 0;
}