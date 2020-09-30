#ifndef _shell_hint_h
#define _shell_hint_h

/**
 * @file	shell_hint.h
 * @author	en2
 * @date	29-09-2020
 * @brief	
 * @details	
**/

#include "stream.h"
#include "shell_type.h"

namespace shell
{

class Hint
{
    static constexpr auto size = 128;
    static constexpr auto code_space = 32;
    static constexpr auto code_pipe = 124;
    struct Result_max {int value; int pos; bool duplicate;};

public:
    Hint(Handler_program program, Handler_keyword keyword);
    ~Hint();  

    bool tab(Stream & stream);

protected:
    int _program_count();
    int _keyword_count(char * program);

    char * _program_find(Stream & stream);
    char * _program_ready(int count, char * program);
    void _program_calculate(int count, char * program);

    char * _keyword_find(Stream & stream);

    int _match_calculte(char * hint, char * word);

    void _keyword_calculate(int count, char * program, char * keyword);

    Result_max _find_max(int count);

private:
    Handler_program _handler_program;
    Handler_keyword _handler_keyword;

    int _match[size];

}; /* class: Hint */

}; /* namespace: shell */

#endif /* define: shell_hint_h */