#include "essentials.h"


QueryType GetQueryType(const QString& query) {
    static const QMap<QString, QueryType> querystring_to_querytype = {
        {"exit", QueryType::Exit},
        {"add", QueryType::Add},
        {"get_t", QueryType::GetTypes},
        {"get_s", QueryType::GetServices},
        {"remove_t", QueryType::RemoveTypes},
        {"remove_s", QueryType::RemoveService}
    };

    const auto it = querystring_to_querytype.find(query);
    if (it != querystring_to_querytype.end()) {
        return it.value();
    }

    throw IncorrectQueryType("Exception has occured: incorrect query type!");
}

RunType GetRunType(const QString& run) {
    static const QMap<QString, RunType> runstring_to_runtype = {
        {"default", RunType::DEFAULT},
        {"extended", RunType::EXTENDED},
        {"temporary", RunType::TEMPORARY}
    };

    const auto it = runstring_to_runtype.find(run);
    if (it != runstring_to_runtype.end()) {
        return it.value();
    }

    throw IncorrectRunType("Exception has occured: incorrect run type!");
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

void CompleteInterface(QDBusInterface& interface, const QJsonObject& json_object) {
    for (const auto& key : json_object.keys()) {
        interface.callWithArgumentList(QDBus::Block, "addService", QList<QVariant>{key});
        for (const auto& value : json_object.value(key).toArray()) {
            QString temp_str = value.toString();
            interface.callWithArgumentList(QDBus::Block, "addType", QList<QVariant>{key, temp_str});
        }
    }
}

QJsonObject GetDataFromInterface(QDBusInterface& interface) {
    QJsonObject json_object;
    const QStringList all_keys = interface.call(QDBus::Block, "getServices").arguments().at(0).toStringList();
    for (const auto& key : all_keys) {
        const QStringList all_types = interface.callWithArgumentList(QDBus::Block, "getTypes", QList<QVariant>{key}).arguments().at(0).toStringList();
        if (all_types.size() == 0) {
            continue;
        }

        QJsonArray temp_array;

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

void ValidatePath(const QString& path) {
    if (!path.contains('/')) {
        throw IncorrectPath("Exception has occured: incorrect service path!");
    }
}
