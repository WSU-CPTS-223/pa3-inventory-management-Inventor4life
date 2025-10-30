#ifndef REPL_MANAGER_H
#define REPL_MANAGER_H

#include "repl_command.h"

#include <string>

#include "hash_table.h"

class ReplManager {
public:
    ReplManager();
    ~ReplManager();
    void AddReplCommand(ReplCommand* command);
    void SetDefaultCommand(ReplCommand* command);
    void Evaluate(const std::string& command);
    void PrintHelp() const;
private:
    HashTable<std::string, ReplCommand*> command_table_;
    ReplCommand* default_command_;
    bool initial_default_ = true;
};

#endif // !REPL_MANAGER_H
