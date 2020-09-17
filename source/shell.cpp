#include "shell.h"

Shell::Shell(Handler_flush flush) : _handler_flush(flush)
{

}

Shell::~Shell()
{

}

Shell & Shell::init()
{
    _buffer.input.ansi.reset().size(stream.size_max() + strlen(name) + 2, 100);

    _prompt();

    return *this;
}

Shell & Shell::reset()
{
    stream.reset();

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
            _capture.input.character(character, "");
            _mode = Mode::ESCAPE;
        }
    }
    else
    {
        _capture.input.character(character, "");

        for (int i = 0; i < sizeof(escape); i++)
        {
            if (tools::string::compare::difference(_capture.buffer, (char *) &escape[i][0]) != 0) continue;

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
            _capture.reset();
        }

        if (_capture.input.pointer.position() >= 6 && _mode == Mode::ESCAPE)
        {
            _mode = Mode::INPUT;
            _capture.reset();  
        }
    }
    
    _flush(_buffer);

    return result;
}

Shell & Shell::output(Stream * stream)
{
    _buffer.input.ansi.clear.line.entire();
    _buffer.input.ansi.special.r();

    _flush(_buffer);
    _flush(*stream, false);

    _buffer.input.ansi.special.r().n();
    _prompt();

    return *this;
}

/* ---------------------------------------------| info |--------------------------------------------- */

void Shell::_handler_tab()
{

}

void Shell::_handler_enter()
{
    _buffer.input.ansi.special.r().n();
}

void Shell::_handler_backspace()
{
    if (stream.input.pointer.position() == 0) return;

    auto offset = stream.input.offset_end();

    _buffer.input.ansi.cursor.move.left(1);
    _buffer.input.text(stream.input.pointer);
    _buffer.input.align_end();
    _buffer.input.word("");
    _buffer.input.ansi.cursor.move.left(offset + 1);

    stream.input.pointer--;
    tools::string::trim::left::characters(stream.input.pointer, 1);
}

void Shell::_handler_printable(char character)
{
    if (stream.size_actual() == stream.size_max() - 1) return;

    stream.input.character(character, "");
    _buffer.input.text(stream.input.pointer - 1, "");

    if (stream.input.is_aligned() == false) _buffer.input.ansi.cursor.move.left(stream.input.offset_end());
}

void Shell::_handler_home()
{
    if (stream.input.pointer.position() == 0) return;

    _buffer.input.ansi.cursor.move.left(stream.input.offset_home());
    stream.input.align_home();
}

void Shell::_handler_end()
{
    if (stream.input.is_aligned() == true) return;

    _buffer.input.ansi.cursor.move.right(stream.input.offset_end());
    stream.input.align_end();
}

void Shell::_handler_left()
{
    if (stream.input.pointer.position() == 0) return;

    stream.input.pointer--;
    _buffer.input.ansi.cursor.move.left(1);  
}

void Shell::_handler_right()
{
    if (stream.input.is_aligned() == true) return;

    stream.input.pointer++;
    _buffer.input.ansi.cursor.move.right(1);
}

void Shell::_handler_up()
{

}

void Shell::_handler_down()
{

}

void Shell::_handler_delete()
{
    if (stream.input.is_aligned() == true) return;

    auto size = stream.input.offset_end();

    _buffer.input.word(stream.input.pointer + 1);
    _buffer.input.align_end();
    _buffer.input.word("");
    _buffer.input.ansi.cursor.move.left(size + 1);

    tools::string::trim::left::characters(stream.input.pointer, 1);
}

void Shell::_handler_ctrl_left()
{
    if (stream.input.pointer.position() == 0) return;

    auto initial = stream.input.pointer.position();
    auto iterations = stream.input.offset_home();

    for (int i = 0; i < iterations; i++)
    {
        if (*stream.input.pointer != code_null && *stream.input.pointer != code_space)
        {
            if (*(stream.input.pointer - 1) == code_space && i > 0) break;
            else stream.input.pointer--;
        }
        else stream.input.pointer--;
    }

    auto shift = initial - stream.input.pointer.position();

    if (shift > 0) _buffer.input.ansi.cursor.move.left(shift);
}

void Shell::_handler_ctrl_right()
{
    if (stream.input.is_aligned() == true) return;

    auto initial = stream.input.pointer.position();
    auto iterations = stream.input.offset_end();

    for (int i = 0; i < iterations; i++)
    {
        if (*stream.input.pointer != code_null && *stream.input.pointer != code_space)
        {
            if (*(stream.input.pointer + 1) == code_null || *(stream.input.pointer + 1) == code_space)
            {
                if (i > 0)
                {
                    stream.input.pointer++;
                    break;
                }
            }
            else stream.input.pointer++;
        }
        else stream.input.pointer++;
    }

    auto shift = stream.input.pointer.position() - initial;

    if (shift > 0) _buffer.input.ansi.cursor.move.right(shift);
}

void Shell::_flush(Stream & stream, bool reset)
{
    auto size_actual = stream.size_actual();

    if (size_actual > 0) _handler_flush(stream.buffer, size_actual);
    if (reset == true) stream.reset();
}

void Shell::_prompt()
{
    _buffer.input.ansi.color.foreground(0, 255, 0);
    _buffer.input.format("%s: ", name);
    _buffer.input.ansi.color.foreground(255, 255, 255);


    _flush(_buffer);
    _flush(stream, false);

    _buffer.input.ansi.cursor.move.column(stream.input.pointer.position() + strlen(name) + 3);

    _flush(_buffer);
}