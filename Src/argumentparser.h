#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H
#include "config.h"
#include <QString>

 class ArgumentParser
{
public:
     ArgumentParser();
     static void Parse(int argc, char *argv[], Config *config);
};

#endif // ARGUMENTPARSER_H
