#include "argumentparser.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
ArgumentParser::ArgumentParser()
{
}

void ArgumentParser::Parse(int argc, char *argv[], Config* config)
{
        for (int i = 1; i < argc; i++) // first argument is program name
        {
            if (!strcmp(argv[i], "-h"))
            {
                QFile f(":/help.txt");
                if (f.open(QFile::ReadOnly))
                {
                    QTextStream in(&f);
                    qDebug() << in.readAll();
                }
                exit(0);
            }
            else if (!strcmp(argv[i], "-p"))
            {
                if (argc > i+1)
                {
                    config->portNumber = atoi(argv[++i]);
                }
                else
                {
                    qWarning() << "Port number is missing after argument -p";
                    exit(1);
                }
            }
            else if (!strcmp(argv[i], "-d"))
            {
                if (argc > i+1)
                {
                    config->displayID = atoi(argv[++i]);
                }
                else
                {
                    qWarning() << "Display number is missing after argument -v";
                    exit(1);
                }
            }
            else if (!strcmp(argv[i], "-v"))
            {
                qDebug() << "Program version: " << config->programVersion;
                exit(0);
            }
            else if (!strcmp(argv[i], "-e"))
            {
                if (argc > i+1)
                {
                    if (argv[i+1][0] != '-')
                        config->isEmulator = argv[++i];
                    else
                        config->isEmulator = "800x480";
                }
                else
                {
                    config->isEmulator = "800x480";
                }
            }
            else if (!strcmp(argv[i], "-l"))
            {
                if (argc > i+1)
                {
                    config->logs = atoi(argv[++i]);
                }
                else
                {
                    qWarning() << "Number of stored logs is missing after argument -l";
                    exit(1);
                }
            }
            else
            {
                qWarning() << "Unknown argument: " << argv[i];
            }
        }
        return;
}
