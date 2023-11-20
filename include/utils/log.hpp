#pragma once
#include <spdlog/spdlog.h>
#include <string>


#define LOG_INFO(toLog) LOG(INFO) << FILE_ID << ", " << toLog;
#define LOG_WARNING(toLog) LOG(WARNING) << toLog;
#define LOG_ERROR(toLog) LOG(ERROR) << toLog;
#define LOG_FATAL(toLog) LOG(FATAL) << toLog;
#define CONDTION_LOG_INFO(toLog, condition) LOG_IF(INFO, condition) << toLog;
#define CONDTION_LOG_WARNING(toLog, condition) LOG_IF(WARNING, condition) << toLog;
#define CONDTION_LOG_ERROR(toLog, condition) LOG_IF(ERROR, condition) << toLog;
#define CONDTION_LOG_FATAL(toLog, condition) LOG_IF(FATAL, condition) << toLog;

class logger
{
  public:
	static void init(const std::string &pathToLogs, const std::string &projectName);
	static void close();
};
