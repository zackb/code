#include "log.h"

const char *LogLevelLabels[] = {
    " ERROR ",
    " WARN  ",
    " INFO  ",
    " DEBUG "
};

void Log::log(LogLevel level, const char *name, QString msg)  {
    if (loglevel >= level)   {

        *(Log::outs) << "[" << LogLevelLabels[level] << "] -"
                     << " [" << QDate::currentDate().toString(Qt::ISODate) 
                     << " " << QTime::currentTime().toString(Qt::ISODate) << "]";

        *(Log::outs) <<  " [" << "log-" << QCoreApplication::applicationPid() << "]";

        if (name)   {
            *(Log::outs) << " [" << name << "]";
        }
        if (!msg.isEmpty())    {
            *(Log::outs) << " " << msg;
        }

        *(Log::outs) << "\n";

        Log::outs->flush();

        if (DEBUG == level) {
            qDebug() << name << msg;
        }
    }
}


bool Log::initialized = false;
LogLevel Log::loglevel = DEBUG;
QFile *Log::logfile;
QTextStream *Log::outs;
