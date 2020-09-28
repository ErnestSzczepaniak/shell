#ifndef _shell_program_h
#define _shell_program_h

/**
 * @file	shell_program.h
 * @author	en2
 * @date	28-09-2020
 * @brief	
 * @details	
**/

namespace shell
{

class Program
{
public:
    Program();
    ~Program();

    Program & name(char * name);
    char * name();

    Program & keywords(char ** keywords);
    char ** keywords();
    char * keyword(int index);

    Program & size_keywords(int value);
    int size_keywords();

protected:
    char * _name = nullptr;
    char ** _keywods = nullptr;
    int _size = 0;

}; /* class: Program */


}; /* namespace: shell */


#endif /* define: shell_program_h */