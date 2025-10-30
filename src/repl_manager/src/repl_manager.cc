#include "repl_manager.h"


ReplManager::ReplManager() {
    initial_default_ = true;
    default_command_ = new ReplCommand();
}

ReplManager::~ReplManager() {
    if (initial_default_) {
        delete default_command_;
    }
}

void ReplManager::AddReplCommand(ReplCommand* command) {
    this->command_table_.Insert(command->GetCommand(), command);
}

void ReplManager::SetDefaultCommand(ReplCommand* command) {
    if (initial_default_) {
        delete default_command_;
        initial_default_ = false;
    }
    this->default_command_ = command;
}

void ReplManager::Evaluate(const std::string& command) {
    std::string command_ = command.substr(0, command.find(' '));
    if (command_ == "help") {
        PrintHelp();
        return;
    }
    auto i = this->command_table_.Find(command_);
    if (i == this->command_table_.end()) {
        this->default_command_->Execute(command);
    } else {
        (*i).second->Execute(command);
    }
}

void ReplManager::PrintHelp() const {
    std::cout << "Available commands:" << std::endl;
    for (auto&& command : this->command_table_) {
        std::cout << command.second->GetCommand() << " - " << command.second->GetHelpText() << std::endl;
    }
}
