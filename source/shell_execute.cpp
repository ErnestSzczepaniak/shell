#include "shell_execute.h"

namespace shell
{

Execute::Execute(Handler_call call) : _handler_call(call)
{

}

Execute::~Execute()
{

}

bool Execute::enter(Stream & stream)
{
    if (stream.command.size_actual() == 0) return false;

    auto temp = stream.command;
    auto count = temp.info.count(pipe);

    for (int i = 0; i < count + 1; i++)
    {
        auto * name = temp.pop.word();

        stream.command.clear();

        stream.command.push.text(temp.pop.text(pipe_or_null), pipe_or_null);
        stream.error.push.text("Command '").text(name, " ").text("': ");

        auto position = stream.error.push.pointer.position();

        if (auto result = _handler_call(name, stream); result == true)
        {
            temp.pop.pointer.move(1); 

            if (stream.error.push.pointer.position() > position) break;
            else if (i < count) stream.flush();

            stream.error.clear();
            continue;
        }

        stream.error.push.text("not found ...");
    }

    stream.input.clear();
    stream.command.clear();
    
    return true;
}


}; /* namespace: shell */
