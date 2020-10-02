#include "shell.h"

Shell::Shell(Handler_flush flush, Handler_call call, Handler_program program, Handler_keyword keyword) 
:
_handler_flush(flush),
_execute(call),
_hint(program, keyword)
{

}

Shell::~Shell()
{

}

Shell & Shell::init()
{
    _stream.output.push.ansi.reset().size(150, 50);

    _flush();

    _prompt();

    return *this;
}

Shell & Shell::input(char character)
{
    auto flush_newline = false;
    auto prompt_newline = false;

    if (_mode == Mode::INPUT)
    {
        if (character == code_backspace) _modify.backspace(_stream);
        else if (character == code_clear) _modify.clear(_stream);
        else if (character >= 32 && character < 127) _modify.print(_stream, character);
        if (character == code_tab)
        {
            prompt_newline = _hint.tab(_stream); // false - nieudalo sie
        } 
        else if (character == code_enter)
        {
            flush_newline = _execute.enter(_stream); // naweline after output - empty or error
            prompt_newline = true;

        }
        else if (character == code_escape)
        {
            _stream.input.push.character(character, "");
            _mode = Mode::ESCAPE;
        }   
    }
    else
    {
        _stream.input.push.character(character, "");

        for (int i = 0; i < sizeof(escape); i++)
        {
            if (tools::string::compare::difference(_stream.input.buffer, (char *) &escape[i][0]) != 0) continue;

            if (i == 0) _cursor.home(_stream);
            else if (i == 1) _cursor.end(_stream);
            else if (i == 2) _cursor.left(_stream);
            else if (i == 3) _cursor.right(_stream);
            else if (i == 4) _history.up(_stream);
            else if (i == 5) _history.down(_stream);
            else if (i == 6) _modify.del(_stream);
            else if (i == 7) _ctrl.left(_stream);
            else if (i == 8) _ctrl.right(_stream);

            _mode = Mode::INPUT;
            _stream.input.clear();
        }

        if (_stream.input.push.pointer.position() >= 6 && _mode == Mode::ESCAPE) // tutaj sie zastanowic
        {
            _mode = Mode::INPUT;
            _stream.input.clear();
        }
    }
    
    _flush(flush_newline);

    if (prompt_newline) _prompt(true);

    return *this;
}

Shell & Shell::output(char * data)
{
    _clear();
    _handler_flush(data, strlen(data));
    _prompt(true);

    return *this;
}

Shell & Shell::execute(char * data)
{
    auto temp = _stream.command;

    _stream.command.clear();
    _stream.command.push.text(data);

    _execute.enter(_stream);

    _clear();

    _flush(false);

    _stream.command = temp;

    _prompt(true);

    return *this;
}

/* ---------------------------------------------| info |--------------------------------------------- */

void Shell::_clear()
{
    char * data = "\e[2K\e[0G";
    _handler_flush(data, strlen(data));
}

void Shell::_newline()
{
    char * data = "\r\n";
    _handler_flush(data, strlen(data));
}

void Shell::_prompt(bool newline)
{
    if (newline) _newline();

    char * data = "\e[38;2;0;255;0men2@mouse: \e[38;2;255;255;255m";
    _handler_flush(data, strlen(data));

    auto size = _stream.command.size_actual();
    _handler_flush(_stream.command.buffer, size);

    auto offset = size - _stream.command.push.pointer.position();

    if (offset > 0)
    {
        _stream.output.push.ansi.cursor.move.left(offset);
        _flush(false);
    }
}

void Shell::_flush(bool newline)
{
    if (newline) _newline();

    if (_stream.error.push.pointer.position() > 0)
    {
        _handler_flush(_stream.error.buffer, _stream.error.push.pointer.position());
        _stream.error.clear();
    }
    else if (_stream.output.push.pointer.position() > 0)
    {
        _handler_flush(_stream.output.buffer, _stream.output.push.pointer.position());
        _stream.output.clear();
    }
}
