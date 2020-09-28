#ifndef _shell_type_h
#define _shell_type_h

/**
 * @file	shell_type.h
 * @author	en2
 * @date	28-09-2020
 * @brief	
 * @details	
**/

#include "stream.h"

using Handler_flush = void (*)(char * buffer, int size);
using Handler_call = bool (*)(char *, Stream &);

#endif /* define: shell_type_h */