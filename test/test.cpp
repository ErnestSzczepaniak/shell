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

/* ---------------------------------------------| handler |--------------------------------------------- */

void handler_flush(char * buffer, int size)
{
    for (int i = 0; i < size; i++) printf("%c", buffer[i]);
    
    fflush(nullptr);
}

bool handler_call(char * name, Stream & stream)
{
    if (strncmp(name, "gen", strlen("gen")) == 0)
    {
        gen(stream);
        return true;
    }
    else if (strncmp(name, "xor", strlen("xor")) == 0)
    {
        xxor(stream);
        return true;
    }
    else if (strncmp(name, "echo", strlen("echo")) == 0)
    {
        echo(stream);
        return true;
    }
    else if (strncmp(name, "rand", strlen("rand")) == 0)
    {
        rnd(stream);
        return true;
    }
    
    return false;
}

/* ---------------------------------------------| test |--------------------------------------------- */

// char * table[]
// {
//     "gen", "xor", "echo", "rand", "git"
// };

// char * keywords_gen[] {"aasd", "asd"};
// char * keywords_xor[] {"aasd", "asd"};
// char * keywords_echo[] {"aasd", "asd"};
// char * keywords_rand[] {"aasd", "asd"};
// char * keywords_git[] {"push", "pull"};

TEST_CASE("test_case_name")
{ 
    enableRawMode();

    Shell shell(handler_flush, handler_call);

    shell.init();

    while(1)
    {
        auto character = get();

        auto event = shell.input(character);

    }
}