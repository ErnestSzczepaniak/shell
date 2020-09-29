#include "shell_ctrl.h"

namespace shell
{

Ctrl::Ctrl(Stream & stream) : stream(stream)
{

}

Ctrl::~Ctrl()
{

}

Ctrl & Ctrl::left(bool output)
{
    if (stream.command.push.pointer.position() == 0) return *this;

    auto position = stream.command.push.pointer.position();

    for (int i = 0; i < position; i++)
    {
        if (*stream.command.push.pointer != code_null && *stream.command.push.pointer != code_space)
        {
            if (*(stream.command.push.pointer - 1) == code_space && i > 0) break;
            else stream.command.push.pointer.move(-1);
        }
        else stream.command.push.pointer.move(-1);
    }

    auto shift = position - stream.command.push.pointer.position();

    if (shift > 0 && output) stream.output.push.ansi.cursor.move.left(shift);

    return *this;
}

Ctrl & Ctrl::right(bool output)
{
    if (*stream.command.push.pointer == 0) return *this;

    auto initial = stream.command.push.pointer.position();
    auto iterations = stream.command.size_actual() - initial;

    for (int i = 0; i < iterations; i++)
    {
        if (*stream.command.push.pointer != code_null && *stream.command.push.pointer != code_space)
        {
            if (*(stream.command.push.pointer + 1) == code_null || *(stream.command.push.pointer + 1) == code_space)
            {
                if (i > 0)
                {
                    stream.command.push.pointer.move(1);
                    break;
                }
            }
            else stream.command.push.pointer.move(1);
        }
        else stream.command.push.pointer.move(1);
    }

    auto shift = stream.command.push.pointer.position() - initial;

    if (shift > 0 && output) stream.output.push.ansi.cursor.move.right(shift);

    return *this;
}


}; /* namespace: shell */
