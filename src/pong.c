#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define h 25
#define w 80

void showScene(int xl, int xr, int yl, int yr, int xball, int yball,
               int scoreLeft, int scoreRight);
void startGame();
int moveRocketUp(int current, int speed);
int moveRocketDown(int current, int speed);
int xmoveBall(int x, int speed, int xmode);
int ymoveBall(int y, int speed, int ymode);
int ycheckCollisionWall(int y, int ymode);
int xcheckCollisionWallLeft(int x);
int xcheckCollisionWallRight(int x, int speedBall);
int checkCollisionL(int x, int y, int xl, int yl, int mode);
int checkCollisionR(int x, int y, int xr, int yr, int mode);
void showFinish(int number);
void key_pressed();

int main() {
  startGame();
  return 0;
}

void showScene(int xl, int xr, int yl, int yr, int xball, int yball,
               int scoreLeft, int scoreRight) {
  int i;
  int j;
  char c;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i == w / 2 && j > 1 && j < h - 1)
        c = '|';
      else
        c = (j == 0 || j == h - 1) ? '-' : ' ';

      if (i == 0 && j > 0 && j < h - 1) c = '|';

      if (i == 79 && j > 0 && j < h - 1) c = '|';

      if ((j == 1) && (i == 27)) {
        printf("Player 1 [%.2d]|[%.2d] Player 2", scoreLeft, scoreRight);
        i += 27;
      }

      if (i == xl && j > yl - 2 && j < yl + 2) c = '|';

      if (i == xr && j > yr - 2 && j < yr + 2) c = '|';

      if (i == xball && j == yball) c = 'o';

      printf("%c", c);
    }
    printf("\n");
  }
  printf("\n");
}

void startGame() {
  key_pressed();
  fd_set rfds;

  char choice;

  struct timeval tv;
  int scoreLeft = 0;
  int scoreRight = 0;

  const int yDef = 20;
  const int xDefLeft = 5;
  const int xDefRight = 75;
  const int speed = 2;

  int yLeft = yDef;
  int yRight = yDef;

  int xBallDef = w / 2;
  int yBallDef = h / 2;
  int xBall = w / 2;
  int yBall = h / 2;
  int ymode = 1;
  int xmode = 1;

  showScene(xDefLeft, xDefRight, yDef, yDef, xBall, yBall, scoreLeft,
            scoreRight);

  int temp = 1;

  while (1) {
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int retval = select(1, &rfds, NULL, NULL, &tv);
    if (retval)
      choice = getchar();
    else
      choice = 'i';

    int checkL = 0;
    int checkR = 0;

    ymode = ycheckCollisionWall(yBall, ymode);

    xmode = checkCollisionL(xBall, yBall, xDefLeft, yLeft, xmode);

    xmode = checkCollisionR(xBall, yBall, xDefRight, yRight, xmode);

    checkL = xcheckCollisionWallLeft(xBall);
    checkR = xcheckCollisionWallRight(xBall, speed);

    if (checkL || checkR) {
      yBall = yBallDef;
      xBall = xBallDef;

      if (checkL == 1)
        ++scoreRight;
      else
        ++scoreLeft;

    } else {
      if (temp >= 1000) {
        yBall = ymoveBall(yBall, speed, ymode);
        xBall = xmoveBall(xBall, speed, xmode);
        temp = 1;
      }
    }

    if (choice == 'A' || choice == 'a') {
      yLeft = moveRocketUp(yLeft, speed);
    }

    if (choice == 'Z' || choice == 'z') {
      yLeft = moveRocketDown(yLeft, speed);
    }

    if (choice == 'K' || choice == 'k') {
      yRight = moveRocketUp(yRight, speed);
    }

    if (choice == 'M' || choice == 'm') {
      yRight = moveRocketDown(yRight, speed);
    }

    if (choice == 'Q' || choice == 'q') {
      return;
    }

    printf("\e[2J\e[H");

    if (scoreLeft == 21 || scoreRight == 21) {
      if (scoreLeft == 21) {
        showFinish(1);
        return;
      } else {
        showFinish(2);
        return;
      }

    } else
      showScene(xDefLeft, xDefRight, yLeft, yRight, xBall, yBall, scoreLeft,
                scoreRight);
    usleep(50);
    temp++;
  }
}

int moveRocketUp(int current, int speed) {
  if (current < speed + 1)
    return current;
  else
    return current - speed;
}

int moveRocketDown(int current, int speed) {
  if (current > h - speed - 3)
    return current;
  else
    return current + speed;
}

int xmoveBall(int x, int speed, int xmode) { return x + speed * xmode; }

int ymoveBall(int y, int speed, int ymode) { return y + speed * ymode; }

int ycheckCollisionWall(int y, int ymode) {
  if (y >= h - 1) ymode = ymode * (-1);
  if (y <= 1) ymode = ymode * (-1);

  return ymode;
}

int xcheckCollisionWallLeft(int x) {
  if (x <= 0) return 1;
  return 0;
}

int xcheckCollisionWallRight(int x, int speedBall) {
  if (x >= w - speedBall) return 1;
  return 0;
}

int checkCollisionL(int x, int y, int xl, int yl, int mode) {
  if ((x == xl + 1 || x == xl - 1) && y < yl + 2 && y > yl - 2)
    mode = mode * (-1);

  return mode;
}

int checkCollisionR(int x, int y, int xr, int yr, int mode) {
  if ((x == xr + 1 || x == xr - 1) && y < yr + 2 && y > yr - 2)
    mode = mode * (-1);

  return mode;
}

void showFinish(int number) {
  int i;
  int j;
  char c;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i == w / 2 && j > 0 && j < h - 1)
        c = '|';
      else
        c = (j == 0 || j == h - 1) ? '-' : ' ';

      if (i == 0 && j > 0 && j < h - 1) c = '|';

      if (i == 79 && j > 0 && j < h - 1) c = '|';

      if (i == 26 && j == h / 2) {
        printf("> TOTAL SCORE: 21 - PLAYER %d WON! <", number);
        i += 35;
      }

      printf("%c", c);
    }
    printf("\n");
  }
}

void key_pressed() {
  static struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}
