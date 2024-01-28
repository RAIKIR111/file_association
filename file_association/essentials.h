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

#include "fileassociation.h"
#include "FA_Adaptor.h"
#include "FA_Interface.h"

class IncorrectQueryType : public std::exception {
public:
    IncorrectQueryType(const char* message) : error_message_(message) {}

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

enum class QueryType {
    Exit,
    Add,
    GetTypes,
    GetServices,
    ERROR
};

QueryType GetQueryType(const QString& query);

void ResetFile(QFile& file);

QString CreateDomainName(QString filePath);

QJsonObject GetDataFromJSON(QFile& file);

void CompleteInterface(file::association& interface, const QJsonObject& json_object);

QJsonObject GetDataFromInterface(file::association& interface);

void CreateJsonFile(QFile& file);

#endif // ESSENTIALS_H
