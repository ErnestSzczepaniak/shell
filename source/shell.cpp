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

    _prompt();

    _flush();

    return *this;
}

Shell & Shell::input(char character)
{
    if (_mode == Mode::INPUT)
    {
        if (character == code_tab)
        {
            auto result = _hint.tab(_stream);

            if (result == false) _prompt();
        } 
        else if (character == code_enter)
        {
            _execute.enter(_stream);
            _prompt();
        }
        else if (character == code_backspace) _editor.backspace(_stream);
        else if (character >= 32 && character < 127) _editor.print(_stream, character);
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
            else if (i == 6) _editor.del(_stream);
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
    
    _flush();

    return *this;
}

/* ---------------------------------------------| info |--------------------------------------------- */

void Shell::_flush()
{
    if (_stream.error.push.pointer.position() > 0)
    {
        _stream.error.push.pointer.reset();
        _stream.error.push.ansi.special.r().n().r().n();
        _stream.error.push.pointer.position(_stream.error.size_actual());
        _stream.error.push.ansi.special.r().n().r().n();

        _handler_flush(_stream.error.buffer, _stream.error.push.pointer.position());
        _stream.error.clear();
    }

    if (_stream.output.push.pointer.position() > 0)
    {
        _handler_flush(_stream.output.buffer, _stream.output.push.pointer.position());
        _stream.output.clear();
    }
}

void Shell::_prompt()
{
    _stream.output.push.ansi.color.foreground(0, 255, 0);
    _stream.output.push.format("%s: ", name);
    _stream.output.push.ansi.color.foreground(255, 255, 255);
    _stream.output.push.text(_stream.command.buffer);
}
