#ifndef __ZB_LOG
#define __ZB_LOG


typedef enum    {
    NONE    = -1,
    ERROR   = 0,
    WARN    = 1,
    INFO    = 2,
    DEBUG   = 3
} LogLevel;

const char *LogLevelLabels[] = {
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG"
};


class Log   {

    public:
        static QFile *logfile;
        static QTextStream *outs;
        static bool initialized;
        static LogLevel loglevel;

        static void log(LogLevel level, const char *name, QString msg)  {
            if (loglevel >= level)   {

                *(Log::outs) << LogLevelLabels[level] 
                             << " [" << QDate::currentDate().toString(Qt::ISODate) 
                             << " " << QTime::currentTime().toString(Qt::ISODate) << "]";

                *(Log::outs) <<  " [" << QCoreApplication::applicationFilePath() << "-" << QCoreApplication::applicationPid() << "]";

                if (name)   {
                    *(Log::outs) << " [" << name << "]";
                }
                if (!msg.isEmpty())    {
                    *(Log::outs) << " " << msg;
                }

                *(Log::outs) << "\n";

                Log::outs->flush();
            }
        }
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

bool Log::initialized = false;
LogLevel Log::loglevel = DEBUG;
QFile *Log::logfile;
QTextStream *Log::outs;

#endif
