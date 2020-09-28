#include "shell_editor.h"

namespace shell
{

Editor::Editor(Stream & stream) : stream(stream)
{

}

Editor::~Editor()
{

}

Editor & Editor::backspace()
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

Editor & Editor::del()
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

Editor & Editor::print(char character)
{

if (stream.command.is_full()) return *this;

    stream.command.push.character(character, "");
    stream.output.push.text(stream.command.push.pointer - 1);

    if (*stream.command.push.pointer != 0) stream.output.push.ansi.cursor.move.left(stream.command.size_actual() - stream.command.push.pointer.position());

    return *this;
}

Editor & Editor::home()
{
    if (stream.command.push.pointer.position() == 0) return *this;

    stream.output.push.ansi.cursor.move.left(stream.command.push.pointer.position());
    stream.command.push.pointer.reset();

    return *this;
}

Editor & Editor::end()
{
    if (*stream.command.push.pointer == 0) return *this;

    auto size = stream.command.size_actual();
    stream.output.push.ansi.cursor.move.right(size - stream.command.push.pointer.position());
    stream.command.push.pointer.position(size);

    return *this;
}

Editor & Editor::left()
{
    if (stream.command.push.pointer.position() == 0) return *this;

    stream.command.push.pointer.move(-1);
    stream.output.push.ansi.cursor.move.left(1);  

    return *this;
}

Editor & Editor::right()
{
    if (*stream.command.push.pointer == 0) return *this;

    stream.command.push.pointer.move(1);
    stream.output.push.ansi.cursor.move.right(1);

    return *this;
}

Editor & Editor::ctrl_left(bool output)
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

Editor & Editor::ctrl_right(bool output)
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
