#ifndef UPLOADCOMMAND_H
#define UPLOADCOMMAND_H
#include <string>
#include "CLI.h"
#include "Command.h"
using namespace std;
class UploadCommand : public Command {
    public:
    UploadCommand(CLI& cli,DefaultIO &dio);
    virtual void execute();
    ~UploadCommand()=default;

};
#endif