#include "essentials.h"

CfgQueryType GetCfgQueryType(const QString& cfg_query) {
    static const QMap<QString, CfgQueryType> querystring_to_querytype = {
        {"reg", CfgQueryType::Register},
        {"unreg", CfgQueryType::Unregister},
        {"get", CfgQueryType::GetFormats},
        {"exit", CfgQueryType::Exit},
        {"unreg_all", CfgQueryType::UnregAll}
    };

    const auto it = querystring_to_querytype.find(cfg_query);
    if (it != querystring_to_querytype.end()) {
        return it.value();
    }

    throw IncorrectQueryType("Exception has occured: incorrect query type!");
}

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
        {"temporary", RunType::TEMPORARY},
        {"cfg", RunType::CFG_MODE}
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

QSet<QString> GetConfigurations() {
    QFile file(CONFIG_FILE_PATH_DEFAULT);

    if (!file.exists()) {
        CreateJsonFile(file);
    }

    file.open(QIODevice::ReadOnly); // file open
    QJsonObject json_object = GetDataFromJSON(file);
    file.close(); // file close

    QSet<QString> dst;
    for (const auto& item : json_object.value("types").toArray()) {
        dst.insert(item.toString());
    }

    return dst;
}

bool ValidateFileFormat(const QString& json_file_path) {
    QFile file(json_file_path);
    if (!file.exists()) {
        CreateJsonFile(file);
    }

    QSet<QString> cfg_formats = GetConfigurations();
    const auto it_last_dot = std::find(json_file_path.rbegin(), json_file_path.rend(), '.');

    QString format;
    for (auto it = it_last_dot.base(); it != json_file_path.end(); ++it) {
        format += *it;
    }

    if (cfg_formats.find(format) != cfg_formats.end()) {
        if (format != "json") {
            qDebug() << "Format" << format << "exists in the configuration file!";
            qDebug() << "Unfortunately, the demo version of the application cannot proccess the information from" << format << "file";
            qDebug() << "It only supports json files!";
            return false;
        }
    }
    else if (format == "json") {
        qDebug() << "!!!json format does not exist in the configuration file but it is still supported by default!!!";
    }
    else {
        throw IncorrectFormat("Exception has occured: the application does not support this file format");
    }

    return true;
}

int ConfigurationScenary() {
    QSet<QString> types = GetConfigurations();

    qDebug() << R"(===> IN THE FOLLOWING INSTRUCTIONS '+' MEANS 'SPACE' SYMBOL <===)" << '\n' <<
                R"(To register new supported formats:)" << '\n' << R"(-> Input 'reg' + new formats for example 'txt cpp')" << '\n' <<
                R"(To unregister supported formats:)" << '\n' << R"(-> Input 'unreg' + formats for example 'txt cpp')" << '\n' <<
                R"(To get all supported formats:)" << '\n' << R"(-> Input 'get')" << '\n' <<
                R"(To unregister all supported formats:)" << '\n' << R"(-> Input 'unreg_all')" << '\n' <<
                R"(To exit:)" << '\n' << R"(-> Input 'exit')" << '\n';

    QTextStream qtin(stdin);

    QString str;
    qtin >> str;

    try {
        CfgQueryType q_type = GetCfgQueryType(str);
        while (q_type != CfgQueryType::Exit) {
            if (q_type == CfgQueryType::Register) {
                str = qtin.readLine();
                for (const auto& item : str.simplified().split(" ")) {
                    size_t s = types.size();
                    types.insert(item);
                    if (s == types.size()) {
                        qDebug() << "Format" << item << "is already attached";
                    }
                    else {
                        qDebug() << "Format" << item << "has been attached";
                    }
                }
                qDebug() << "";
            }
            else if (q_type == CfgQueryType::Unregister) {
                str = qtin.readLine();
                for (const auto& item : str.simplified().split(" ")) {
                    size_t s = types.size();
                    types.remove(item);
                    if (s == types.size()) {
                        qDebug() << "Format" << item << "is not attached";
                    }
                    else {
                        qDebug() << "Format" << item << "has been removed";
                    }
                }
                qDebug() << "";
            }
            else if (q_type == CfgQueryType::GetFormats) {
                qDebug() << "This application supports the following formats to work with:";
                for (const auto& item : types) {
                    qDebug() << item;
                }
                qDebug() << "";
            }
            else if (q_type == CfgQueryType::UnregAll) {
                types.clear();
                qDebug() << "All supported formats have been removed!\n";
            }
            qtin >> str;
            q_type = GetCfgQueryType(str);
        }
    } catch (const IncorrectQueryType& e) {
        qDebug() << e.what();
    } catch (...) {
        qDebug() << "Exception has occured: some exception has occured!";
    }

    QJsonArray temp_array;
    for (const auto& item : types) {
        temp_array.append(item);
    }

    QJsonObject json_object;
    json_object["types"] = temp_array;

    QFile file(CONFIG_FILE_PATH_DEFAULT);
    file.open(QIODevice::WriteOnly); // file open

    QJsonDocument json_doc;
    json_doc.setObject(json_object);

    file.write(json_doc.toJson());
    file.close(); // file close

    qDebug() << "All data has been saved!";

    return 0;
}

int DefaultScenary(QCoreApplication& a, const QString& json_file_path) {
    if (!ValidateFileFormat(json_file_path)) {
        return -3;
    }

    FileAssociation* fa_obj = nullptr;
    AssociationAdaptor* adaptor = nullptr;

    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.interface()->isServiceRegistered(FA_SERVICE))
    {
        fa_obj = new FileAssociation(&a);
        adaptor = new AssociationAdaptor(fa_obj);

        if (!connection.registerService(FA_SERVICE))
        {
            qFatal("Could not register FileAssociation service!");
        }

        if (!connection.registerObject(FA_PATH, fa_obj))
        {
            qFatal("Could not register FileAssociation object!");
        }
    }

    QString main_str;
    QTextStream qtin(stdin);

    QJsonObject json_object;

    QFile file;
    file.setFileName(json_file_path);

    if (!file.exists()) {
        CreateJsonFile(file);
    }

    qDebug() << R"(If you want to reset the data stored in)" << json_file_path << R"(- input 'yes')";
    qDebug() << R"(If you DO NOT want to reset the data stored in)" << json_file_path << R"(- input 'no')";

    qtin >> main_str;

    if (main_str != "yes") {
        file.open(QIODevice::ReadOnly); // file open
        json_object = GetDataFromJSON(file);
        file.close(); // file close
    }

    qDebug() << "\nThe data from the JSON file has been successfuly reuploaded!\n";

    QDBusInterface service_interface(FA_SERVICE, FA_PATH, QString(), connection);
    CompleteInterface(service_interface, json_object);

    qDebug() << R"(===> IN THE FOLLOWING INSTRUCTIONS '+' MEANS 'SPACE' SYMBOL <===)" << '\n' <<
                R"(To add supported types to the application:)" << '\n' << R"(-> Input 'add' + application path (relative or global) for example '/home/dir/app' + new supported types for example 'mp3 txt cpp')" << '\n' <<
                R"(To get which types does the application (service) support:)" << '\n' << R"(-> Input 'get_t' + application path (relative or global) for example '/home/dir/app')" << '\n' <<
                R"(To get which applications (services) support the type:)" << '\n' << R"(-> Input 'get_s' + type name for example 'cpp')" << '\n' <<
                R"(To remove a registered domain:)" << '\n' << R"(-> Input 'remove_s' + application path (relative or global) for example '/home/dir/app')" << '\n' <<
                R"(To remove a type/types which is/are attached to a registered domain:)" << '\n' << R"(-> Input 'remove_t' + application path (relative or global) for example '/home/dir/app' + types that should be removed for example 'txt cpp')" << '\n' <<
                R"(To exit:)" << '\n' << R"(-> Input 'exit')" << '\n';

    try {
        qtin >> main_str;
        QueryType query_type = GetQueryType(main_str);

        while (query_type != QueryType::Exit && query_type != QueryType::ERROR) {
            qtin >> main_str;
            if (query_type == QueryType::Add) {
                ValidatePath(main_str);
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                qDebug() << "Domain Name:" << domain_name;

                if (connection.isConnected()) {
                    service_interface.callWithArgumentList(QDBus::Block, "addService", QList<QVariant>{domain_name});

                    QString line = qtin.readLine();
                    for (const auto& item : line.simplified().split(" ")) {
                        service_interface.callWithArgumentList(QDBus::Block, "addType", QList<QVariant>{domain_name, item});
                    }
                }
                else {
                    qFatal("Cannot connect to the D-Bus session bus!");
                }

                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
            else if (query_type == QueryType::GetTypes) {
                ValidatePath(main_str);
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                qDebug() << "Domain Name:" << domain_name;

                if (connection.isConnected()) {
                    const QDBusMessage message = service_interface.callWithArgumentList(QDBus::Block, "serviceExist", QList<QVariant>{domain_name});
                    if (message.arguments().at(0).toBool()) {
                        const QDBusMessage available_types_message = service_interface.callWithArgumentList(QDBus::Block, "getTypes", QList<QVariant>{domain_name});
                        QStringList available_types = available_types_message.arguments().at(0).toStringList();

                        qDebug() << "The following types for the service with the domain name" << domain_name << "are available:";
                        for (const auto& item : available_types) {
                            qDebug() << "->" << item;
                        }
                    }
                    else {
                        qDebug() << "The are no available types!";
                    }
                }
                else {
                    qFatal("Cannot connect to the D-Bus session bus!");
                }

                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
            else if (query_type == QueryType::GetServices) {
                if (connection.isConnected()) {
                    const QDBusMessage available_services_message = service_interface.callWithArgumentList(QDBus::Block, "getServicesByType", QList<QVariant>{main_str});
                    QStringList available_services = available_services_message.arguments().at(0).toStringList();

                    qDebug() << "The following services for the type" << main_str << "are available:";
                    if (available_services.size() == 0) {
                        qDebug() << "The are no available services!";
                    }
                    else {
                        for (const auto& item : available_services) {
                            qDebug() << "->" << item;
                        }
                    }
                }
                else {
                    qFatal("Cannot connect to the D-Bus session bus!");
                }

                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
            else if (query_type == QueryType::RemoveService) {
                ValidatePath(main_str);
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                qDebug() << "Domain Name:" << domain_name;

                if (connection.isConnected()) {
                    service_interface.callWithArgumentList(QDBus::Block, "removeService", QList<QVariant>{domain_name});
                }
                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
            else if (query_type == QueryType::RemoveTypes) {
                ValidatePath(main_str);
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                qDebug() << "Domain Name:" << domain_name;

                if (connection.isConnected()) {
                    QString line = qtin.readLine();
                    for (const auto& item : line.simplified().split(" ")) {
                        service_interface.callWithArgumentList(QDBus::Block, "removeType", QList<QVariant>{domain_name, item});
                    }
                }
                else {
                    qFatal("Cannot connect to the D-Bus session bus!");
                }

                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
        }
    } catch (const IncorrectQueryType& e) {
        // incorrect query type exception
        qDebug() << e.what();
    } catch (const IncorrectPath& e) {
        // incorrect path exception
        qDebug() << e.what();
    } catch (...) {
        qDebug() << "Exception has occured: some exception has occured!";
        // in case of any execption i would still save some data from the interface
        // that's why my catch block is empty. i just need to get to the next line of code after catching the execption
    }

    json_object = GetDataFromInterface(service_interface);

    file.open(QIODevice::WriteOnly); // file open

    QJsonDocument json_doc;
    json_doc.setObject(json_object);

    file.write(json_doc.toJson());
    file.close(); // file close

    qDebug() << "All data has been saved!";

    if (adaptor) {
        delete adaptor;
    }
    if (fa_obj) {
        delete fa_obj;
    }

    return 0;
}

int ExtendedScenary(QCoreApplication& a, const QString& json_file_path) {
    return DefaultScenary(a, json_file_path);
}

int TemporaryScenary(QCoreApplication& a) {
    FileAssociation* fa_obj = nullptr;
    AssociationAdaptor* adaptor = nullptr;

    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.interface()->isServiceRegistered(FA_SERVICE))
    {
        fa_obj = new FileAssociation(&a);
        adaptor = new AssociationAdaptor(fa_obj);

        if (!connection.registerService(FA_SERVICE))
        {
            qFatal("Could not register FileAssociation service!");
        }

        if (!connection.registerObject(FA_PATH, fa_obj))
        {
            qFatal("Could not register FileAssociation object!");
        }
    }

    QString main_str;
    QTextStream qtin(stdin);

    QDBusInterface service_interface(FA_SERVICE, FA_PATH, QString(), connection);

    qDebug() << R"(===> IN THE FOLLOWING INSTRUCTIONS '+' MEANS 'SPACE' SYMBOL <===)" << '\n' <<
                R"(To add supported types to the application:)" << '\n' << R"(-> Input 'add' + application path (relative or global) for example '/home/dir/app' + new supported types for example 'mp3 txt cpp')" << '\n' <<
                R"(To get which types does the application (service) support:)" << '\n' << R"(-> Input 'get_t' + application path (relative or global) for example '/home/dir/app')" << '\n' <<
                R"(To get which applications (services) support the type:)" << '\n' << R"(-> Input 'get_s' + type name for example 'cpp')" << '\n' <<
                R"(To remove a registered domain:)" << '\n' << R"(-> Input 'remove_s' + application path (relative or global) for example '/home/dir/app')" << '\n' <<
                R"(To remove a type/types which is/are attached to a registered domain:)" << '\n' << R"(-> Input 'remove_t' + application path (relative or global) for example '/home/dir/app' + types that should be removed for example 'txt cpp')" << '\n' <<
                R"(To exit:)" << '\n' << R"(-> Input 'exit')" << '\n';

    try {
        qtin >> main_str;
        QueryType query_type = GetQueryType(main_str);

        while (query_type != QueryType::Exit && query_type != QueryType::ERROR) {
            qtin >> main_str;
            if (query_type == QueryType::Add) {
                ValidatePath(main_str);
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                qDebug() << "Domain Name:" << domain_name;

                if (connection.isConnected()) {
                    service_interface.callWithArgumentList(QDBus::Block, "addService", QList<QVariant>{domain_name});

                    QString line = qtin.readLine();
                    for (const auto& item : line.simplified().split(" ")) {
                        service_interface.callWithArgumentList(QDBus::Block, "addType", QList<QVariant>{domain_name, item});
                    }
                }
                else {
                    qFatal("Cannot connect to the D-Bus session bus!");
                }

                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
            else if (query_type == QueryType::GetTypes) {
                ValidatePath(main_str);
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                qDebug() << "Domain Name:" << domain_name;

                if (connection.isConnected()) {
                    const QDBusMessage message = service_interface.callWithArgumentList(QDBus::Block, "serviceExist", QList<QVariant>{domain_name});
                    if (message.arguments().at(0).toBool()) {
                        const QDBusMessage available_types_message = service_interface.callWithArgumentList(QDBus::Block, "getTypes", QList<QVariant>{domain_name});
                        QStringList available_types = available_types_message.arguments().at(0).toStringList();

                        qDebug() << "The following types for the service with the domain name" << domain_name << "are available:";
                        for (const auto& item : available_types) {
                            qDebug() << "->" << item;
                        }
                    }
                    else {
                        qDebug() << "The are no available types!";
                    }
                }
                else {
                    qFatal("Cannot connect to the D-Bus session bus!");
                }

                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
            else if (query_type == QueryType::GetServices) {
                if (connection.isConnected()) {
                    const QDBusMessage available_services_message = service_interface.callWithArgumentList(QDBus::Block, "getServicesByType", QList<QVariant>{main_str});
                    QStringList available_services = available_services_message.arguments().at(0).toStringList();

                    qDebug() << "The following services for the type" << main_str << "are available:";
                    if (available_services.size() == 0) {
                        qDebug() << "The are no available services!";
                    }
                    else {
                        for (const auto& item : available_services) {
                            qDebug() << "->" << item;
                        }
                    }
                }
                else {
                    qFatal("Cannot connect to the D-Bus session bus!");
                }

                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
            else if (query_type == QueryType::RemoveService) {
                ValidatePath(main_str);
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                qDebug() << "Domain Name:" << domain_name;

                if (connection.isConnected()) {
                    service_interface.callWithArgumentList(QDBus::Block, "removeService", QList<QVariant>{domain_name});
                }
                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
            else if (query_type == QueryType::RemoveTypes) {
                ValidatePath(main_str);
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                qDebug() << "Domain Name:" << domain_name;

                if (connection.isConnected()) {
                    QString line = qtin.readLine();
                    for (const auto& item : line.simplified().split(" ")) {
                        service_interface.callWithArgumentList(QDBus::Block, "removeType", QList<QVariant>{domain_name, item});
                    }
                }
                else {
                    qFatal("Cannot connect to the D-Bus session bus!");
                }

                qDebug() << "";

                qtin >> main_str;
                query_type = GetQueryType(main_str);
            }
        }
    } catch (const IncorrectQueryType& e) {
        // incorrect query type exception
        qDebug() << e.what();
    } catch (const IncorrectPath& e) {
        // incorrect path exception
        qDebug() << e.what();
    } catch (...) {
        qDebug() << "Exception has occured: some exception has occured!";
        // in case of any execption i would still save some data from the interface
        // that's why my catch block is empty. i just need to get to the next line of code after catching the execption
    }

    if (adaptor) {
        delete adaptor;
    }
    if (fa_obj) {
        delete fa_obj;
    }

    return 0;
}
