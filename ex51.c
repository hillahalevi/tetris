//
// Created by hilla on 6/10/19.
//

#include <stdio.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>

#define STDERR 2
#define ERROR "Error in system call\n"
#define DRAW "./draw.out"


char getch();

void PrintError();

int IsValidKey(char key);


int main() {
    int Pipe[2];
    pipe(Pipe);
    int pid;
    if ((pid = fork()) < 0) {
        PrintError();
    }
    // Child process.
    if (pid == 0) {
        // Force stdin to be the read side of the pipe.
        dup2(Pipe[0], 0);
        // Call exe to run draw.
        execlp(DRAW, DRAW, NULL);
        // gets here only if exe command fails- Print error.
        PrintError();
    }
    // Father Process
    char key;
    while (1) {
        key = getch();
        if (!IsValidKey(key)) {
            continue;
        }
        // Write the game key to the pipe- Print the error message if  failed.
        if (write(Pipe[1], &key, 1) < 0) {
            PrintError();
        }
        kill(pid, SIGUSR2);
        if (key == 'q') {
            break;
        }
    }
    return 0;
}

/**
 * getch() function supplied by instructor.
 * Used to read characters from the shell.
 * @return
 */
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}


/**
 * Check if the char input is a valid character, returns 1 if yes, otherwise return 0
 * @param key
 * @return
 */
int IsValidKey(char key) {
    // Checking the keys.
    // a - Left, d - Right, s - Down, w - Turn shape, q - Quit.
    if (key == 'a' || key == 'd' || key == 's' || key == 'w' || key == 'q') {
        return 1;
    }
    return 0;
}


/**
 * Prints error message
 */
void PrintError() {

    write(STDERR, ERROR, sizeof(ERROR) - 1);
}

