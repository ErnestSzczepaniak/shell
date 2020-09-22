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

using Handler = void (*)(Stream &);

void p1(Stream & stream)
{
    auto value = stream.input.get.decimal();

    stream.output.set.decimal(value + 1);
}

void p2(Stream & stream)
{
    auto flag = stream.input.get.character();
    auto value = stream.input.get.decimal();

    stream.output.set.format("Flag was %c, value was %d", flag, value);
}

Handler _candidate(char * name)
{
    if (strncmp(name, "p1", strlen("p1")) == 0) return p1;
    else if (strncmp(name, "p2", strlen("p2")) == 0) return p2;
    return nullptr;
}

TEST_CASE("test_case_name")
{ 
    enableRawMode();

    Shell shell(handler_flush);

    shell.init();

    shell.stream.error.set.text("pizdeczka");

    shell.output(shell.stream.error);


    while(1)
    {
        auto character = get();

        auto event = shell.input(character);

        if (event == Shell::Event::ENTER)
        {
            stream::Channel input;
            input = shell.stream.input;
            shell.stream.input.reset();

            auto count = tools::string::count::word(input.buffer, "|");

            for (int i = 0; i < count + 1; i++)
            {
                auto * name = input.get.word();

                if (auto * candidate = _candidate(name); candidate != nullptr)
                {
                    shell.stream.input.set.pointer.reset();

                    if (i < count) shell.stream.input.set.text(input.get.text("|"), "|");
                    else
                    {
                        shell.stream.input.set.text(input.get.text());
                        shell.stream.input.set.text(" ");
                    }
                    
                    input.get.pointer++;

                    candidate(shell.stream);

                    if (i < count) shell.stream.flush();
                }
            }

            handler_flush(shell.stream.output.buffer, shell.stream.output.size_actual());
            // consume stream
            shell.reset();
        }
    }
}