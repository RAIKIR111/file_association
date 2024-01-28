#ifndef FILEASSOCIATION_H
#define FILEASSOCIATION_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QList>
#include <QDBusConnection>
#include <QDBusArgument>
#include <QMetaType>
#include <QDBusMetaType>

class FileAssociation : public QObject
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface", "file.association")

signals:
    void ServiceAdded(const QString& service_domain);
    void TypeAdded(const QString& service_domain, const QString& type_name);
    void ServiceRemoved(const QString& service_domain);
    void TypeRemoved(const QString& service_domain, const QString& type_name);
    void GotTypes(const QString& service_domain);


public slots:
    void addService(const QString& service_domain);
    void addType(const QString& service_domain, const QString& type_name);
    void removeService(const QString& service_domain);
    void removeType(const QString& service_domain, const QString& type_name);
    QStringList getTypes(const QString& service_domain) const;
    bool serviceExist(const QString& service_domain) const;
    QStringList getServices() const;
    QStringList getServicesByType(const QString& type_name) const;

public:
    FileAssociation(QObject* parent = nullptr);
    virtual ~FileAssociation();

    QMap<QString, QSet<QString>> data() const;

private:
    QMap<QString, QSet<QString>> service_to_types_;
    QMap<QString, QSet<QString>> type_to_services_;
};

#endif // FILEASSOCIATION_H
