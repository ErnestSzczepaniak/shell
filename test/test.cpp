#include "test.h"
#include "stream.h"
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "shell.h"

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode() 
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    system("clear");
}
char get()
{
    char temp;
    read(STDIN_FILENO, &temp, 1);
    return temp;
}

void handler_flush(char * buffer, int size)
{
    printf("%s", buffer);
    fflush(nullptr);
    
}

TEST_CASE("test_case_name")
{ 
    enableRawMode();

    Shell sh(handler_flush);

    sh.stream.input.format("qwe qwe %d", 1);

    sh.stream.input.pointer -= 2;

    sh.init();

    Stream temp;

    int k = 2;
    temp.input.format("Pizdeczka %d", k++);

    sh.output(&temp);


    while(1)
    {
        auto character = get();

        auto event = sh.input(character);

        if (event == Shell::Event::ENTER)
        {
            auto * w = sh.stream.output.word();


            // consume stream
            sh.reset();



        }
    }
}