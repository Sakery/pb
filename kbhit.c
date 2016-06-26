#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static struct termios oldt;
static int oldf;

void stash_term_settings() {
  tcgetattr(STDIN_FILENO, &oldt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
}

void restore_term_settings() {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
}

void non_blocking_term() {
  struct termios newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
}

void blocking_term() {
  struct termios newt = oldt;
  newt.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
}

/*
void begin_key_press() {
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
}

void end_key_press() {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
}

int is_key_press() {
  int ch = getc(stdin);
  if(ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }
  return 0;
}
*/