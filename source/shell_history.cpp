#include "shell_history.h"

namespace shell
{

History::History(Stream & stream) : stream(stream)
{

}

History::~History()
{

}

History & History::up()
{
    return *this;
}

History & History::down()
{
    return *this;
}


}; /* namespace: shell */
