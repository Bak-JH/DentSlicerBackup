#include "cmdlineparser.h"
#include "configuration.h"

CmdLineParser::CmdLineParser()
{


}

CommandLineParseResult CmdLineParser::parseCommandLine(QString *errorMessage)
{
    //this->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    //const QCommandLineOption resolutionoption("r", "resolution", "");
    //const QCommandLineOption layerheightoption("l", "layer height", "");
    //const QCommandLineOption filloption("f", "fill type", "");
    //const QCommandLineOption supporteroption("s", "supporter type", "");

    //this->addOption(resolutionoption);
    //this->addOption(layerheightoption);
    //this->addOption(filloption);
    //this->addOption(supporteroption);

    //this->addPositionalArgument("inputfilepath", "inputfile path to slice.");
    //this->addPositionalArgument("outputfilepath", "outputfile path to slice.");

    //const QCommandLineOption helpOption = this->addHelpOption();
    //const QCommandLineOption versionOption = this->addVersionOption();

    //if (!this->parse(QCoreApplication::arguments())) {
    //    *errorMessage = this->errorText();
    //    return CommandLineError;
    //}

    //if (this->isSet(versionOption))
    //    return CommandLineVersionRequested;

    //if (this->isSet(helpOption))
    //    return CommandLineHelpRequested;

    //const QStringList positionalArguments = this->positionalArguments();

    //if (positionalArguments.size()<2){
    //    *errorMessage = "There must be exactly 2 file paths.";
    //    return CommandLineError;
    //}
    //if (positionalArguments.size() > 2) {
    //    *errorMessage = "Too many arguments";
    //    return CommandLineError;
    //}
    //inputfilename = positionalArguments.first();
    //outputfilename = positionalArguments.back();
    //if (layerheightoption.valueName() != "")
    //    Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight = layerheightoption.valueName().toFloat();
    //if (supporteroption.valueName() != "")
    //    scfg->support_type = supporteroption.valueName().toInt();
    //if (filloption.valueName() != "")
    //    scfg->infill_type = filloption.valueName().toInt();

    return CommandLineError;
}
