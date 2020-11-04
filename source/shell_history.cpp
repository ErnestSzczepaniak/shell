#include "shell_history.h"

namespace shell
{

History & History::push(Stream & stream) // warunek na not empty string
{
    _pointer = -1;

    if (stream.command.push.pointer.position() > 0)
    {
        for (int i = 0; i < _size; i++)
        {
            if (stream.command == _stack[i])
            {
                auto temp = _stack[i];

                for (int j = 0; j < i; j++)
                {
                    _stack[i - j] = _stack[i - j -1];
                }

                _stack[0] = stream.command;

                return *this;
            }
        }
        
        for (int i = 0; i < 9; i++)
        {
            _stack[9 - i] = _stack[9 - i -1];
        }
        
        _stack[0] = stream.command;

        if (_size < 10) _size++;
    }

    return *this;
}

bool  History::up(Stream & stream)
{
    if (_pointer == -1)
    {
        _stash = stream.command;
    }

    if (_pointer < _size -1)
    {
        stream.command.reset();
        stream.command = _stack[++_pointer];
        return true;
    } 
    else return false;
}

bool  History::down(Stream & stream)
{
    if (_pointer == 0)
    {   
        _pointer = -1;
        stream.command = _stash;
        return true;
    }
    else if (_pointer > 0) 
    {
        stream.command.reset();
        stream.command = _stack[--_pointer];
        return true;
    }
    else return false;

}

}; /* namespace: shell */
