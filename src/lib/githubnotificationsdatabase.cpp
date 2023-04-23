/*
 * Copyright (C) 2014 Jolla Ltd.
 * Contact: Bea Lam <bea.lam@jollamobile.com>
 * Copyright (C) 2023 Peter G.
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
static const int VERSION = 0;

struct GithubNotificationPrivate
{
    explicit GithubNotificationPrivate(const QString &identifier,
                                   int accountId,
                                   const QString &type,
                                   const QString &title,
                                   const QString &from,
                                   const QString &reason,
                                   const bool    &unread,
                                   const QString &repo,
                                   const QString &avatar,
                                   const QString &url,
                                   const QDateTime &createdTime);

    QString m_id;
    int m_accountId;
    QString m_type;
    QString m_title;
    QString m_from;
    QString m_reason;
    bool    m_unread;
    QString m_repo;
    QString m_avatar;
    QString m_url;
    QDateTime m_createdTime;
};

GithubNotificationPrivate::GithubNotificationPrivate(const QString &identifier,
                                             int accountId,
                                             const QString &type,
                                             const QString &title,
                                             const QString &from,
                                             const QString &reason,
                                             const bool    &unread,
                                             const QString &repo,
                                             const QString &avatar,
                                             const QString &url,
                                             const QDateTime &createdTime)
    : m_id(identifier)
    , m_accountId(accountId)
    , m_type(type)
    , m_title(title)
    , m_from(from)
    , m_reason(reason)
    , m_unread(unread)
    , m_repo(repo)
    , m_avatar(avatar)
    , m_url(url)
    , m_createdTime(createdTime)
{
}

GithubNotification::GithubNotification(const QString &identifier,
                               int accountId,
                               const QString &type,
                               const QString &title,
                               const QString &from,
                               const QString &reason,
                               const bool    &unread,
                               const QString &repo,
                               const QString &avatar,
                               const QString &url,
                               const QDateTime &createdTime)
    : d_ptr(new GithubNotificationPrivate(identifier, accountId, type, title, from, reason, unread, repo, avatar, url, createdTime))
{
}

GithubNotification::Ptr GithubNotification::create(const QString &identifier,
                                           int accountId,
                                           const QString &type,
                                           const QString &title,
                                           const QString &from,
                                           const QString &reason,
                                           const bool    &unread,
                                           const QString &repo,
                                           const QString &avatar,
                                           const QString &url,
                                           const QDateTime &createdTime)
{
    return GithubNotification::Ptr(new GithubNotification(identifier, accountId, type, title, from, reason, unread, repo, avatar, url, createdTime));
}

GithubNotification::~GithubNotification()
{
}

QString GithubNotification::identifier() const
{
    Q_D(const GithubNotification);
    return d->m_id;
}

QString GithubNotification::title() const
{
    Q_D(const GithubNotification);
    return d->m_title;
}

QString GithubNotification::type() const
{
    Q_D(const GithubNotification);
    return d->m_type;
}

QString GithubNotification::from() const
{
    Q_D(const GithubNotification);
    return d->m_from;
}

QString GithubNotification::reason() const
{
    Q_D(const GithubNotification);
    return d->m_reason;
}

bool GithubNotification::unread() const
{
    Q_D(const GithubNotification);
    return d->m_unread;
}

QString GithubNotification::repo() const
{
    Q_D(const GithubNotification);
    return d->m_repo;
}

QString GithubNotification::avatar() const
{
    Q_D(const GithubNotification);
    return d->m_avatar;
}

QString GithubNotification::url() const
{
    Q_D(const GithubNotification);
    return d->m_url;
}

QDateTime GithubNotification::createdTime() const
{
    Q_D(const GithubNotification);
    return d->m_createdTime;
}

int GithubNotification::accountId() const
{
    Q_D(const GithubNotification);
    return d->m_accountId;
}


class GithubNotificationsDatabasePrivate: public AbstractSocialCacheDatabasePrivate
{
public:
    explicit GithubNotificationsDatabasePrivate(GithubNotificationsDatabase *q);

    QMap<int, QList<GithubNotification::ConstPtr> > insertNotifications;
    QList<int> removeNotificationsFromAccounts;
    QStringList removeNotifications;

    struct {
        QMap<int, QList<GithubNotification::ConstPtr> > insertNotifications;
        QList<int> removeNotificationsFromAccounts;
        QStringList removeNotifications;
    } queue;
};

GithubNotificationsDatabasePrivate::GithubNotificationsDatabasePrivate(GithubNotificationsDatabase *q)
    : AbstractSocialCacheDatabasePrivate(
            q,
            SocialSyncInterface::socialNetwork(SocialSyncInterface::Github),
            SocialSyncInterface::dataType(SocialSyncInterface::Notifications),
            QLatin1String(DB_NAME),
            VERSION)
{
}

GithubNotificationsDatabase::GithubNotificationsDatabase()
    : AbstractSocialCacheDatabase(*(new GithubNotificationsDatabasePrivate(this)))
{
}

GithubNotificationsDatabase::~GithubNotificationsDatabase()
{
    wait();
}

void GithubNotificationsDatabase::addGithubNotification(int accountId,
                                                const QString &type,
                                                const QString &title,
                                                const QString &from,
                                                const QString &reason,
                                                const bool    &unread,
                                                const QString &repo,
                                                const QString &avatar,
                                                const QString &url,
                                                const QDateTime &createdTime)
{
    Q_D(GithubNotificationsDatabase);
    qDebug() << Q_FUNC_INFO << "creating" << accountId << type << title << from << reason << unread << repo << avatar << url << createdTime;
    d->insertNotifications[accountId].append(GithubNotification::create(QString(), accountId, type, title, from, reason, unread, repo, avatar, url, createdTime));
}

void GithubNotificationsDatabase::removeAllNotifications()
{
   //FIXME: this is in the qml plugin
   qWarning() << Q_FUNC_INFO << "Not implemented";
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
    if (!d->queue.removeNotifications.contains(notificationId)) {
        d->queue.removeNotifications.append(notificationId);
    }
}

void GithubNotificationsDatabase::removeNotifications(const QStringList &notificationIds)
{
    Q_D(GithubNotificationsDatabase);

    QMutexLocker locker(&d->mutex);
    Q_FOREACH(const QString notifId, notificationIds) {
        removeNotification(notifId);
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
    QList<GithubNotification::ConstPtr> data;

    QSqlQuery query;
    query = prepare(QStringLiteral(
                "SELECT identifier, accountId, typeStr, titleStr, fromStr, reasonStr, unread, repoStr, avatarUrl, url, createdTime " \
                "FROM notifications ORDER BY createdTime DESC"));

    if (!query.exec()) {
        qWarning() << Q_FUNC_INFO << "Failed to query events" << query.lastError().text();
        return data;
    }

    while (query.next()) {
        data.append(GithubNotification::create(QString::number(query.value(0).toInt()),         // id
                                           query.value(1).toInt(),                          // accountId
                                           query.value(2).toString(),                       // type
                                           query.value(3).toString(),                       // title
                                           query.value(4).toString(),                       // from
                                           query.value(5).toString(),                       // reason
                                           query.value(6).toBool(),                         // unread
                                           query.value(7).toString(),                       // repo
                                           query.value(8).toString(),                       // avatar
                                           query.value(9).toString(),                       // url
                                           QDateTime::fromTime_t(query.value(10).toInt())));// createdTime
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

    d->queue.insertNotifications.clear();
    d->queue.removeNotificationsFromAccounts.clear();
    d->queue.removeNotifications.clear();

    locker.unlock();

    bool success = true;
    QSqlQuery query;

    if (!removeNotificationsFromAccounts.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "removing...";
        QVariantList accountIds;

        Q_FOREACH (const int accountId, removeNotificationsFromAccounts) {
            accountIds.append(accountId);
        }

        query = prepare(QStringLiteral("DELETE FROM notifications WHERE accountId = :accountId"));
        query.bindValue(QStringLiteral(":accountId"), accountIds);
        qDebug() << Q_FUNC_INFO << "executing DELETE from" << accountIds;
        executeBatchSocialCacheQuery(query);
    }

    if (!removeNotifications.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "removing...";
        QVariantList notifIds;

        Q_FOREACH (const QString notifId, removeNotifications) {
            notifIds.append(notifId.toInt());
        }

        query = prepare(QStringLiteral("DELETE FROM notifications WHERE identifier = :identifier"));
        query.bindValue(QStringLiteral(":identifier"), notifIds);
        qDebug() << Q_FUNC_INFO << "executing DELETE from" << notifIds;
        executeBatchSocialCacheQuery(query);
        executeBatchSocialCacheQuery(query);
    }

    if (!insertNotifications.isEmpty()) {
        QVariantList accountIds;
        QVariantList types;
        QVariantList titles;
        QVariantList froms;
        QVariantList reasons;
        QVariantList unreads;
        QVariantList repos;
        QVariantList avatars;
        QVariantList urls;
        QVariantList createdTimes;

        Q_FOREACH (const QList<GithubNotification::ConstPtr> &notifications, insertNotifications) {
            Q_FOREACH (const GithubNotification::ConstPtr &notification, notifications) {
                accountIds.append(notification->accountId());
                types.append(notification->type());
                titles.append(notification->title());
                froms.append(notification->from());
                reasons.append(notification->reason());
                unreads.append(notification->unread());
                repos.append(notification->repo());
                avatars.append(notification->avatar());
                urls.append(notification->url());
                createdTimes.append(notification->createdTime().toTime_t());
            }
        }

        query = prepare(QStringLiteral(
                    "INSERT OR REPLACE INTO notifications ("
                    "accountId, typeStr, titleStr, fromStr, reasonStr, unread, repoStr, avatarUrl, url, createdTime) "
                    "VALUES("
                    ":accountId, :typeStr, :titleStr, :fromStr, :reasonStr, :unread, :repoStr, :avatarUrl, :url, :createdTime)"));
        query.bindValue(QStringLiteral(":accountId"), accountIds);
        query.bindValue(QStringLiteral(":typeStr"), types);
        query.bindValue(QStringLiteral(":titleStr"), titles);
        query.bindValue(QStringLiteral(":fromStr"), froms);
        query.bindValue(QStringLiteral(":reasonStr"), reasons);
        query.bindValue(QStringLiteral(":unread"), unreads);
        query.bindValue(QStringLiteral(":repoStr"), repos);
        query.bindValue(QStringLiteral(":avatarUrl"), avatars);
        query.bindValue(QStringLiteral(":url"), urls);
        query.bindValue(QStringLiteral(":createdTime"), createdTimes);

        qDebug() << Q_FUNC_INFO << "executing INSERT or REPLACE for" << accountIds;
        executeBatchSocialCacheQuery(query);
    }

    return success;
}

bool GithubNotificationsDatabase::createTables(QSqlDatabase database) const
{
    qDebug() << Q_FUNC_INFO << "called";
    QSqlQuery query(database);

    query.prepare("CREATE TABLE IF NOT EXISTS notifications ("\
                  "identifier INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT,"\
                  "accountId INTEGER,"\
                  "typeStr TEXT,"\
                  "titleStr TEXT,"\
                  "fromStr TEXT,"\
                  "reasonStr TEXT,"\
                  "unread BOOLEAN CHECK (unread IN (0, 1),"\
                  "repoStr TEXT,"\
                  "avatarUrl TEXT,"\
                  "url TEXT,"\
                  "createdTime INTEGER)");
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
