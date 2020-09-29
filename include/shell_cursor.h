#ifndef _shell_cursor_h
#define _shell_cursor_h

/**
 * @file	shell_cursor.h
 * @author	en2
 * @date	29-09-2020
 * @brief	
 * @details	
**/

#include "stream.h"

namespace shell
{

class Cursor
{
public:
    Cursor(Stream & stream);
    ~Cursor();

    Cursor & home(bool output = true);
    Cursor & end(bool output = true);
    Cursor & left(bool output = true);
    Cursor & right(bool output = true); 

private:
    Stream & stream;

}; /* class: Cursor */


}; /* namespace: shell */


#endif /* define: shell_cursor_h */