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

using Handler_flush = void (*)(char *, int);
using Handler_call = bool (*)(char *, Stream &);
using Handler_program = char * (*)(int);
using Handler_keyword = char * (*)(char *, int);

#endif /* define: shell_type_h */