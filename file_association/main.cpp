#include "essentials.h"

#define FA_SERVICE "file.association"
#define FA_PATH "/demo"

#define JSON_FILE_PATH "./data.json"

int main(int argc, char *argv[]) {
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

    QFile file(JSON_FILE_PATH);

    if (!file.exists()) {
        CreateJsonFile(file);
    }

    qDebug() << R"(If you want to reset the data - input 'yes')";
    qDebug() << R"(If you do NOT want to reset the data - input 'no')";

    QString main_str;

    QTextStream qtin(stdin);
    qtin >> main_str;

    if (main_str == "yes") {
        ResetFile(file);
    }

    QJsonObject json_object;
    file.open(QIODevice::ReadWrite); // file open
    if (main_str != "yes") {
        json_object = GetDataFromJSON(file);
    }

    file::association interface(FA_SERVICE, FA_PATH, connection);
    CompleteInterface(interface, json_object);

    qDebug() << "\nThe data from the JSON file has been successfuly reuploaded!\n";

    qDebug() << R"(===> IN THE FOLLOWING INSTRUCTIONS '+' MEANS 'SPACE' SYMBOL <===)" << '\n' <<
                R"(To add supported types to the application:)" << '\n' << R"(-> Input 'add' + application path (relative or global) for example '/home/dir/app' + new supported types for example 'mp3 txt cpp')" << '\n' <<
                R"(To get which types does the application (service) support:)" << '\n' << R"(-> Input 'get_t' + application path (relative or global) for example '/home/dir/app')" << '\n' <<
                R"(To get which applications (services) support the type:)" << '\n' << R"(-> Input 'get_s' + type name for example 'cpp')" << '\n' <<
                R"(To exit:)" << '\n' << R"(-> Input 'exit')" << '\n';

    try {
        qtin >> main_str;
        QueryType query_type = GetQueryType(main_str);

        while (query_type != QueryType::Exit && query_type != QueryType::ERROR) {
            qtin >> main_str;
            if (query_type == QueryType::Add) {
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                qDebug() << "Domain Name:" << domain_name;

                if (connection.isConnected()) {
                    interface.addService(domain_name);

                    QString line = qtin.readLine();
                    for (const auto& item : line.simplified().split(" ")) {
                        interface.addType(domain_name, item);
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
                QString file_path = QFileInfo(main_str).absoluteFilePath();
                QString domain_name = CreateDomainName(file_path);

                if (connection.isConnected()) {
                    if (connection.interface()->isServiceRegistered(domain_name)) {
                        QDBusPendingReply<QStringList> available_types_reply = interface.getTypes(domain_name);
                        QStringList available_types = available_types_reply.value();

                        qDebug() << "-> The following types for the service with the domain name" << domain_name << "are available:";
                        for (const auto& item : available_types) {
                            qDebug() << item;
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
                    QDBusPendingReply<QStringList> available_services_reply = interface.getServicesByType(main_str);
                    QStringList available_services = available_services_reply.value();

                    qDebug() << "-> The following services for the type" << main_str << "are available:";
                    if (available_services.size() == 0) {
                        qDebug() << "The are no available services!";
                    }
                    else {
                        for (const auto& item : available_services) {
                            qDebug() << item;
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
        }
    } catch (const IncorrectQueryType& e) {
        // incorrect query type exception
        qDebug() << e.what();
    } catch (const IncorrectPath& e) {
        qDebug() << e.what();
    } catch (...) {
        qDebug() << "Exception has occured: some exception has occured!";
        // in case of any execption i would still save some data from the interface
        // that's why my catch block is empty. i just need to get to the next line of code after catching the execption
    }

    json_object = GetDataFromInterface(interface);

    file.reset();

    QJsonDocument json_doc;
    json_doc.setObject(json_object);

    file.write(json_doc.toJson());
    file.close(); // file close

    if (adaptor) {
        delete adaptor;
    }
    if (fa_obj) {
        delete fa_obj;
    }

    return 0;
}
