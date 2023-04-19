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

#include "githubnotificationsdatabase.h"
#include "abstractsocialcachedatabase_p.h"
#include "socialsyncinterface.h"

#include <QtSql/QSqlError>
#include <QtCore/QtDebug>

static const char *DB_NAME = "githubNotifications.db";
static const int VERSION = 1;

struct GithubNotificationPrivate
{
    explicit GithubNotificationPrivate(const QString &githubId, const QString &from, const QString &to,
                                         const QDateTime &createdTime, const QDateTime &updatedTime,
                                         const QString &title, const QString &link,
                                         const QString &application, const QString &object,
                                         bool unread, int accountId, const QString &clientId);

    QString m_githubId;
    QString m_from;
    QString m_to;
    QDateTime m_createdTime;
    QDateTime m_updatedTime;
    QString m_title;
    QString m_link;
    QString m_application;
    QString m_object;
    bool m_unread;
    int m_accountId;
    QString m_clientId;
};

GithubNotificationPrivate::GithubNotificationPrivate(const QString &githubId, const QString &from, const QString &to,
                                                         const QDateTime &createdTime, const QDateTime &updatedTime,
                                                         const QString &title, const QString &link,
                                                         const QString &application, const QString &object,
                                                         bool unread, int accountId, const QString &clientId)
    : m_githubId(githubId)
    , m_from(from)
    , m_to(to)
    , m_createdTime(createdTime)
    , m_updatedTime(updatedTime)
    , m_title(title)
    , m_link(link)
    , m_application(application)
    , m_object(object)
    , m_unread(unread)
    , m_accountId(accountId)
    , m_clientId(clientId)
{
}

GithubNotification::GithubNotification(const QString &githubId, const QString &from, const QString &to,
                                           const QDateTime &createdTime, const QDateTime &updatedTime,
                                           const QString &title, const QString &link,
                                           const QString &application, const QString &object,
                                           bool unread, int accountId, const QString &clientId)
    : d_ptr(new GithubNotificationPrivate(githubId, from, to, createdTime, updatedTime, title, link,
                                            application, object, unread, accountId, clientId))
{
}

GithubNotification::Ptr GithubNotification::create(const QString &githubId, const QString &from, const QString &to,
                                                       const QDateTime &createdTime, const QDateTime &updatedTime,
                                                       const QString &title, const QString &link,
                                                       const QString &application, const QString &object,
                                                       bool unread, int accountId, const QString &clientId)
{
    return GithubNotification::Ptr(new GithubNotification(githubId, from, to, createdTime, updatedTime, title, link,
                                                              application, object, unread, accountId, clientId));
}

GithubNotification::~GithubNotification()
{
}

QString GithubNotification::githubId() const
{
    Q_D(const GithubNotification);
    return d->m_githubId;
}

QString GithubNotification::from() const
{
    Q_D(const GithubNotification);
    return d->m_from;
}

QString GithubNotification::to() const
{
    Q_D(const GithubNotification);
    return d->m_to;
}

QDateTime GithubNotification::createdTime() const
{
    Q_D(const GithubNotification);
    return d->m_createdTime;
}

QDateTime GithubNotification::updatedTime() const
{
    Q_D(const GithubNotification);
    return d->m_updatedTime;
}

QString GithubNotification::title() const
{
    Q_D(const GithubNotification);
    return d->m_title;
}

QString GithubNotification::link() const
{
    Q_D(const GithubNotification);
    return d->m_link;
}

QString GithubNotification::application() const
{
    Q_D(const GithubNotification);
    return d->m_application;
}

QString GithubNotification::object() const
{
    Q_D(const GithubNotification);
    return d->m_object;
}

bool GithubNotification::unread() const
{
    Q_D(const GithubNotification);
    return d->m_unread;
}

int GithubNotification::accountId() const
{
    Q_D(const GithubNotification);
    return d->m_accountId;
}

QString GithubNotification::clientId() const
{
    Q_D(const GithubNotification);
    return d->m_clientId;
}

class GithubNotificationsDatabasePrivate: public AbstractSocialCacheDatabasePrivate
{
public:
    explicit GithubNotificationsDatabasePrivate(GithubNotificationsDatabase *q);

    QMap<int, QList<GithubNotification::ConstPtr> > insertNotifications;
    QList<int> removeNotificationsFromAccounts;
    QVariantList accountIdFilter;
    QStringList removeNotifications;
    int purgeTimeLimit;

    struct {
        QMap<int, QList<GithubNotification::ConstPtr> > insertNotifications;
        QList<int> removeNotificationsFromAccounts;
        QStringList removeNotifications;
        bool removeAll;
    } queue;
};

GithubNotificationsDatabasePrivate::GithubNotificationsDatabasePrivate(GithubNotificationsDatabase *q)
    : AbstractSocialCacheDatabasePrivate(
            q,
            SocialSyncInterface::socialNetwork(SocialSyncInterface::Github),
            SocialSyncInterface::dataType(SocialSyncInterface::Notifications),
            QLatin1String(DB_NAME),
            VERSION)
    , purgeTimeLimit(0)
{
    queue.removeAll = false;
}

GithubNotificationsDatabase::GithubNotificationsDatabase()
    : AbstractSocialCacheDatabase(*(new GithubNotificationsDatabasePrivate(this)))
{
}

GithubNotificationsDatabase::~GithubNotificationsDatabase()
{
    wait();
}

QVariantList GithubNotificationsDatabase::accountIdFilter() const
{
    Q_D(const GithubNotificationsDatabase);

    return d->accountIdFilter;
}

void GithubNotificationsDatabase::setAccountIdFilter(const QVariantList &accountIds)
{
    Q_D(GithubNotificationsDatabase);

    if (d->accountIdFilter != accountIds) {
        d->accountIdFilter = accountIds;
        emit accountIdFilterChanged();
    }
}

void GithubNotificationsDatabase::addGithubNotification(const QString &githubId, const QString &from, const QString &to,
                                                            const QDateTime &createdTime, const QDateTime &updatedTime,
                                                            const QString &title, const QString &link,
                                                            const QString &application, const QString &object,
                                                            bool unread, int accountId, const QString &clientId)
{
    Q_D(GithubNotificationsDatabase);
    d->insertNotifications[accountId].append(GithubNotification::create(githubId, from, to, createdTime, updatedTime, title, link,
                                                                          application, object, unread, accountId, clientId));
}

void GithubNotificationsDatabase::removeAllNotifications()
{
    Q_D(GithubNotificationsDatabase);

    {
        QMutexLocker locker(&d->mutex);
        d->queue.insertNotifications.clear();
        d->queue.removeNotificationsFromAccounts.clear();
        d->queue.removeNotifications.clear();
        d->queue.removeAll = true;
    }

    executeWrite();
}

void GithubNotificationsDatabase::removeNotifications(int accountId)
{
    Q_D(GithubNotificationsDatabase);

    QMutexLocker locker(&d->mutex);
    if (!d->queue.removeNotificationsFromAccounts.contains(accountId)) {
        d->queue.removeNotificationsFromAccounts.append(accountId);
    }
    d->queue.insertNotifications.remove(accountId);
}

void GithubNotificationsDatabase::removeNotification(const QString &notificationId)
{
    Q_D(GithubNotificationsDatabase);

    QMutexLocker locker(&d->mutex);
    removeNotificationFromQueues(notificationId);
}

void GithubNotificationsDatabase::removeNotifications(QStringList notificationIds)
{
    Q_D(GithubNotificationsDatabase);

    QMutexLocker locker(&d->mutex);
    Q_FOREACH(const QString notifId, notificationIds) {
        removeNotificationFromQueues(notifId);
    }
}

void GithubNotificationsDatabase::purgeOldNotifications(int limitInDays)
{
    Q_D(GithubNotificationsDatabase);
    d->purgeTimeLimit = limitInDays;
}

void GithubNotificationsDatabase::removeNotificationFromQueues(const QString &notificationId)
{
    Q_D(GithubNotificationsDatabase);

    if (!d->queue.removeNotifications.contains(notificationId)) {
        d->queue.removeNotifications.append(notificationId);
        Q_FOREACH(int accountId, d->insertNotifications.keys()) {
            for (int i = 0; i < d->insertNotifications[accountId].count(); ++i) {
                if (d->insertNotifications[accountId][i]->githubId() == notificationId) {
                    d->insertNotifications[accountId].removeAt(i);
                    i--;
                }
            }
        }
    }
}

void GithubNotificationsDatabase::sync()
{
    Q_D(GithubNotificationsDatabase);

    {
        QMutexLocker locker(&d->mutex);
        Q_FOREACH(int accountId, d->insertNotifications.keys()) {
            d->queue.insertNotifications.insert(accountId, d->insertNotifications.take(accountId));
        }
        while (d->removeNotificationsFromAccounts.count()) {
            d->queue.removeNotificationsFromAccounts.append(d->removeNotificationsFromAccounts.takeFirst());
        }
        while (d->removeNotifications.count()) {
            d->queue.removeNotifications.append(d->removeNotifications.takeFirst());
        }
    }

    executeWrite();
}

QList<GithubNotification::ConstPtr> GithubNotificationsDatabase::notifications()
{
    Q_D(GithubNotificationsDatabase);

    QList<GithubNotification::ConstPtr> data;

    QString queryString = QStringLiteral(
                "SELECT githubId, accountId, fromStr, toStr, createdTime, updatedTime, title, link, application," \
                "objectStr, unread, clientId FROM notifications");
    if (!d->accountIdFilter.isEmpty()) {
        QStringList accountIds;
        for (int i=0; i<d->accountIdFilter.count(); i++) {
            if (d->accountIdFilter[i].type() == QVariant::Int) {
                accountIds << d->accountIdFilter[i].toString();
            }
        }
        if (accountIds.count()) {
            queryString += " WHERE accountId IN (" + accountIds.join(',') + ')';
        }
    }
    queryString += QStringLiteral(" ORDER BY updatedTime DESC");
    QSqlQuery query = prepare(queryString);

    if (!query.exec()) {
        qWarning() << Q_FUNC_INFO << "Failed to query events" << query.lastError().text();
        return data;
    }

    while (query.next()) {
        data.append(GithubNotification::create(query.value(0).toString(),                      // githubId
                                                 query.value(2).toString(),                      // from
                                                 query.value(3).toString(),                      // to
                                                 QDateTime::fromTime_t(query.value(4).toInt()),  // createdTime
                                                 QDateTime::fromTime_t(query.value(5).toInt()),  // updatedTime
                                                 query.value(6).toString(),                      // title
                                                 query.value(7).toString(),                      // link
                                                 query.value(8).toString(),                      // application
                                                 query.value(9).toString(),                      // object
                                                 query.value(10).toBool(),                       // unread
                                                 query.value(1).toInt(),                         // accountId
                                                 query.value(11).toString()));                   // clientId
    }

    return data;
}

void GithubNotificationsDatabase::readFinished()
{
    emit notificationsChanged();
}

bool GithubNotificationsDatabase::write()
{
    Q_D(GithubNotificationsDatabase);

    QMutexLocker locker(&d->mutex);

    const QMap<int, QList<GithubNotification::ConstPtr> > insertNotifications = d->queue.insertNotifications;
    const QList<int> removeNotificationsFromAccounts = d->queue.removeNotificationsFromAccounts;
    QStringList removeNotifications = d->queue.removeNotifications;
    bool removeAll = d->queue.removeAll;

    d->queue.insertNotifications.clear();
    d->queue.removeNotificationsFromAccounts.clear();
    d->queue.removeNotifications.clear();
    d->queue.removeAll = false;

    locker.unlock();

    bool success = true;
    QSqlQuery query;

    if (removeAll) {
        QVariantList accountIds;
        accountIds.append(-1);

        query = prepare(QStringLiteral("DELETE FROM notifications WHERE accountId > :accountId"));
        query.bindValue(QStringLiteral(":accountId"), accountIds);
        executeBatchSocialCacheQuery(query);
    }

    if (!removeNotificationsFromAccounts.isEmpty()) {
        QVariantList accountIds;

        Q_FOREACH (const int accountId, removeNotificationsFromAccounts) {
            accountIds.append(accountId);
        }

        query = prepare(QStringLiteral("DELETE FROM notifications WHERE accountId = :accountId"));
        query.bindValue(QStringLiteral(":accountId"), accountIds);
        executeBatchSocialCacheQuery(query);
    }

    if (!removeNotifications.isEmpty()) {
        QVariantList notifIds;

        Q_FOREACH (const QString notifId, removeNotifications) {
            notifIds.append(notifId);
        }

        query = prepare(QStringLiteral("DELETE FROM notifications WHERE githubId = :githubId"));
        query.bindValue(QStringLiteral(":githubId"), notifIds);
        executeBatchSocialCacheQuery(query);
    }

    if (!insertNotifications.isEmpty()) {
        QVariantList githubIds;
        QVariantList accountIds;
        QVariantList fromStrings;
        QVariantList toStrings;
        QVariantList createdTimes;
        QVariantList updatedTimes;
        QVariantList titles;
        QVariantList links;
        QVariantList applications;
        QVariantList unreads;
        QVariantList objects;
        QVariantList clientIds;

        Q_FOREACH (const QList<GithubNotification::ConstPtr> &notifications, insertNotifications) {
            Q_FOREACH (const GithubNotification::ConstPtr &notification, notifications) {
                githubIds.append(notification->githubId());
                accountIds.append(notification->accountId());
                fromStrings.append(notification->from());
                toStrings.append(notification->to());
                createdTimes.append(notification->createdTime().toTime_t());
                updatedTimes.append(notification->updatedTime().toTime_t());
                titles.append(notification->title());
                links.append(notification->link());
                applications.append(notification->application());
                objects.append(notification->object());
                unreads.append((notification->unread()));
                clientIds.append(notification->clientId());
            }
        }

        query = prepare(QStringLiteral(
                    "INSERT OR REPLACE INTO notifications ("
                    " githubId, accountId, fromStr, toStr, createdTime, updatedTime, title, link, application, objectStr, unread, clientId) "
                    "VALUES("
                    " :githubId, :accountId, :fromStr, :toStr, :createdTime, :updatedTime, :title, :link, :application, :objectStr, :unread, :clientId)"));
        query.bindValue(QStringLiteral(":githubId"), githubIds);
        query.bindValue(QStringLiteral(":accountId"), accountIds);
        query.bindValue(QStringLiteral(":fromStr"), fromStrings);
        query.bindValue(QStringLiteral(":toStr"), toStrings);
        query.bindValue(QStringLiteral(":createdTime"), createdTimes);
        query.bindValue(QStringLiteral(":updatedTime"), updatedTimes);
        query.bindValue(QStringLiteral(":title"), titles);
        query.bindValue(QStringLiteral(":link"), links);
        query.bindValue(QStringLiteral(":application"), applications);
        query.bindValue(QStringLiteral(":objectStr"), objects);
        query.bindValue(QStringLiteral(":unread"), unreads);
        query.bindValue(QStringLiteral(":clientId"), clientIds);

        executeBatchSocialCacheQuery(query);
    }

    if (d->purgeTimeLimit > 0) {
        QVariantList limits;
        // purge notifications older than expirationTime in days
        const quint32 limit = QDateTime::currentDateTime().toTime_t() - d->purgeTimeLimit * 24 * 60 * 60;
        limits.append(limit);
        query = prepare(QStringLiteral("DELETE FROM notifications WHERE updatedTime < :timeLimit"));
        query.bindValue(QStringLiteral(":timeLimit"), limits);
        executeBatchSocialCacheQuery(query);
        d->purgeTimeLimit = 0;
    }

    return success;
}

bool GithubNotificationsDatabase::createTables(QSqlDatabase database) const
{
    QSqlQuery query(database);

    // create the Github notification db tables
    // notifications = githubId, accountId, from, to, createdTime, updatedTime, title, link, application, objectStr, unread, clientId
    query.prepare("CREATE TABLE IF NOT EXISTS notifications ("
                  "githubId TEXT UNIQUE PRIMARY KEY,"
                  "accountId INTEGER,"
                  "fromStr TEXT,"
                  "toStr TEXT,"
                  "createdTime INTEGER,"
                  "updatedTime INTEGER,"
                  "title TEXT,"
                  "link TEXT,"
                  "application TEXT,"
                  "objectStr TEXT,"
                  "unread INTEGER,"
                  "clientId TEXT)");
    if (!query.exec()) {
        qWarning() << Q_FUNC_INFO << "Unable to create notifications table: " << query.lastError().text();
        return false;
    }

    return true;
}

bool GithubNotificationsDatabase::dropTables(QSqlDatabase database) const
{
    QSqlQuery query(database);

    if (!query.exec(QStringLiteral("DROP TABLE IF EXISTS notifications"))) {
        qWarning() << Q_FUNC_INFO << "Unable to delete notifications table: " << query.lastError().text();
        return false;
    }

    return true;
}
