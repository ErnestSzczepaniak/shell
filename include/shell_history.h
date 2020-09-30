#ifndef _shell_history_h
#define _shell_history_h

/**
 * @file	shell_history.h
 * @author	en2
 * @date	28-09-2020
 * @brief	
 * @details	
**/

#include "stream.h"

namespace shell
{

class History
{
public:
    History & up(Stream & stream);
    History & down(Stream & stream);

}; /* class: History */


}; /* namespace: shell */


#endif /* define: shell_history_h */