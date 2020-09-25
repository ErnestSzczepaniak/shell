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
    for (int i = 0; i < size; i++) printf("%c", buffer[i]);
    
    fflush(nullptr);
}

using Handler = void (*)(Stream &);

void gen(Stream & stream)
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

void xxor(Stream & stream)
{
    auto size = stream.input.push.pointer.position();
    auto value = stream.command.pop.hexadecimal();

    if (size == 0)
    {
        stream.error.push.text("No data provided ...");  
        return;
    }

    auto * data = (unsigned char *) stream.input.pop.data(size);
    for (int i = 0; i < size; i++) data[i] ^= value;
    stream.output.push.data(data, size);
}

void echo(Stream & stream)
{
    auto * w = stream.command.pop.text();
    stream.output.push.text(w);
}

void rnd(Stream & stream)
{
    if (stream.command.parse.option("-h").is_present())
    {
        stream.output.push.text("Usage:").ansi.special.r().n();
        stream.output.push.text("\trand [number]").ansi.special.r().n();
        stream.output.push.text("Options:").ansi.special.r().n();
        stream.output.push.text("\t-h\tprint this summary");
    }

    if (stream.command.push.pointer.position() == 0)
    {
        stream.error.push.text("Please specify number of values to generate ...");
        return;
    }

    auto size = stream.command.pop.decimal();

    for (int i = 0; i < size; i++) stream.output.push.decimal(rand() % 255, "");   
}

Handler _candidate(char * name)
{
    if (strncmp(name, "gen", strlen("gen")) == 0) return gen;
    else if (strncmp(name, "xor", strlen("xor")) == 0) return xxor;
    else if (strncmp(name, "echo", strlen("echo")) == 0) return echo;
    else if (strncmp(name, "rand", strlen("rand")) == 0) return rnd;
    return nullptr;
}

TEST_CASE("test_case_name")
{ 
    enableRawMode();

    Shell shell(handler_flush);
    Stream user;

    shell.init();

    while(1)
    {
        auto character = get();

        auto event = shell.input(character);

        if (event == Shell::Event::ENTER)
        {
            auto count = tools::string::count::word(shell.stream.command.pop.pointer, "|");

            for (int i = 0; i < count + 1; i++)
            {
                if (shell.stream.command.find.offset_to("|\0") <= 0) break;

                auto * name = shell.stream.command.pop.word();

                if (auto * candidate = _candidate(name); candidate != nullptr)
                {
                    user.command.push.text(shell.stream.command.pop.text("|\0"), "|\0");
                    shell.stream.command.pop.pointer.move(1);

                    candidate(user);

                    user.command.reset();

                    if (user.error.push.pointer.position() > 0) break;
                    else if (i < count) user.flush();
                    else if (i == count) user.input.reset();
                }
                else
                {
                    user.error.push.format("Command %s unknown...", name);
                    break;
                }
            }
            
            if (user.error.push.pointer.position() > 0)
            {
                user.error.push.pointer.reset();
                user.error.push.ansi.special.r().n();
                user.error.push.ansi.color.foreground(255, 0, 0);
                user.error.push.pointer.position(user.error.size_actual());
                user.error.push.ansi.special.r().n();

                handler_flush(user.error.buffer, user.error.push.pointer.position());

                user.error.reset();

                shell.reset(true);
            }
            else if (user.output.push.pointer.position() > 0)
            {
                handler_flush(user.output.buffer, user.output.push.pointer.position());

                user.output.reset();

                shell.reset(true);
            }
            else shell.reset(false);
        }
    }
}