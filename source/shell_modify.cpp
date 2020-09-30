#include "shell_modify.h"

namespace shell
{

Modify & Modify::backspace(Stream & stream)
{
    if (stream.command.push.pointer.position() == 0) return *this;

    auto offset = stream.command.size_actual() - stream.command.push.pointer.position();

    stream.output.push.ansi.cursor.move.left(1);
    stream.output.push.text(stream.command.push.pointer);
    stream.output.push.pointer.position(stream.output.size_actual());
    stream.output.push.character(code_space, "");
    stream.output.push.ansi.cursor.move.left(offset + 1);

    stream.command.push.pointer.move(-1);
    tools::string::trim::left::characters(stream.command.push.pointer, 1);

    return *this;
}

Modify & Modify::del(Stream & stream)
{
    if (*stream.command.push.pointer == 0) return *this;

    auto offset = stream.command.size_actual() - stream.command.push.pointer.position();

    stream.output.push.text(stream.command.push.pointer + 1);
    stream.output.push.pointer.position(stream.output.size_actual());
    stream.output.push.character(code_space, "");
    stream.output.push.ansi.cursor.move.left(offset);

    tools::string::trim::left::characters(stream.command.push.pointer, 1);
    
    return *this;
}

Modify & Modify::print(Stream & stream, char character)
{
    if (stream.command.is_full()) return *this;

    stream.command.push.character(character, "");
    stream.output.push.text(stream.command.push.pointer - 1);

    if (*stream.command.push.pointer != 0) stream.output.push.ansi.cursor.move.left(stream.command.size_actual() - stream.command.push.pointer.position());

    return *this;
}

Modify & Modify::clear(Stream & stream)
{
    if (stream.command.size_actual() == 0) return *this;

    if (stream.command.push.pointer.position() != 0) stream.output.push.ansi.cursor.move.left(stream.command.push.pointer.position());
    stream.output.push.ansi.clear.line.right();

    stream.command.clear();

    return *this;
}

}; /* namespace: shell */
