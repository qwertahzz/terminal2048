#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <curses.h> // 用来改变命令行界面的显示
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
using namespace std;

// 有几行几列
#define ROW 4 // 可修改
#define COL 4 // 可修改
#define N (ROW>COL?ROW:COL)
// 每个格子的宽度
#define WIDTH 5 // 可修改
// 游戏的最大数值
#define END_NUM 2048 // 可修改
// 生成4的概率（百分比）
#define PROBABILITY_OF_4 10 // 可修改

// 标识游戏状态
#define S_LOSE 0
#define S_WIN 1
#define S_PLAYING 2
#define S_QUIT 3
#define S_NOT_START 4

class Game {
public:
    Game() {
        initGame();
    }

    int getStatus() {return status;}

    // 处理input
    void processInput() {
        // 获取按键
        char ch = getch();
        if(ch >= 'a' && ch <= 'z') {ch -= 'a' - 'A';}

        // 显示按键
        drawChar(ROW, COL * WIDTH * 2, ch);

        // 分情况处理按键
        if(ch == 'Q') {
            status = S_QUIT;
            return;
        } else if(status == S_NOT_START && ch == 'S') {
            status = S_PLAYING;
        } else if(ch == 'R') {
            initGame();
            return;
        } else if(status == S_PLAYING) {
            // move
            if(ch == 'A') {
                if(!moveLeft())return;
            } else if(ch == 'S') {
                rotate();
                bool ret = moveLeft();
                rotate();
                rotate();
                rotate();
                if(!ret) return;
            } else if(ch == 'D'){
                rotate();
                rotate();
                bool ret = moveLeft();
                rotate();
                rotate();
                if(!ret) return;
            } else if(ch == 'W') {
                rotate();
                rotate();
                rotate();
                bool ret = moveLeft();
                rotate();
                if(!ret) return;
            } else {
                return;
            }
        } else {
            return;
        }

        // 生成新的数
        generateNew();

        // 判定游戏是否结束
        checkEnd();

        // 展示当前状态
        showNums();
        showStatus();
    }
private:
    // 绘制游戏的界面
    void drawInfra() {
        /*
        * 画出来的格子 be like:
        * +----+----+----+----+
        * |    |    |    |    |
        * +----+----+----+----+
        * |    |    |    |    |
        * +----+----+----+----+
        * |    |    |    |    |
        * +----+----+----+----+
        * |    |    |    |    |
        * +----+----+----+----+
        */

        // 清屏
        clear();

        // 画 +
        for(int i=0; i<=ROW; ++i) {
            for(int j=0; j<=COL; ++j) {
                drawChar(i * 2, j * WIDTH, '+');
            }
        }

        // 画 -
        for(int i=0; i<=ROW; ++i) {
            for(int j=0; j<COL; ++j) {
                for(int c=1; c<WIDTH; ++c){
                    drawChar(i * 2, j * WIDTH + c, '-');
                }
            }
        }

        // 画 |
        for(int i=0; i<ROW; ++i) {
            for(int j=0; j<=COL; ++j) {
                drawChar(i*2 + 1, j * WIDTH, '|');
            }
        }

        // 显示提示信息
        mvprintw(ROW * 2 + 2, 0, "W(up), S(down), A(left), D(right), Q(quit)"); // 在(2,2)显示字符串
        mvprintw(ROW * 2 + 3, 0, "made by qwerta"); // 在(2,2)显示字符串
    }

    // 在指定位置画一个字符. (左上角为(0,0))
    void drawChar(int row, int col, char ch) {
        move(row, col);
        addch(ch);
    }

    // 在指定位置右对齐画一个数字
    void drawNum(int row, int col, int num) {
        // 先把这里原来的数字抹掉
        for(int c = 0; c < 4; ++c) {
            drawChar(row, col - c, ' ');
        }

        // 再画新的
        while(num) {
            drawChar(row, col, num % 10 + '0');
            num /= 10;
            col--;
        }
    }

    void initGame() {
        // 显示初始界面
        drawInfra();
        for(int i=0; i<ROW; ++i) {
            for(int j=0; j<COL; ++j) {
                drawNum(i * 2 + 1, j * WIDTH + (WIDTH - 1), pow(2, i + j + 4));
            }
        }
        
        // 清空数据
        memset(data, 0, sizeof(data));

        // 设置本轮的行列值
        nowRow = ROW, nowCol = COL;

        // 设置和显示状态
        this -> status = S_NOT_START;
        showStatus();
    }

    // 显示游戏状态对应的字符串
    void showStatus() {
        if (status == S_WIN) {
            mvprintw(ROW * 2 + 5, 0, "[ YOU WIN! PRESS R to Restart.  ]");
        } else if (status == S_LOSE){
            mvprintw(ROW * 2 + 5, 0, "[ YOU LOSE! PRESS R to Restart. ]");
        } else if (status == S_NOT_START) {
            mvprintw(ROW * 2 + 5, 0, "[ WELCOME! PRESS S to Start!    ]");
        } else {
            mvprintw(ROW * 2 + 5, 0, "[ PLAYING...                    ]");
        }
    }

    // 检查游戏是否结束
    void checkEnd() {
        int res = 0;
        for(int i=0; i<ROW; ++i) {
            for(int j=0; j<COL; ++j) {
                if(!data[i][j]) {
                    res++;
                } else if(data[i][j] == END_NUM) {
                    status = S_WIN;
                    return;
                } else if(i && data[i][j] == data[i-1][j]) {
                    res++;
                } else if(j && data[i][j] == data[i][j-1]) {
                    res++;
                }
            }
        }

        if(!res) {status = S_LOSE;}
    }

    // 生成一个新的数
    void generateNew() {
        int res = 0;
        for(int i=0; i<ROW; ++i) {
            for(int j=0; j<COL; ++j) {
                if(!data[i][j]) {res++;}
            }
        }
        if(!res) {return;}

        int num = rand() % 100 < PROBABILITY_OF_4 ? 4 : 2; // 以(PROBABILITY_OF_4%)的概率生成4

        int pos = rand() % res;
        for(int i=0; i<ROW; ++i) {
            for(int j=0; j<COL; ++j) {
                if(!data[i][j]) {
                    if(!pos) {
                        data[i][j] = num;
                        return;
                    }
                    pos--;
                }
            }
        }
    }

    // 显示在开始游戏前的测试数据
    void showTestNum() {
        for(int i=0; i<ROW; ++i) {
            for(int j=0; j<COL; ++j) {
                drawNum(i * 2 + 1, j * WIDTH + (WIDTH - 1), pow(2, i + j + 4));
            }
        }
    }

    // 显示data里的数据
    void showNums() {
        for(int i=0; i<ROW; ++i) {
            for(int j=0; j<COL; ++j) {
                drawNum(i * 2 + 1, j * WIDTH + (WIDTH - 1), data[i][j]);
            }
        }
    }

    // 返回1：动了；返回0：没动
    bool moveLeft() {
        bool ret = false;
        for(int i=0; i<nowRow; ++i) {
            int lastJ = -1926;
            for(int j=-1; j<nowCol-1; ++j) {
                for(int c=j+1; c<nowCol; ++c) {
                    if(data[i][c]) {
                        // 判data[i][j]能不能和data[i][c]合成
                        if(c && data[i][c] == data[i][j] && data[i][c] && j != lastJ) {
                            data[i][j] += data[i][c];
                            data[i][c] = 0;
                            lastJ = j;
                            j--;
                            ret = true;
                        } else if(j+1 != c){
                            swap(data[i][j+1], data[i][c]);
                            ret = true;
                        }
                        break;
                    }
                }
            }
        }

        return ret;
    }

    void rotate() {
        int tmp[N][N] = {0};
        for(int i=0; i<nowRow; ++i) {
            for(int j=0; j<nowCol; ++j) {
                tmp[i][j] = data[i][j];
            }
        }

        swap(nowRow, nowCol);
        memset(data, 0, sizeof(data));

        for(int i=0; i<nowRow; ++i) {
            for(int j=0; j<nowCol; ++j) {
                data[i][j] = tmp[nowCol - j - 1][i];
            }
        }

        FILE* log_file = fopen("log.txt", "w");
        for(int i=0; i<N; ++i) {
            for(int j=0; j<N; ++j) {
                fprintf(log_file, "%d ", tmp[i][j]);
            }
            fprintf(log_file, "\n");
        }
        fprintf(log_file, "\n");
        for(int i=0; i<N; ++i) {
            for(int j=0; j<N; ++j) {
                fprintf(log_file, "%d ", data[i][j]);
            }
            fprintf(log_file, "\n");
        }
        fprintf(log_file, "\n");
        fprintf(log_file, "\n");

        fclose(log_file);
    }

    int data[N][N]; // 取max，便于旋转N
    int nowRow, nowCol;
    int status;
    // bool isEmpty[ROW][COL];
};

void init() {
    // 初始化ncurses
    initscr();
    // 使按键不需要输入回车就可以直接交互
    cbreak();
    // 使按的键不显示
    noecho();
    // 隐藏光标
    curs_set(0);
    // 让随机数可以随机
    srand(time(NULL));
}

void shutdown() {
    // 清理ncurses
    endwin();
}

int main() {
    init();

    Game* game = new Game();

    int test = 0;
    do{
        game -> processInput();
    } while(game -> getStatus() != S_QUIT);

    shutdown();
    return 0;
}
