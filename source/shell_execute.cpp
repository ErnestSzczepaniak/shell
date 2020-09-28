#include "shell_execute.h"

namespace shell
{

Execute::Execute(Stream & stream, Handler_call call) : stream(stream), _handler_call(call)
{

}

Execute::~Execute()
{

}

Execute & Execute::enter()
{
    if (stream.command.size_actual() == 0)
    {
        stream.output.push.ansi.special.r().n();
        return *this;
    } 

    stream::Stack<128> temp;
    temp = stream.command;
    stream.command.clear();

    auto count = temp.info.count(pipe);

    for (int i = 0; i < count + 1; i++)
    {
        auto * name = temp.pop.word();

        stream.command.push.text(temp.pop.text(pipe_or_null), pipe_or_null);

        if (auto result = _handler_call(name, stream); result == true)
        {
            temp.pop.pointer.move(1); 
            stream.command.clear();

            if (stream.error.push.pointer.position() > 0)
            {
                stream.error.push.pointer.reset();
                stream.error.push.format("%s: ", name);
                stream.error.push.pointer.position(stream.error.size_actual());
                return *this;
            }
            else if (i < count) stream.flush();
        }
        else
        {
            stream.error.push.text("Command '").text(name, " ").text("' not found ...");
            return *this;
        }
    }

    if (stream.output.push.pointer.position() > 0)
    {
        stream.output.push.ansi.special.r().n(); // always newline
        stream.output.push.pointer.reset();
        stream.output.push.ansi.special.r().n();
        stream.output.push.pointer.position(stream.output.size_actual());
    }

    return *this;
}


}; /* namespace: shell */
