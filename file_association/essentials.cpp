#include "essentials.h"


QueryType GetQueryType(const QString& query) {
    if (query == "exit") {
        return QueryType::Exit;
    }
    else if (query == "add") {
        return QueryType::Add;
    }
    else if (query == "get_t") {
        return QueryType::GetTypes;
    }
    else if (query == "get_s") {
        return QueryType::GetServices;
    }

    throw IncorrectQueryType("Exception has occured: incorrect query type!");
}

void ResetFile(QFile& file) {
    file.open(QIODevice::WriteOnly);
    file.close();
}

QString CreateDomainName(QString filePath) {
    if (filePath.at(0) == '/' || filePath.at(0) == '\\') {
        filePath = filePath.mid(1, filePath.size() - 1);
    }

    /*if (QSysInfo::productType() == "windows") {
        return filePath.replace('\\', '.');
    }*/

    return filePath.replace('/', '.');
}

QJsonObject GetDataFromJSON(QFile& file) {
    QByteArray data = file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(data);
    QJsonObject json_object = json_doc.object();
    return json_object;
}

void CompleteInterface(file::association& interface, const QJsonObject& json_object) {
    for (const auto& key : json_object.keys()) {
        interface.addService(key);
        for (const auto& value : json_object.value(key).toArray()) {
            interface.addType(key, value.toString());
        }
    }
}

QJsonObject GetDataFromInterface(file::association& interface) {
    QJsonObject json_object;
    const QStringList all_keys = interface.getServices();
    for (const auto& key : all_keys) {
        QJsonArray temp_array;

        const QStringList all_types = interface.getTypes(key);
        for (const auto& type : all_types) {
            temp_array.append(type);
        }
        json_object[key] = temp_array;
    }
    return json_object;
}

void CreateJsonFile(QFile& file) {
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    stream << "";
    file.close();
}
