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
    auto size = stream.command.parse.option("-s").decimal();

    if (stream.command.parse.option("-f").is_equal("D"))
    {
        for (int i = 0; i < size; i++)
        {
            stream.output.push.decimal(i);
        }   
    }
    else if (stream.command.parse.option("-f").is_equal("R"))
    {
        auto * mem = new unsigned char [size];

        for (int i = 0; i < size; i++) mem[i] = i + 1;

        stream.output.push.data(mem, size);
        delete [] mem;
    }
    else if (stream.command.parse.option("-f").is_equal("2H"))
    {
        for (int i = 0; i < size; i++)
        {
            stream.output.push.hexadecimal(i, 2);
        }   
    }
    else if (stream.command.parse.option("-f").is_equal("4H"))
    {
        for (int i = 0; i < size; i++)
        {
            stream.output.push.hexadecimal(i, 4);
        }   
    }
    else if (stream.command.parse.option("-f").is_equal("8H"))
    {
        for (int i = 0; i < size; i++)
        {
            stream.output.push.hexadecimal(i, 8);
        }   
    }
    else stream.error.push.text("No output specified");
}

void p2(Stream & stream)
{
    auto size = stream.input.push.pointer.position();

    if (size < 1024) stream.output.push.format("Received %d characters", size);
    else stream.error.push.text("Buffer overflow ...");
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
            auto count = tools::string::count::word(shell.stream.command.pop.pointer, "|");

            for (int i = 0; i < count + 1; i++)
            {
                auto size = tools::string::get::size(shell.stream.command.pop.pointer, "|\0");

                if (size <= 0) break;

                shell.stream.command.pop.pointer.save();

                auto * name = shell.stream.command.pop.word();

                if (auto * candidate = _candidate(name); candidate != nullptr)
                {
                    auto span = tools::string::get::size(shell.stream.command.pop.pointer, "|\0");

                    shell.stream.command.pop.pointer.stop(shell.stream.command.pop.pointer + span);
                    shell.stream.command.parse.pointer = shell.stream.command.pop.pointer;

                    candidate(shell.stream);

                    if (shell.stream.error.size() > 0)
                    {
                        shell.stream.error.push.pointer.reset();
                        shell.stream.error.push.ansi.special.r().n();
                        shell.stream.error.push.ansi.color.foreground(255, 0, 0);
                        shell.stream.error.push.text("on thread ").word(name).text(": ");
                        shell.stream.error.push.pointer.position(shell.stream.error.size());
                        shell.stream.error.push.ansi.special.r().n();

                        shell.stream.command.pop.pointer.restore();
                        
                        break;
                    }
                    else if (i < count) shell.stream.flush();
                }

                shell.stream.command.pop.pointer.restore();
                shell.stream.command.pop.pointer.move(size + 2);
            }
            
            if (shell.stream.error.push.pointer.position() > 0)
            {
                handler_flush(shell.stream.error.buffer, shell.stream.error.push.pointer.position());

                shell.reset(true);
            }
            else if (shell.stream.output.push.pointer.position() > 0)
            {
                handler_flush(shell.stream.output.buffer, shell.stream.output.push.pointer.position() );

                shell.reset(true);
            }
            else shell.reset(false);
        }
    }
}