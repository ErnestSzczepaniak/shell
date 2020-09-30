#ifndef _shell_ctrl_h
#define _shell_ctrl_h

/**
 * @file	shell_ctrl.h
 * @author	en2
 * @date	29-09-2020
 * @brief	
 * @details	
**/

#include "stream.h"

namespace shell
{

class Ctrl
{
    static constexpr auto code_null =           0;
    static constexpr auto code_space =          32;

public:
    Ctrl & left(Stream & stream);
    Ctrl & right(Stream & stream);

}; /* class: Ctrl */


}; /* namespace: shell */


#endif /* define: shell_ctrl_h */