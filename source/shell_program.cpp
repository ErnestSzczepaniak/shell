#include "shell_program.h"

namespace shell
{

Program::Program()
{

}

Program::~Program()
{

}

Program & Program::name(char * name)
{
    _name = name;

    return *this;
}

char * Program::name()
{
    return _name;
}

Program & Program::keywords(char ** keywords)
{
    _keywods = keywords;

    return *this;
}

char ** Program::keywords()
{
    return _keywods;
}

char * Program::keyword(int index)
{
    if (index < _size) return _keywods[index];
    return nullptr;
}

Program & Program::size_keywords(int value)
{
    _size = value;

    return *this;
}

int Program::size_keywords()
{
    return _size;
}

}; /* namespace: shell */
