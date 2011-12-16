#ifndef __ZB_LOG
#define __ZB_LOG 1

#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>


typedef enum    {
    NONE    = -1,
    ERROR   = 0,
    WARN    = 1,
    INFO    = 2,
    DEBUG   = 3
} LogLevel;

class Log   {

    public:
        static QFile *logfile;
        static QTextStream *outs;
        static bool initialized;
        static LogLevel loglevel;

        static void log(LogLevel level, const char *name, QString msg);
        static void error(const char *name, QString msg)  {
            log(ERROR, name, msg);
        }
        static void warn(const char *name, QString msg)  {
            log(WARN, name, msg);
        }
        static void info(const char *name, QString msg)  {
            log(INFO, name, msg);
        }
        static void debug(const char *name, QString msg)  {
            log(DEBUG, name, msg);
        }

        static void init(const char *filename, LogLevel level)  {
            if (!initialized)    {
                if (level > NONE)   {
                    Log::logfile = new QFile(filename);
                    Log::logfile->open(QIODevice::WriteOnly | QIODevice::Append);
                    Log::outs = new QTextStream(Log::logfile);
                    Log::outs->setCodec("utf-8");
                }
                Log::loglevel = level;
                Log::initialized = true;
            }
        }

};

#endif
