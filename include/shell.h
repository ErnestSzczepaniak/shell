#ifndef _shell_h
#define _shell_h

/**
 * @file	shell.h
 * @author	en2
 * @date	14-09-2020
 * @brief	
 * @details	
**/

#include "stream.h"
#include "shell_modify.h"
#include "shell_history.h"
#include "shell_execute.h"
#include "shell_ctrl.h"
#include "shell_cursor.h"
#include "shell_hint.h"

class Shell
{
    const char * name =                         "user@mouse";

    static constexpr auto code_tab =            9;
    static constexpr auto code_enter =          13; //10 for host
    //static constexpr auto code_enter =          10; //10 for host
    static constexpr auto code_escape =         27;
    static constexpr auto code_backspace =      8; //127 for host
    //static constexpr auto code_backspace =      127; //127 for host

    static constexpr auto size_program =        128;   
    enum class Mode {INPUT, ESCAPE};

    static constexpr char escape[][8] = 
    {
        {27,  91,  72,  0,   0,    0,   0}, // home
        {27,  91,  70,  0,   0,    0,   0}, // end
        {27,  91,  68,  0,   0,    0,   0}, // left
        {27,  91,  67,  0,   0,    0,   0}, // right
        {27,  91,  65,  0,   0,    0,   0}, // up
        {27,  91,  66,  0,   0,    0,   0}, // down
        {27,  91,  51,  126, 0,    0,   0}, // delete
        {27,  91,  49,  59,  53,   68,  0}, // ctrl_left
        {27,  91,  49,  59,  53,   67,  0}  // ctrl_right
    };

public:
    Shell(Handler_flush flush, Handler_call call, Handler_program program, Handler_keyword keyword);
    ~Shell();

    Shell & init();
    Shell & input(char character);

protected:
    void _flush();
    void _prompt();

private:
    Stream _stream;

    shell::Modify _editor;
    shell::History _history;
    shell::Execute _execute;
    shell::Ctrl _ctrl;
    shell::Cursor _cursor;
    shell::Hint _hint;

    Mode _mode = Mode::INPUT;
    Handler_flush _handler_flush = nullptr;

}; /* class: Shell */

#endif /* define: shell_h */