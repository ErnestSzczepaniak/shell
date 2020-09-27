#include "shell.h"

Shell::Shell(Handler_flush flush, Handler_execute execute, char ** table, int number) 
:
_handler_flush(flush),
_handler_execute(execute),
_program_table(table),
_program_number(number)
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

            if (_user.error.push.pointer.position() > 0) break;
            else if (i < count) _user.flush();
        }
        else
        {
            _user.error.push.text("Command '").text(name, " ").text("' not found...");
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

void Shell::_handler_ctrl_left()
{
    // if (_system.input.pointer.position() == 0) return;

    // auto initial = _system.input.pointer.position();
    // auto iterations = _system.input.offset_home();

    // for (int i = 0; i < iterations; i++)
    // {
    //     if (*_system.input.pointer != code_null && *_system.input.pointer != code_space)
    //     {
    //         if (*(_system.input.pointer - 1) == code_space && i > 0) break;
    //         else _system.input.pointer--;
    //     }
    //     else _system.input.pointer--;
    // }

    // auto shift = initial - _system.input.pointer.position();

    // if (shift > 0) _system.output.set.ansi.cursor.move.left(shift);
}

void Shell::_handler_ctrl_right()
{
    // if (_system.input.is_aligned() == true) return;

    // auto initial = _system.input.pointer.position();
    // auto iterations = _system.input.offset_end();

    // for (int i = 0; i < iterations; i++)
    // {
    //     if (*_system.input.pointer != code_null && *_system.input.pointer != code_space)
    //     {
    //         if (*(_system.input.pointer + 1) == code_null || *(_system.input.pointer + 1) == code_space)
    //         {
    //             if (i > 0)
    //             {
    //                 _system.input.pointer++;
    //                 break;
    //             }
    //         }
    //         else _system.input.pointer++;
    //     }
    //     else _system.input.pointer++;
    // }

    // auto shift = _system.input.pointer.position() - initial;

    // if (shift > 0) _system.output.set.ansi.cursor.move.right(shift);
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
}

bool Shell::_program_known(char * name)
{
    for (int i = 0; i < _program_number; i++)
    {
        if (tools::string::compare::equality(name, _program_table[i], " ")) return true;
    }
    
    return false;
}