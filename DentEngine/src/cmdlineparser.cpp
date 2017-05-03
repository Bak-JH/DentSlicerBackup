#include "cmdlineparser.h"

CmdLineParser::CmdLineParser()
{


}

CommandLineParseResult CmdLineParser::parseCommandLine(QString *errorMessage)
{
    this->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption filloption("f", "fill type", "fill");
    const QCommandLineOption supporteroption("s", "supporter type", "supporter");

    this->addOption(filloption);
    this->addOption(supporteroption);

    this->addPositionalArgument("inputfilepath", "inputfile path to slice.");
    this->addPositionalArgument("outputfilepath", "outputfile path to slice.");

    const QCommandLineOption helpOption = this->addHelpOption();
    const QCommandLineOption versionOption = this->addVersionOption();

    if (!this->parse(QCoreApplication::arguments())) {
        *errorMessage = this->errorText();
        return CommandLineError;
    }

    if (this->isSet(versionOption))
        return CommandLineVersionRequested;

    if (this->isSet(helpOption))
        return CommandLineHelpRequested;

    const QStringList positionalArguments = this->positionalArguments();

    if (positionalArguments.size()<2){
        *errorMessage = "There must be exactly 2 file paths.";
        return CommandLineError;
    }
    if (positionalArguments.size() > 2) {
        *errorMessage = "Too many arguments";
        return CommandLineError;
    }
    inputfilename = positionalArguments.first();
    outputfilename = positionalArguments.back();

    return CommandLineOk;
}
