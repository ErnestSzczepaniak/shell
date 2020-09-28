#include "shell.h"

Shell::Shell(Handler_flush flush, Handler_execute execute) 
:
_handler_flush(flush),
_handler_execute(execute)
{

}

Shell::~Shell()
{

}

Shell & Shell::init()
{
    _system.output.push.ansi.reset().size(150, 50);

    _prompt();

    _flush_system();

    return *this;
}

Shell & Shell::input(char character)
{
    if (_mode == Mode::INPUT)
    {
        if (character == code_tab) _handler_tab();
        else if (character == code_enter) _handler_enter();
        else if (character == code_backspace) _handler_backspace();
        else if (character >= 32 && character < 127) _handler_printable(character);
        else if (character == code_escape)
        {
            _system.input.push.character(character, "");
            _mode = Mode::ESCAPE;
        }   
    }
    else
    {
        _system.input.push.character(character, "");

        for (int i = 0; i < sizeof(escape); i++)
        {
            if (tools::string::compare::difference(_system.input.buffer, (char *) &escape[i][0]) != 0) continue;

            if (i == 0) _handler_home();
            else if (i == 1) _handler_end();
            else if (i == 2) _handler_left();
            else if (i == 3) _handler_right();
            else if (i == 4) _handler_up();
            else if (i == 5) _handler_down();
            else if (i == 6) _handler_delete();
            else if (i == 7) _handler_ctrl_left();
            else if (i == 8) _handler_ctrl_right();

            _mode = Mode::INPUT;
            _system.input.clear();
        }

        if (_system.input.push.pointer.position() >= 6 && _mode == Mode::ESCAPE) // tutaj sie zastanowic
        {
            _mode = Mode::INPUT;
            _system.input.clear();
        }
    }
    
    _flush_system();

    return *this;
}

/* ---------------------------------------------| info |--------------------------------------------- */

void Shell::_handler_tab()
{
    if (_system.command.push.pointer.position() == 0) return;
    if (*_system.command.push.pointer != 0) return;

    auto count = _system.command.info.words();

    if (count == 1)
    {
        _system.command.push.pointer.save();

        _handler_ctrl_left(false);
        
        int match[size_program];

        for (int i = 0; i < _program_number; i++) match[i] = _complete_match(_system.command.push.pointer, _program[i].name());

        auto [max, pos] = _complete_max(match, _program_number);

        _system.command.push.pointer.restore();

        if (max == 0) return;
        else if (max < _system.command.push.pointer.position()) return;

        if (_complete_repetition(match, _program_number, max) == 1)
        {
            _system.command.push.format("%s ", _program[pos].name() + max);
            _system.output.push.format("%s ", _program[pos].name() + max);
        }
        else
        {
            for (int i = 0; i < _program_number; i++)
            {
                if (match[i] == max)
                {
                    _system.output.push.ansi.special.r().n();
                    _system.output.push.text(_program[i].name());
                }
            }   

            _system.output.push.ansi.special.r().n();
            _prompt();
        }
    }
    else if (count > 1)
    {

    }
}

void Shell::_handler_enter()
{
    _system.output.push.ansi.special.r().n();

    if (_system.command.push.pointer.position() == 0)
    {
        _prompt();
        return;
    }
    else _flush_system();

    auto count = _system.command.info.count(pipe);

    for (int i = 0; i < count + 1; i++)
    {
        auto * name = _system.command.pop.word();

        if (_program_known(name) == true)
        {
            _user.command.push.text(_system.command.pop.text(pipe_or_null), pipe_or_null);
            _system.command.pop.pointer.move(1);

            _handler_execute(name, _user);

            _user.command.clear();

            if (_user.error.push.pointer.position() > 0)
            {
                _user.error.push.pointer.reset();
                _user.error.push.format("%s: ", name);
                _user.error.push.pointer.position(_user.error.size_actual());
                break;
            }
            else if (i < count) _user.flush();
        }
        else
        {
            _user.error.push.text("Command '").text(name, " ").text("' not found ...");
            break;
        }
    }
    
    _user.input.clear();
    _user.command.clear();

    _system.command.clear();

    _flush_user();
    _prompt();
}

void Shell::_handler_backspace()
{
    if (_system.command.push.pointer.position() == 0) return;

    auto offset = _system.command.size_actual() - _system.command.push.pointer.position();

    _system.output.push.ansi.cursor.move.left(1);
    _system.output.push.text(_system.command.push.pointer);
    _system.output.push.pointer.position(_system.output.size_actual());
    _system.output.push.character(code_space, "");
    _system.output.push.ansi.cursor.move.left(offset + 1);

    _system.command.push.pointer.move(-1);
    tools::string::trim::left::characters(_system.command.push.pointer, 1);
}

void Shell::_handler_printable(char character)
{
    if (_system.command.is_full()) return;

    _system.command.push.character(character, "");
    _system.output.push.text(_system.command.push.pointer - 1);

    if (*_system.command.push.pointer != 0) _system.output.push.ansi.cursor.move.left(_system.command.size_actual() - _system.command.push.pointer.position());
}

void Shell::_handler_home()
{
    if (_system.command.push.pointer.position() == 0) return;

    _system.output.push.ansi.cursor.move.left(_system.command.push.pointer.position());
    _system.command.push.pointer.reset();
}

void Shell::_handler_end()
{
    if (*_system.command.push.pointer == 0) return;

    auto size = _system.command.size_actual();
    _system.output.push.ansi.cursor.move.right(size - _system.command.push.pointer.position());
    _system.command.push.pointer.position(size);
}

void Shell::_handler_left()
{
    if (_system.command.push.pointer.position() == 0) return;

    _system.command.push.pointer.move(-1);
    _system.output.push.ansi.cursor.move.left(1);  
}

void Shell::_handler_right()
{
    if (*_system.command.push.pointer == 0) return;

    _system.command.push.pointer.move(1);
    _system.output.push.ansi.cursor.move.right(1);
}

void Shell::_handler_up()
{

}

void Shell::_handler_down()
{

}

void Shell::_handler_delete()
{
    if (*_system.command.push.pointer == 0) return;

    auto offset = _system.command.size_actual() - _system.command.push.pointer.position();

    _system.output.push.text(_system.command.push.pointer + 1);
    _system.output.push.pointer.position(_system.output.size_actual());
    _system.output.push.character(code_space, "");
    _system.output.push.ansi.cursor.move.left(offset);

    tools::string::trim::left::characters(_system.command.push.pointer, 1);
}

void Shell::_handler_ctrl_left(bool output)
{
    if (_system.command.push.pointer.position() == 0) return;

    auto position = _system.command.push.pointer.position();

    for (int i = 0; i < position; i++)
    {
        if (*_system.command.push.pointer != code_null && *_system.command.push.pointer != code_space)
        {
            if (*(_system.command.push.pointer - 1) == code_space && i > 0) break;
            else _system.command.push.pointer.move(-1);
        }
        else _system.command.push.pointer.move(-1);
    }

    auto shift = position - _system.command.push.pointer.position();

    if (shift > 0 && output) _system.output.push.ansi.cursor.move.left(shift);
}

void Shell::_handler_ctrl_right(bool output)
{
    if (*_system.command.push.pointer == 0) return;

    auto initial = _system.command.push.pointer.position();
    auto iterations = _system.command.size_actual() - initial;

    for (int i = 0; i < iterations; i++)
    {
        if (*_system.command.push.pointer != code_null && *_system.command.push.pointer != code_space)
        {
            if (*(_system.command.push.pointer + 1) == code_null || *(_system.command.push.pointer + 1) == code_space)
            {
                if (i > 0)
                {
                    _system.command.push.pointer.move(1);
                    break;
                }
            }
            else _system.command.push.pointer.move(1);
        }
        else _system.command.push.pointer.move(1);
    }

    auto shift = _system.command.push.pointer.position() - initial;

    if (shift > 0 && output) _system.output.push.ansi.cursor.move.right(shift);
}

void Shell::_flush_system()
{
    if (_system.output.push.pointer.position() > 0)
    {
        _handler_flush(_system.output.buffer, _system.output.push.pointer.position());
        _system.output.clear();
    }
}

void Shell::_flush_user()
{
    if (_user.error.push.pointer.position() > 0)
    {
        _system.output.push.ansi.special.r().n();
        _flush_system();

        _user.error.push.ansi.special.r().n().r().n();

        _handler_flush(_user.error.buffer, _user.error.push.pointer.position());
        _user.error.clear();
    }
    else if (_user.output.push.pointer.position() > 0)
    {
        _handler_flush(_user.output.buffer, _user.output.push.pointer.position());
        _user.output.clear();

        _system.output.push.ansi.special.r().n();
    }
}

void Shell::_prompt()
{
    _system.output.push.ansi.color.foreground(0, 255, 0);
    _system.output.push.format("%s: ", name);
    _system.output.push.ansi.color.foreground(255, 255, 255);
    _system.output.push.text(_system.command.buffer);
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
