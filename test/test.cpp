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
    if (stream.command.parse.is_present("-c"))
    {
        stream.output.set.decimal(10);
    }
    else if (stream.command.parse.is_present("-d"))
    {
        stream.output.set.text("Option -d provided");
    }
    else
    {
        stream.error.set.text("no option provided");
    }   
}

void p2(Stream & stream)
{
    auto increment = stream.command.get.decimal();

    auto value = stream.input.get.decimal();

    if (increment < 100) stream.output.set.format("nee value = %d", value + increment);
    else stream.error.set.text("Value out of range ...");

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

    int index[10];

    while(1)
    {
        auto character = get();

        auto event = shell.input(character);

        if (event == Shell::Event::ENTER)
        {
            auto count = tools::string::count::word(shell.stream.command.get.pointer, "|");

            for (int i = 0; i < count + 1; i++)
            {
                auto size = tools::string::get::size(shell.stream.command.get.pointer, "|\0");

                if (size <= 0) break;

                shell.stream.command.get.pointer.save();

                auto * name = shell.stream.command.get.word();

                if (auto * candidate = _candidate(name); candidate != nullptr)
                {
                    auto span = tools::string::get::size(shell.stream.command.get.pointer, "|\0");

                    shell.stream.command.get.pointer.stop(shell.stream.command.get.pointer + span);
                    shell.stream.command.parse.pointer = shell.stream.command.get.pointer;

                    candidate(shell.stream);

                    if (shell.stream.error.size_actual() > 0)
                    {
                        shell.stream.error.set.pointer.reset();
                        shell.stream.error.set.ansi.special.r().n();
                        shell.stream.error.set.ansi.color.foreground(255, 0, 0);
                        shell.stream.error.set.text("on thread ").word(name).text(": ");
                        shell.stream.error.set.pointer.align_end();
                        shell.stream.error.set.ansi.special.r().n();
                    }
                    else if (i < count) shell.stream.flush();
                }

                shell.stream.command.get.pointer.restore();
                shell.stream.command.get.pointer += (size + 2);
            }
            
            if (shell.stream.error.size_actual() > 0)
            {
                handler_flush(shell.stream.error.buffer, shell.stream.error.size_actual());

                shell.reset(true);
            }
            else if (shell.stream.output.size_actual() > 0)
            {
                handler_flush(shell.stream.output.buffer, shell.stream.output.size_actual());

                shell.reset(true);
            }
            else shell.reset(false);
        }
    }
}