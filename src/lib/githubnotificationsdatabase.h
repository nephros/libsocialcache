/*
 * Copyright (C) 2014-2015 Jolla Ltd.
 * Contact: Antti Seppälä <antti.seppala@jollamobile.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef GITHUBNOTIFICATIONSDATABASE_H
#define GITHUBNOTIFICATIONSDATABASE_H

#include "abstractsocialcachedatabase.h"

#include <QtCore/QSharedPointer>
#include <QStringList>
#include <QDateTime>

class GithubNotificationPrivate;
class GithubNotification
{
public:
    typedef QSharedPointer<GithubNotification> Ptr;
    typedef QSharedPointer<const GithubNotification> ConstPtr;
    virtual ~GithubNotification();
    static GithubNotification::Ptr create(const QString &githubId, const QString &from, const QString &to,
                                            const QDateTime &createdTime, const QDateTime &updatedTime,
                                            const QString &title, const QString &link,
                                            const QString &application, const QString &object,
                                            bool unread, int accountId, const QString &clientId);
    QString githubId() const;
    QString from() const;
    QString to() const;
    QDateTime createdTime() const;
    QDateTime updatedTime() const;
    QString title() const;
    QString link() const;
    QString application() const;
    QString object() const;
    bool unread() const;
    int accountId() const;
    QString clientId() const;

protected:
    QScopedPointer<GithubNotificationPrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(GithubNotification)
    explicit GithubNotification(const QString &githubId, const QString &from, const QString &to,
                                  const QDateTime &createdTime, const QDateTime &updatedTime,
                                  const QString &title, const QString &link,
                                  const QString &application, const QString &object,
                                  bool unread, int accountId, const QString &clientId);
};


class GithubNotificationsDatabasePrivate;
class GithubNotificationsDatabase: public AbstractSocialCacheDatabase
{
    Q_OBJECT
    Q_PROPERTY(QVariantList accountIdFilter READ accountIdFilter WRITE setAccountIdFilter NOTIFY accountIdFilterChanged)

public:
    explicit GithubNotificationsDatabase();
    ~GithubNotificationsDatabase();

    QVariantList accountIdFilter() const;
    void setAccountIdFilter(const QVariantList &accountIds);

    void addGithubNotification(const QString &githubId, const QString &from, const QString &to,
                                 const QDateTime &createdTime, const QDateTime &updatedTime,
                                 const QString &title, const QString &link,
                                 const QString &application, const QString &object,
                                 bool unread, int accountId, const QString &clientId);
    void removeAllNotifications();
    void removeNotifications(int accountId);
    void removeNotification(const QString &notificationId);
    void removeNotifications(QStringList notificationIds);
    void purgeOldNotifications(int limitInDays);
    void sync();
    QList<GithubNotification::ConstPtr> notifications();

signals:
    void notificationsChanged();
    void accountIdFilterChanged();

protected:
    void readFinished();
    bool write();
    bool createTables(QSqlDatabase database) const;
    bool dropTables(QSqlDatabase database) const;

private:
    void removeNotificationFromQueues(const QString &notificationId);

private:
    Q_DECLARE_PRIVATE(GithubNotificationsDatabase)
};

#endif // GITHUBNOTIFICATIONSDATABASE_H
