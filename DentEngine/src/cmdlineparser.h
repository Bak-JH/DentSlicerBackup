#ifndef CMDLINEPARSER_H
#define CMDLINEPARSER_H
#include <QCommandLineParser>

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

class CmdLineParser :public QCommandLineParser
{
public:
    QString inputfilename;
    QString outputfilename;

public:
    CmdLineParser();
    CommandLineParseResult parseCommandLine(QString *errorMessage);
};

#endif // CMDLINEPARSER_H
