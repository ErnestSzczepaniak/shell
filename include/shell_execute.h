#ifndef _shell_execute_h
#define _shell_execute_h

/**
 * @file	shell_execute.h
 * @author	en2
 * @date	28-09-2020
 * @brief	
 * @details	
**/

#include "shell_type.h"

namespace shell
{

class Execute
{
    static constexpr char * pipe =              "|";
    static constexpr char * pipe_or_null =      "|\0";

public:
    Execute(Stream & stream, Handler_call call);
    ~Execute();

    Execute & enter();

protected:
    Handler_call _handler_call;

private:
    Stream & stream;

}; /* class: Execute */


}; /* namespace: shell */


#endif /* define: shell_execute_h */