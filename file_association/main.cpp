#include "essentials.h"

#define FA_SERVICE "file.association"
#define FA_PATH "/demo"

#define JSON_FILE_PATH_DEFAULT "./data.json"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        qDebug() << "Use following instrucions:";
        qDebug() << R"(To start the application in the "default" mode (.json data file will be configured right into your build's directory), input:)";
        qDebug() << R"(-> ./file_association default)";
        qDebug() << R"(To start the application in the "extended" mode, input:)";
        qDebug() << R"(-> ./file_association extended /path/to/your/json/data/file)";
        qDebug() << R"(To start the application in the "temporary" mode (.json will not be created and your data will be erased after the session), input)";
        qDebug() << R"(-> ./file_association temporary)";
        return -1;
    }

    QString json_file_path;
    RunType run_type;

    try {
        run_type = GetRunType(argv[1]);
        switch (run_type)
        {
            case (RunType::DEFAULT):
                json_file_path = JSON_FILE_PATH_DEFAULT;
                break;
            case (RunType::EXTENDED):
                json_file_path = argv[2];
                break;
            case (RunType::TEMPORARY):
                break;
        }
    }
    catch (const IncorrectRunType& e) {
        // incorrect run type exception
        qDebug() << e.what();
        return -1;
    }
    catch (...) {
        qDebug() << "Exception has occured: some exception has occured before the QCoreApplication object initialization!";
        return -1;
    }

    QCoreApplication a(argc, argv);

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
    if (run_type != RunType::TEMPORARY) {
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
    }

    QDBusInterface service_interface(FA_SERVICE, FA_PATH, QString(), connection);
    CompleteInterface(service_interface, json_object);

    qDebug() << R"(===> IN THE FOLLOWING INSTRUCTIONS '+' MEANS 'SPACE' SYMBOL <===)" << '\n' <<
                R"(To add supported types to the application:)" << '\n' << R"(-> Input 'add' + application path (relative or global) for example '/home/dir/app' + new supported types for example 'mp3 txt cpp')" << '\n' <<
                R"(To get which types does the application (service) support:)" << '\n' << R"(-> Input 'get_t' + application path (relative or global) for example '/home/dir/app')" << '\n' <<
                R"(To get which applications (services) support the type:)" << '\n' << R"(-> Input 'get_s' + type name for example 'cpp')" << '\n' <<
                R"(To remove a registered domain:)" << '\n' << R"(-> Input 'remove_s' + application path (relative or global) for example '/home/dir/app')" << '\n' <<
                R"(To remove a type which is attached to a registered domain:)" << '\n' << R"(-> Input 'remove_t' + application path (relative or global) for example '/home/dir/app' + type name that should be removed for example 'txt')" << '\n' <<
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

    if (run_type != RunType::TEMPORARY) {
        json_object = GetDataFromInterface(service_interface);

        file.open(QIODevice::WriteOnly); // file open

        QJsonDocument json_doc;
        json_doc.setObject(json_object);

        file.write(json_doc.toJson());
        file.close(); // file close

        qDebug() << "All data has been saved!";
    }

    if (adaptor) {
        delete adaptor;
    }
    if (fa_obj) {
        delete fa_obj;
    }

    return 0;
}
