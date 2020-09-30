#include "shell_ctrl.h"

namespace shell
{

Ctrl & Ctrl::left(Stream & stream)
{
    auto span = stream.command.push.pointer.position();

    for (int i = 0; i < span; i++)
    {
        stream.command.push.pointer.move(-1);

        if (*stream.command.push.pointer != 0 && *(stream.command.push.pointer - 1) == code_space) break;
    }
    
    auto shift = span - stream.command.push.pointer.position();

    if (shift > 0) stream.output.push.ansi.cursor.move.left(shift);

    return *this;
}

Ctrl & Ctrl::right(Stream & stream)
{
    auto initial = stream.command.push.pointer.position();
    auto span = stream.command.size_actual() - initial;

    for (int i = 0; i < span; i++)
    {
        stream.command.push.pointer.move(1);

        if (*stream.command.push.pointer == code_space && *(stream.command.push.pointer + 1) != 0) break;
    }

    auto shift = stream.command.push.pointer.position() - initial;

    if (shift > 0) stream.output.push.ansi.cursor.move.right(shift);

    return *this;
}


}; /* namespace: shell */
