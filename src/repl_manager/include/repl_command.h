#ifndef REPL_COMMAND_H
#define REPL_COMMAND_H

#include <string>
#include <iostream>

class ReplCommand {
    public:
    ReplCommand() = default;
    ~ReplCommand() = default;
    virtual std::string GetCommand() const {
        return {"Invalid Command"};
    }
    virtual std::string GetHelpText() const {
        return {"Default Help"};
    }
    virtual void Execute(std::string argument) const {
        std::cout << "Invalid command. Type 'help' to see available commands." << std::endl;
    }
   /* std::ostream& operator<<(std::ostream& os) const {
        os << this->GetCommand() << " -- " << this->GetHelpText() << std::endl;
        return os;
    }*/

};

#endif // !REPL_COMMAND_H
