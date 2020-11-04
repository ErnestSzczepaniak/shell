#include "shell_history.h"

namespace shell
{

History & History::push(Stack & stack) // warunek na not empty string
{
    if (stack.push.pointer.position() > 0)
    {
        if (auto index = _find(stack); index == -1)
        {
            _shift(_size);
            if (_size < 10) _size++;
        }
        else _shift(index);

        _stack[0] = stack;
    }

    _pointer = -1;
    return *this;
}

bool History::up(Stack & stack)
{
    if (_pointer < _size - 1) _pointer++;
    else return false;

    if (_pointer == 0) _stash = stack;

    stack = _stack[_pointer];

    return true;
}

bool History::down(Stack & stack)
{
    if (_pointer >= 0) _pointer--;
    else return false;

    if (_pointer == -1) stack = _stash;
    else stack = _stack[_pointer];

    return true;
}

/* ---------------------------------------------| info |--------------------------------------------- */

int History::_find(Stack & stack)
{
    for (int i = 0; i < _size; i++) if (_stack[i] == stack) return i;
    return -1;
}

void History::_shift(int items)
{
    for (int i = 0; i < items; i++) _stack[items - i] = _stack[items - i - 1];
}

}; /* namespace: shell */
