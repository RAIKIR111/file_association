
#include "fileassociation.h"

FileAssociation::FileAssociation(QObject* parent)
    : QObject(parent) {
}

void FileAssociation::addService(const QString& service_domain) {
    if (!service_to_types_.count(service_domain)) {
        service_to_types_[service_domain] = {};
        qDebug() << "The following domain name has been REGISTERED:" << service_domain;
        emit ServiceAdded(service_domain);
    }
}

void FileAssociation::addType(const QString& service_domain, const QString& type_name) {
    if (service_to_types_.count(service_domain) && !service_to_types_[service_domain].contains(type_name)) {
        service_to_types_[service_domain].insert(type_name);
        type_to_services_[type_name].insert(service_domain);
        qDebug() << "-> Domain name" << service_domain << "has been successfully ADDED to the list of available domains for the type" << type_name;
        qDebug() << "-> Type" << type_name << "has been successfully ADDED to the list of available types for the domain" << service_domain;
        emit TypeAdded(service_domain, type_name);
    }
}

void FileAssociation::removeService(const QString& service_domain) {
    if (service_to_types_.remove(service_domain)) {
        for (auto it = type_to_services_.begin(); it != type_to_services_.end(); ++it) {
            it.value().remove(service_domain);
        }
        qDebug() << "-> The following domain name has been UNREGISTERED:" << service_domain;
        emit ServiceRemoved(service_domain);
    }
    else {
        qDebug() << "-> Domain" << service_domain << "does not exist!";
    }
}

void FileAssociation::removeType(const QString& service_domain, const QString& type_name) {
    if (service_to_types_.count(service_domain) && service_to_types_[service_domain].contains(type_name)) {
        service_to_types_[service_domain].remove(type_name);
        type_to_services_.remove(type_name);
        qDebug() << "-> Domain name" << service_domain << "has been successfully REMOVED from list of available domains for the type" << type_name;
        qDebug() << "-> Type" << type_name << "has been successfully REMOVED from list of available types for the domain" << service_domain;
        emit TypeRemoved(service_domain, type_name);
    }
    else {
        qDebug() << "-> Type" << type_name << "is not attached to the domain" << service_domain;
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
    if (type_to_services_.count(type_name)) {
        return {type_to_services_[type_name].begin(), type_to_services_[type_name].end()};
    }
    return {};
}

QMap<QString, QSet<QString>> FileAssociation::data() const {
    return service_to_types_;
}

FileAssociation::~FileAssociation() {
}
