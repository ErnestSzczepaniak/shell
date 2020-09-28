#include "shell.h"

Shell::Shell(Handler_flush flush, Handler_call call) 
:
_handler_flush(flush),
_editor(_stream),
_history(_stream),
_execute(_stream, call)
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
        if (character == code_tab) _handler_tab();
        else if (character == code_enter)
        {
            _execute.enter();
            _prompt();
        }
        else if (character == code_backspace) _editor.backspace();
        else if (character >= 32 && character < 127) _editor.print(character);
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

            if (i == 0) _editor.home();
            else if (i == 1) _editor.end();
            else if (i == 2) _editor.left();
            else if (i == 3) _editor.right();
            else if (i == 4) _history.up();
            else if (i == 5) _history.down();
            else if (i == 6) _editor.del();
            else if (i == 7) _editor.ctrl_left();
            else if (i == 8) _editor.ctrl_right();

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

void Shell::_handler_tab()
{
    if (_stream.command.push.pointer.position() == 0) return;
    if (*_stream.command.push.pointer != 0) return;

    auto count = _stream.command.info.words();

    if (count == 1)
    {
        _stream.command.push.pointer.save();

        //_handler_ctrl_left(false);
        
        int match[size_program];

        for (int i = 0; i < _program_number; i++) match[i] = _complete_match(_stream.command.push.pointer, _program[i].name());

        auto [max, pos] = _complete_max(match, _program_number);

        _stream.command.push.pointer.restore();

        if (max == 0) return;
        else if (max < _stream.command.push.pointer.position()) return;

        if (_complete_repetition(match, _program_number, max) == 1)
        {
            _stream.command.push.format("%s ", _program[pos].name() + max);
            _stream.output.push.format("%s ", _program[pos].name() + max);
        }
        else
        {
            for (int i = 0; i < _program_number; i++)
            {
                if (match[i] == max)
                {
                    _stream.output.push.ansi.special.r().n();
                    _stream.output.push.text(_program[i].name());
                }
            }   

            _stream.output.push.ansi.special.r().n();
            _prompt();
        }
    }
    else if (count > 1)
    {

    }
}

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

bool Shell::_program_known(char * name)
{
    for (int i = 0; i < _program_number; i++)
    {
        if (tools::string::compare::equality(name, _program[i].name(), " ")) return true;
    }
    
    return false;
}

int Shell::_complete_match(char * hint, char * element)
{
    auto size = strlen(element);
    auto count = 0;

    for (int i = 0; i < size; i++)
    {
        if (hint[i] == element[i]) count++;
        else break;
    }
    return count;
}

Shell::Max Shell::_complete_max(int * match, int size)
{
    auto max = 0;
    auto pos = 0;

    for (int i = 0; i < size; i++)
    {
        if (match[i] > max)
        {
            max = match[i];
            pos = i;
        }
    }
    return {max, pos};
}

int Shell::_complete_repetition(int * match, int size, int max)
{
    auto count = 0;

    for (int i = 0; i < size; i++)
    {
        if (match[i] == max) count++;
    }
    return count;
}
