#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>

#define DEFAULT_CONFIG_NAME "config.ini"
#define CONFIG_GROUP_SOURCES "Sources"
#define CONFIG_SOURCES_PATH "Path"
#define CONFIG_GROUP_DATABASE "Database"
#define CONFIG_DATABASE_FILENAME "FileName"

struct OptionsStructure
{
    QString sourceDirectory;
};

extern OptionsStructure optionsData;

#endif // OPTIONS_H
