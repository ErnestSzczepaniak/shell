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
    Cursor & home(Stream & stream);
    Cursor & end(Stream & stream);
    Cursor & left(Stream & stream);
    Cursor & right(Stream & stream);

}; /* class: Cursor */


}; /* namespace: shell */


#endif /* define: shell_cursor_h */