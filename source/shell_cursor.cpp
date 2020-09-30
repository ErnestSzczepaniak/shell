#include "shell_cursor.h"

namespace shell
{

Cursor & Cursor::home(Stream & stream)
{
    if (stream.command.push.pointer.position() == 0) return *this;

    stream.output.push.ansi.cursor.move.left(stream.command.push.pointer.position());
    stream.command.push.pointer.reset();

    return *this;
}

Cursor & Cursor::end(Stream & stream)
{
    if (*stream.command.push.pointer == 0) return *this;

    auto size = stream.command.size_actual();
    stream.output.push.ansi.cursor.move.right(size - stream.command.push.pointer.position());
    stream.command.push.pointer.position(size);

    return *this;
}

Cursor & Cursor::left(Stream & stream)
{
    if (stream.command.push.pointer.position() == 0) return *this;

    stream.command.push.pointer.move(-1);
    stream.output.push.ansi.cursor.move.left(1);  

    return *this;
}

Cursor & Cursor::right(Stream & stream)
{
    if (*stream.command.push.pointer == 0) return *this;

    stream.command.push.pointer.move(1);
    stream.output.push.ansi.cursor.move.right(1);

    return *this;
}


}; /* namespace: shell */
