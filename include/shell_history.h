#ifndef _shell_history_h
#define _shell_history_h

/**
 * @file	shell_history.h
 * @author	en2
 * @date	28-09-2020
 * @brief	
 * @details	
**/

#include "stream.h"

namespace shell
{

class History
{
    static constexpr auto size = 10;
    using Stack = stream::Stack<128>;

public:
    History & push(Stack & stack);

    bool up(Stack & stack);
    bool down(Stack & stack);

protected:
    int _find(Stack & stack);
    void _shift(int items);

private:
    int _size = 0;
    int _pointer = -1;

    Stack _stack[size];
    Stack _stash;

}; /* class: History */

}; /* namespace: shell */

#endif /* define: shell_history_h */