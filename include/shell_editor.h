#ifndef _shell_editor_h
#define _shell_editor_h

/**
 * @file	shell_editor.h
 * @author	en2
 * @date	28-09-2020
 * @brief	
 * @details	
**/

#include "stream.h"

namespace shell
{

class Editor
{
    static constexpr auto code_null =           0;
    static constexpr auto code_space =          32;

public:
    Editor(Stream & stream);
    ~Editor();

    Editor & backspace();
    Editor & print(char character);
    Editor & home();
    Editor & end();
    Editor & left();
    Editor & right();
    Editor & ctrl_left(bool output = true);
    Editor & ctrl_right(bool output = true);
    Editor & del();

private:
    Stream & stream;    

}; /* class: Editor */


}; /* namespace: shell */


#endif /* define: shell_editor_h */