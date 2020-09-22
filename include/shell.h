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

class Shell
{
    const char * name =                         "user@mouse";

    static constexpr auto code_null =           0;
    static constexpr auto code_tab =            9;
    // static constexpr auto code_enter =          13; //10 for host
    static constexpr auto code_enter =          10; //10 for host
    
    static constexpr auto code_escape =         27;
    static constexpr auto code_space =          32;
    // static constexpr auto code_backspace =      8; //127 for host
    static constexpr auto code_backspace =      127; //127 for host

    using Handler_flush = void (*)(char * buffer, int size);
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
    enum class Event : unsigned char
    {
        HOME,
        END,
        LEFT,
        RIGHT,
        UP,
        DOWN,
        DELETE,
        CTRL_LEFT,
        CTRL_RIGHT,
        TAB,
        ENTER,
        BACKSPACE,
        PRINTABLE,
        NONE,
        FULL
    }; /* enum: Event */

    Shell(Handler_flush flush);
    ~Shell();

    Shell & reset();
    Shell & init();
    Event input(char character);
    Shell & output(stream::Channel & channel);

    Stream stream;

protected:
    void _handler_tab();
    void _handler_enter();
    void _handler_backspace();
    void _handler_printable(char character);
    void _handler_home();
    void _handler_end();
    void _handler_left();
    void _handler_right();
    void _handler_up();
    void _handler_down();
    void _handler_delete();
    void _handler_ctrl_left();
    void _handler_ctrl_right();

    void _flush(stream::Channel & channel, bool reset = true);
    void _prompt();

private:
    Mode _mode = Mode::INPUT;
    Handler_flush _handler_flush = nullptr;

}; /* class: Shell */


#endif /* define: shell_h */