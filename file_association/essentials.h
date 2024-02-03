#ifndef ESSENTIALS_H
#define ESSENTIALS_H

#include <QCoreApplication>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusConnectionInterface>
#include <QFileInfo>
#include <QDir>
#include <QDBusInterface>
#include <QDBusReply>
#include <QJsonObject>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>
#include <QMap>
#include <QString>
#include <QDBusPendingReply>
#include <QMap>
#include <QDBusArgument>
#include <QVariant>

#include <algorithm>

#include "fileassociation.h"
#include "FA_Adaptor.h"
#include "FA_Interface.h"

#define FA_SERVICE "file.association"
#define FA_PATH "/demo"

#define JSON_FILE_PATH_DEFAULT "./data.json"
#define CONFIG_FILE_PATH_DEFAULT "./cfg.json"

class IncorrectQueryType : public std::exception {
public:
    IncorrectQueryType(const char* message) : error_message_(message) {}

    const char* what() const noexcept override {
        return error_message_.c_str();
    }

private:
    std::string error_message_;
};

class IncorrectRunType : public std::exception {
public:
    IncorrectRunType(const char* message) : error_message_(message) {}

    const char* what() const noexcept override {
        return error_message_.c_str();
    }

private:
    std::string error_message_;
};

class IncorrectPath : public std::exception {
public:
    IncorrectPath(const char* message) : error_message_(message) {}

    const char* what() const noexcept override {
        return error_message_.c_str();
    }

private:
    std::string error_message_;
};

class IncorrectFormat : public std::exception {
public:
    IncorrectFormat(const char* message) : error_message_(message) {}

    const char* what() const noexcept override {
        return error_message_.c_str();
    }

private:
    std::string error_message_;
};


enum class CfgQueryType {
    Register,
    Unregister,
    GetFormats,
    UnregAll,
    Exit
};

enum class QueryType {
    Exit,
    Add,
    GetTypes,
    GetServices,
    RemoveTypes,
    RemoveService,
    ERROR
};

enum class RunType {
    DEFAULT,
    EXTENDED,
    TEMPORARY,
    CFG_MODE
};

CfgQueryType GetCfgQueryType(const QString& cfg_query);

QueryType GetQueryType(const QString& query);

RunType GetRunType(const QString& run);

QString CreateDomainName(QString filePath);

QJsonObject GetDataFromJSON(QFile& file);

void CompleteInterface(QDBusInterface& interface, const QJsonObject& json_object);

QJsonObject GetDataFromInterface(QDBusInterface& interface);

void CreateJsonFile(QFile& file);

void ValidatePath(const QString& path);

QSet<QString> GetConfigurations();

bool ValidateFileFormat(const QString& json_file_path);

// scenaries

int ConfigurationScenary();

int DefaultScenary(QCoreApplication& a, const QString& json_file_path);

int ExtendedScenary(QCoreApplication& a, const QString& json_file_path);

int TemporaryScenary(QCoreApplication& a);

#endif // ESSENTIALS_H
