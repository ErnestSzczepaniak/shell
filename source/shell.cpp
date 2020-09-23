#include "shell.h"

Shell::Shell(Handler_flush flush) : _handler_flush(flush)
{

}

Shell::~Shell()
{

}

Shell & Shell::init()
{
    stream.output.set.ansi.reset().size(150, 50);

    _prompt();

    return *this;
}

Shell & Shell::reset(bool newline)
{
    stream.reset();

    if (newline) stream.output.set.ansi.special.r().n();

    _prompt();

    return *this;
}

Shell::Event Shell::input(char character)
{
    auto result = Event::NONE;

    if (_mode == Mode::INPUT)
    {
        if (character == code_tab)
        {
            _handler_tab();
            result = Event::TAB;
        }
        else if (character == code_enter)
        {
            _handler_enter();
            result = Event::ENTER;
        }
        else if (character == code_backspace)
        {
            _handler_backspace();
            result = Event::BACKSPACE;
        }
        else if (character >= 32 && character < 127)
        {   
            _handler_printable(character);
            result = Event::PRINTABLE;
        }
        else if (character == code_escape)
        {
            stream.input.set.character(character, "");
            _mode = Mode::ESCAPE;
        }
    }
    else
    {
        stream.input.set.character(character, "");

        for (int i = 0; i < sizeof(escape); i++)
        {
            if (tools::string::compare::difference(stream.input.buffer, (char *) &escape[i][0]) != 0) continue;

            result = (Event) i;

            if (result == Event::HOME) _handler_home();
            else if (result == Event::END) _handler_end();
            else if (result == Event::LEFT) _handler_left();
            else if (result == Event::RIGHT) _handler_right();
            else if (result == Event::UP) _handler_up();
            else if (result == Event::DOWN) _handler_down();
            else if (result == Event::DELETE) _handler_delete();
            else if (result == Event::CTRL_LEFT) _handler_ctrl_left();
            else if (result == Event::CTRL_RIGHT) _handler_ctrl_right();

            _mode = Mode::INPUT;
            stream.input.reset();
        }

        if (stream.input.set.pointer.position() >= 6 && _mode == Mode::ESCAPE)
        {
            _mode = Mode::INPUT;
            stream.input.reset();
        }
    }
    
    _flush(stream.output);

    return result;
}

Shell & Shell::output(stream::Channel & channel)
{
    if (channel.size_actual() == 0) return *this;

    stream.output.set.ansi.clear.line.entire();
    stream.output.set.ansi.special.r();

    _flush(stream.output);
    _flush(channel, false);

    stream.output.set.ansi.special.r().n();
    _prompt();

    return *this;
}

/* ---------------------------------------------| info |--------------------------------------------- */

void Shell::_handler_tab()
{

}

void Shell::_handler_enter()
{
    stream.output.set.ansi.special.r().n();
}

void Shell::_handler_backspace()
{
    if (stream.command.set.pointer.position() == 0) return;

    auto offset = stream.command.set.pointer.offset_end();

    stream.output.set.ansi.cursor.move.left(1);
    stream.output.set.text(stream.command.set.pointer);
    stream.output.set.pointer.align_end();
    stream.output.set.character(code_space, "");
    stream.output.set.ansi.cursor.move.left(offset + 1);

    stream.command.set.pointer--;
    tools::string::trim::left::characters(stream.command.set.pointer, 1);
}

void Shell::_handler_printable(char character)
{
    if (stream.command.size_actual() == stream.command.size_max() - 1) return;

    stream.command.set.character(character, "");
    stream.output.set.text(stream.command.set.pointer - 1);

    if (stream.command.set.pointer.is_aligned() == false) stream.output.set.ansi.cursor.move.left(stream.input.set.pointer.offset_end());
}

void Shell::_handler_home()
{
    if (stream.command.set.pointer.position() == 0) return;

    stream.output.set.ansi.cursor.move.left(stream.command.set.pointer.position());
    stream.command.set.pointer.reset();
}

void Shell::_handler_end()
{
    if (stream.command.set.pointer.is_aligned() == true) return;

    stream.output.set.ansi.cursor.move.right(stream.command.set.pointer.offset_end());
    stream.command.set.pointer.align_end();
}

void Shell::_handler_left()
{
    if (stream.command.set.pointer.position() == 0) return;

    stream.command.set.pointer--;
    stream.output.set.ansi.cursor.move.left(1);  
}

void Shell::_handler_right()
{
    if (stream.command.set.pointer.is_aligned() == true) return;

    stream.command.set.pointer++;
    stream.output.set.ansi.cursor.move.right(1);
}

void Shell::_handler_up()
{

}

void Shell::_handler_down()
{

}

void Shell::_handler_delete()
{
    if (stream.command.set.pointer.is_aligned() == true) return;

    auto size = stream.command.set.pointer.offset_end();

    stream.output.set.text(stream.command.set.pointer + 1);
    stream.output.set.pointer.align_end();
    stream.output.set.character(code_space, "");
    stream.output.set.ansi.cursor.move.left(size);

    tools::string::trim::left::characters(stream.command.set.pointer, 1);
}

void Shell::_handler_ctrl_left()
{
    // if (stream.input.pointer.position() == 0) return;

    // auto initial = stream.input.pointer.position();
    // auto iterations = stream.input.offset_home();

    // for (int i = 0; i < iterations; i++)
    // {
    //     if (*stream.input.pointer != code_null && *stream.input.pointer != code_space)
    //     {
    //         if (*(stream.input.pointer - 1) == code_space && i > 0) break;
    //         else stream.input.pointer--;
    //     }
    //     else stream.input.pointer--;
    // }

    // auto shift = initial - stream.input.pointer.position();

    // if (shift > 0) stream.output.set.ansi.cursor.move.left(shift);
}

void Shell::_handler_ctrl_right()
{
    // if (stream.input.is_aligned() == true) return;

    // auto initial = stream.input.pointer.position();
    // auto iterations = stream.input.offset_end();

    // for (int i = 0; i < iterations; i++)
    // {
    //     if (*stream.input.pointer != code_null && *stream.input.pointer != code_space)
    //     {
    //         if (*(stream.input.pointer + 1) == code_null || *(stream.input.pointer + 1) == code_space)
    //         {
    //             if (i > 0)
    //             {
    //                 stream.input.pointer++;
    //                 break;
    //             }
    //         }
    //         else stream.input.pointer++;
    //     }
    //     else stream.input.pointer++;
    // }

    // auto shift = stream.input.pointer.position() - initial;

    // if (shift > 0) stream.output.set.ansi.cursor.move.right(shift);
}

void Shell::_flush(stream::Channel & channel, bool reset)
{
    auto size_actual = channel.size_actual();

    if (size_actual > 0) _handler_flush(channel.buffer, size_actual);
    if (reset == true) channel.reset();
}

void Shell::_prompt()
{
    stream.command.reset();

    stream.output.set.ansi.color.foreground(0, 255, 0);
    stream.output.set.format("%s: ", name);
    stream.output.set.ansi.color.foreground(255, 255, 255);

    _flush(stream.output);
}