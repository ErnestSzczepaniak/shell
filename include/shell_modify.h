#ifndef _shell_modify_h
#define _shell_modify_h

/**
 * @file	shell_modify.h
 * @author	en2
 * @date	28-09-2020
 * @brief	
 * @details	
**/

#include "stream.h"

namespace shell
{

class Modify
{
    static constexpr auto code_null =           0;
    static constexpr auto code_space =          32;

public:
    Modify(Stream & stream);
    ~Modify();

    Modify & backspace();
    Modify & print(char character);
    Modify & del();

private:
    Stream & stream;    

}; /* class: Modify */


}; /* namespace: shell */


#endif /* define: shell_modify_h */