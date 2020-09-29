#include "shell_hint.h"

namespace shell
{

Hint::Hint(Stream & stream, Handler_program program, Handler_keyword keyword) 
: 
stream(stream),
_handler_program(program),
_handler_keyword(keyword)
{

}

Hint::~Hint()
{

}

bool Hint::tab()
{   
    if (stream.command.push.pointer.position() == 0) return true;
    if (*stream.command.push.pointer != 0) return true;

    stream.command.push.pointer.save();

    auto * candidate = _program_find();
    auto program_number = _program_count();

    stream.command.push.pointer.restore();

    if (auto * program = _program_ready(program_number, candidate); program == nullptr) // hint parent
    {
        _program_calculate(program_number, candidate);

        auto [max, pos, duplicate] = _find_max(program_number);

        if (max == 0) return true;

        if (duplicate == false)
        {
            stream.command.push.format("%s ", _handler_program(pos) + max);
            stream.output.push.format("%s ", _handler_program(pos) + max);

            return true;
        }
        else
        {
            for (int i = 0; i < program_number; i++)
            {
                if (_match[i] == max)
                {
                    stream.output.push.ansi.special.r().n();
                    stream.output.push.text(_handler_program(i));
                }
            }   

            stream.output.push.ansi.special.r().n();
            
            return false;
        }
    }
    else // hint keyword
    {
        if (*(stream.command.push.pointer - 1) == code_space) return true;
        else if (stream.command.push.pointer.position() == strlen(program)) // nie dziala po pipe
        {
            stream.command.push.character(code_space, "");
            stream.output.push.character(code_space, "");
            return true;
        }

        auto keyword_number = _keyword_count(program);

        stream.command.push.pointer.save();

        auto * candidate = _keyword_find();

        stream.command.push.pointer.restore();

        _keyword_calculate(keyword_number, program, candidate);

        auto [max, pos, duplicate] = _find_max(keyword_number);

        if (max == 0) return true;

        if (duplicate == false)
        {
            stream.command.push.format("%s ", _handler_keyword(program, pos) + max);
            stream.output.push.format("%s ", _handler_keyword(program, pos) + max);

            return true;  
        }
        else
        {
            for (int i = 0; i < keyword_number; i++)
            {
                if (_match[i] == max)
                {
                    stream.output.push.ansi.special.r().n();
                    stream.output.push.text(_handler_keyword(program, i));
                }
            }
            stream.output.push.ansi.special.r().n();

            return false;
        }
    }

    return true;
}

int Hint::_program_count()
{
    auto count = 0;

    for (int i = 0; i < size; i++)
    {
        if (_handler_program(i) != nullptr) count++;    
        else break;
    }
    return count;
}

int Hint::_keyword_count(char * program)
{
    auto count = 0;

    for (int i = 0; i < size; i++)
    {
        if (_handler_keyword(program, i) != nullptr) count++;    
        else break;
    }
    return count;
}

char * Hint::_program_find()
{
    auto span = stream.command.push.pointer.position();

    for (int i = 0; i < span; i++)
    {
        stream.command.push.pointer.move(-1);

        if (stream.command.push.pointer.position() > 1)
        {
            char * candidate = stream.command.push.pointer;
            char * space = stream.command.push.pointer - 1;
            char * pipe = stream.command.push.pointer - 2;

            if (*candidate != code_space && *candidate != code_pipe)
            {
                if (*space == code_space && *pipe == code_pipe) return stream.command.push.pointer;
            }
        }        
    }
    
    return stream.command.push.pointer;
}

char * Hint::_keyword_find()
{
    auto span = stream.command.push.pointer.position();

    for (int i = 0; i < span; i++)
    {
        stream.command.push.pointer.move(-1);

        if (stream.command.push.pointer.position() > 1)
        {
            char * candidate = stream.command.push.pointer;
            char * space = stream.command.push.pointer - 1;

            if (*candidate != code_space && *space == code_space) return stream.command.push.pointer;
        }  
    }   

    return stream.command.push.pointer;
}


char * Hint::_program_ready(int count, char * program)
{
    for (int i = 0; i < count; i++)
    {
        auto * candidate = _handler_program(i);

        if (strncmp(program, candidate, strlen(candidate)) == 0) return candidate;
    }
    
    return nullptr;
}

void Hint::_program_calculate(int count, char * program)
{
    for (int i = 0; i < count; i++)
    {
        _match[i] = _match_calculte(program, _handler_program(i));
    }
}

void Hint::_keyword_calculate(int count, char * program, char * keyword)
{
    for (int i = 0; i < count; i++)
    {
        _match[i] = _match_calculte(keyword, _handler_keyword(program, i));
    }  
}

int Hint::_match_calculte(char * hint, char * word)
{
    auto size = strlen(word);
    auto count = 0;

    for (int i = 0; i < size; i++)
    {
        if (hint[i] == word[i]) count++;
        else break;
    }
    return count;
}

Hint::Result_max Hint::_find_max(int count)
{
    auto max = -1;
    auto pos = -1;
    bool duplicate = false;

    for (int i = 0; i < count; i++)
    {
        if (_match[i] > max)
        {
            max = _match[i];
            pos = i;
            duplicate = false;
        }
        else if (_match[i] == max)
        {
            duplicate = true;
        }
    }
    
    return {max, pos, duplicate};
}


}; /* namespace: shell */
