/*
 * Copyright (C) 2014 Jolla Ltd.
 * Contact: Bea Lam <bea.lam@jollamobile.com>
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

    static GithubNotification::Ptr create(int accountId,
                                      const int &threadId,
                                      const QString &type,
                                      const QString &title,
                                      const QString &from,
                                      const QString &reason,
                                      const bool    &unread,
                                      const QString &repo,
                                      const QString &avatar,
                                      const QString &url,
                                      const QDateTime &updatedTime);
    int accountId() const;
    int threadId() const;
    QString type() const;
    QString title() const;
    QString from() const;
    QString reason() const;
    bool    unread() const;
    QString repo() const;
    QString avatar() const;
    QString url() const;
    QDateTime updatedTime() const;


protected:
    QScopedPointer<GithubNotificationPrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(GithubNotification)
    explicit GithubNotification(int accountId,
                            const int &threadId,
                            const QString &type,
                            const QString &title,
                            const QString &from,
                            const QString &reason,
                            const bool    &unread,
                            const QString &repo,
                            const QString &avatar,
                            const QString &url,
                            const QDateTime &updatedTime);
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

    void addGithubNotification(int accountId,
                           const int &threadId,
                           const QString &type,
                           const QString &title,
                           const QString &from,
                           const QString &reason,
                           const bool    &unread,
                           const QString &repo,
                           const QString &avatar,
                           const QString &url,
                           const QDateTime &updatedTime);

    void removeAllNotifications();
    void removeNotifications(int accountId);
    void removeNotification(const QString &notificationId);
    void removeNotifications(const QStringList &notificationIds);

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
    Q_DECLARE_PRIVATE(GithubNotificationsDatabase)
};

#endif // GITHUBNOTIFICATIONSDATABASE_H
