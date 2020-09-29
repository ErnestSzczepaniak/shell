#include "shell_modify.h"

namespace shell
{

Modify::Modify(Stream & stream) : stream(stream)
{

}

Modify::~Modify()
{

}

Modify & Modify::backspace()
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

Modify & Modify::del()
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

Modify & Modify::print(char character)
{

if (stream.command.is_full()) return *this;

    stream.command.push.character(character, "");
    stream.output.push.text(stream.command.push.pointer - 1);

    if (*stream.command.push.pointer != 0) stream.output.push.ansi.cursor.move.left(stream.command.size_actual() - stream.command.push.pointer.position());

    return *this;
}

}; /* namespace: shell */
