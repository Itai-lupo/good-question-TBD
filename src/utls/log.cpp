#include "log.hpp"
#include <stdio.h>


static bool wasInit = false; 
void logger::init(const std::string& pathToLogs, const std::string& projectName)
{
    if(wasInit) return;
    google::InitGoogleLogging(projectName.c_str());
    FLAGS_stderrthreshold = 0;

    google::SetLogDestination(google::INFO, (pathToLogs + "info/" + projectName + "_").c_str());
    google::SetLogDestination(google::WARNING, (pathToLogs + "warninig/" + projectName + "_").c_str());
    google::SetLogDestination(google::ERROR, (pathToLogs + "error/" + projectName + "_").c_str());
    google::SetLogDestination(google::FATAL, (pathToLogs + "fatal/" + projectName + "_").c_str());
    wasInit = true;
    LOG(INFO) << "init glog successfully";
}

void logger::close()
{
    wasInit = false;
    google::ShutdownGoogleLogging();
}
