#include <iostream>
#include <unistd.h>      // for usleep
#include <termios.h>     // for getch() replacement
#include <fcntl.h>       // for kbhit() replacement
#include <cstdlib>       // for rand()
#include <ctime>         // for time()

using namespace std;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;
bool gameOver;
const int height = 20;
const int width = 20;
int headX, headY, fruitX, fruitY, score;
int tailx[100], taily[100];
int tail_len;

// ====== PLATFORM-INDEPENDENT getch() and kbhit() ======
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);            // get old terminal attributes
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);          // disable buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);   // apply new attributes
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);    // get current flags
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // set non-blocking input

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);   // restore old attributes
    fcntl(STDIN_FILENO, F_SETFL, oldf);        // restore old flags

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // turn off canonical and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
// =======================================================

void setup() {
    srand(time(0));
    gameOver = false;
    dir = STOP;
    headX = width / 2;
    headY = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    tail_len = 0;
}

void draw() {
    system("clear"); // use "cls" on Windows
    // Upper border
    cout << "\t\t";
    for (int i = 0; i < width - 8; i++) {
        cout << "||";
    }
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) cout << "\t\t||";

            if (i == headY && j == headX)
                cout << "O";
            else if (i == fruitY && j == fruitX)
                cout << "*";
            else {
                bool print = false;
                for (int k = 0; k < tail_len; k++) {
                    if (tailx[k] == j && taily[k] == i) {
                        cout << "o";
                        print = true;
                    }
                }
                if (!print) cout << " ";
            }

            if (j == width - 1) cout << "||";
        }
        cout << endl;
    }

    // Lower border
    cout << "\t\t";
    for (int i = 0; i < width - 8; i++) {
        cout << "||";
    }
    cout << endl;
    cout << "\t\t\tScore: " << score << endl;
}
 
void input() {
    if (kbhit()) {
        switch (getch()) {
        case 'a':
            dir = LEFT;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'w':
            dir = UP;
            break;
        case 's':
            dir = DOWN;
            break;
        }
    }
}

void logic() {
    int prevx = tailx[0];
    int prevy = taily[0];
    int prev2x, prev2y;
    tailx[0] = headX;
    taily[0] = headY;
    for (int i = 1; i < tail_len; i++) {
        prev2x = tailx[i];
        prev2y = taily[i];
        tailx[i] = prevx;
        taily[i] = prevy;
        prevx = prev2x;
        prevy = prev2y;
    }

    switch (dir) {
    case LEFT:
        headX--;
        break;
    case RIGHT:
        headX++;
        break;
    case UP:
        headY--;
        break;
    case DOWN:
        headY++;
        break;
    }

    // wrap around logic
    if (headX >= width) headX = 0;
    else if (headX < 0) headX = width - 1;
    if (headY >= height) headY = 0;
    else if (headY < 0) headY = height - 1;

    // snake bites itself
    for (int i = 0; i < tail_len; i++) {
        if (tailx[i] == headX && taily[i] == headY) {
            gameOver = true;
        }
    }

    // eat fruit
    if (headX == fruitX && headY == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        tail_len++;
    }
}

int main() {
    char start;
    cout << "\t-------------------------------" << endl;
    cout << "\t\t :Snake King:" << endl;
    cout << "\t-------------------------------" << endl;
    cout << "\tPress 's' to start: ";
    cin >> start;

    if (start == 's') {
        setup();
        while (!gameOver) {
            draw();
            input();
            logic();
            usleep(100000); // 100 ms delay
        }
    }

    cout << "\nGame Over! Final Score: " << score << endl;
    return 0;
}
