#include "fileassociation.h"

FileAssociation::FileAssociation(QObject* parent)
    : QObject(parent) {
}

void FileAssociation::addService(const QString& service_domain) {
    if (!service_to_types_.count(service_domain)) {
        if (!QDBusConnection::sessionBus().registerService(service_domain)) {
            qCritical() << "Failed to REGISTER service with D-Bus:" << service_domain;
        }
        else {
            qDebug() << "Service REGISTERED with D-Bus:" << service_domain;
            service_to_types_[service_domain] = {};
            emit ServiceAdded(service_domain);
        }
    }
}

void FileAssociation::addType(const QString& service_domain, const QString& type_name) {
    if (service_to_types_.count(service_domain) && !service_to_types_[service_domain].contains(type_name)) {
        service_to_types_[service_domain].insert(type_name);
        type_to_services_[type_name].insert(service_domain);
        qDebug() << "-> Service" << service_domain << "has been successfully ADDED to the list of available services for the type" << type_name;
        qDebug() << "-> Type" << type_name << "has been successfully ADDED to the list of available types for the service" << service_domain;
        emit TypeAdded(service_domain, type_name);
    }
}

void FileAssociation::removeService(const QString& service_domain) {
    if (service_to_types_.count(service_domain)) {
        if (!QDBusConnection::sessionBus().unregisterService(service_domain)) {
            qCritical() << "Failed to UNREGISTER service with D-Bus:" << service_domain;
        }
        else {
            qDebug() << "Service UNREGISTERED with D-Bus:" << service_domain;
            service_to_types_.remove(service_domain);
            emit ServiceRemoved(service_domain);
        }
    }
}

void FileAssociation::removeType(const QString& service_domain, const QString& type_name) {
    if (service_to_types_.count(service_domain) && service_to_types_[service_domain].contains(type_name)) {
        service_to_types_[service_domain].remove(type_name);
        type_to_services_.remove(type_name);
        qDebug() << "-> Service" << service_domain << "has been successfully REMOVED from list of available services for the type" << type_name;
        qDebug() << "-> Type" << type_name << "has been successfully REMOVED from list of available types for the service" << service_domain;
        emit TypeRemoved(service_domain, type_name);
    }
}

QStringList FileAssociation::getTypes(const QString& service_domain) const {
    if (service_to_types_.count(service_domain)) {
        return {service_to_types_[service_domain].begin(), service_to_types_[service_domain].end()};
    }
    return {};
}

bool FileAssociation::serviceExist(const QString& service_domain) const {
    return service_to_types_.count(service_domain);
}

QStringList FileAssociation::getServices() const {
    QStringList dst;
    for (auto it = service_to_types_.begin(); it != service_to_types_.end(); ++it) {
        dst.append(it.key());
    }
    return dst;
}

QStringList FileAssociation::getServicesByType(const QString& type_name) const {
    QStringList dst;
    const QSet<QString> services = type_to_services_[type_name];
    for (const auto& service : services) {
        if (service_to_types_.count(service)) {
            dst.append(service);
        }
    }
    return dst;
}

QMap<QString, QSet<QString>> FileAssociation::data() const {
    return service_to_types_;
}

FileAssociation::~FileAssociation() {
}
