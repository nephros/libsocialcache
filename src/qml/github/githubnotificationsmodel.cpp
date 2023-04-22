/*
 * Copyright (C) 2014 Jolla Ltd.
 * Contact: Antti Seppälä <antti.seppala@jollamobile.com>
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

#include "githubnotificationsmodel.h"
#include "abstractsocialcachemodel_p.h"
#include "githubnotificationsdatabase.h"

class GithubNotificationsModelPrivate : public AbstractSocialCacheModelPrivate
{
public:
    explicit GithubNotificationsModelPrivate(GithubNotificationsModel *q);

    GithubNotificationsDatabase database;

private:
    Q_DECLARE_PUBLIC(GithubNotificationsModel)
};

GithubNotificationsModelPrivate::GithubNotificationsModelPrivate(GithubNotificationsModel *q)
    : AbstractSocialCacheModelPrivate(q)
{
}

GithubNotificationsModel::GithubNotificationsModel(QObject *parent)
    : AbstractSocialCacheModel(*(new GithubNotificationsModelPrivate(this)), parent)
{
    Q_D(GithubNotificationsModel);

    connect(&d->database, SIGNAL(notificationsChanged()), this, SLOT(notificationsChanged()));
    //connect(&d->database, SIGNAL(accountIdFilterChanged()), this, SIGNAL(accountIdFilterChanged()));
}

QHash<int, QByteArray> GithubNotificationsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(NotificationId, "identifier");
    roleNames.insert(Type,   "type");
    roleNames.insert(Title,  "title");
    roleNames.insert(From,   "from");
    roleNames.insert(Repo,   "repo");
    roleNames.insert(Avatar, "avatar");
    roleNames.insert(Link,    "url");
    roleNames.insert(TimeStamp, "createdTime");
    return roleNames;
}

QVariantList GithubNotificationsModel::accountIdFilter() const
{
    Q_D(const GithubNotificationsModel);

    //return d->database.accountIdFilter();
    return m_accountIdFilterStub;
}

void GithubNotificationsModel::setAccountIdFilter(const QVariantList &accountIds)
{
    Q_D(GithubNotificationsModel);

    //d->database.setAccountIdFilter(accountIds);
    m_accountIdFilterStub = accountIds;
}

void GithubNotificationsModel::refresh()
{
    notificationsChanged();
}

void GithubNotificationsModel::remove(const QString &notificationId)
{
    Q_D(GithubNotificationsModel);
    for (int i=0; i<count(); i++) {
        if (getField(i, GithubNotificationsModel::NotificationId).toString() == notificationId) {
            d->removeRange(i, 1);
            d->database.removeNotification(notificationId);
            d->database.sync();
            break;
        }
    }
}

void GithubNotificationsModel::clear()
{
    Q_D(GithubNotificationsModel);
    d->clearData();
    d->database.removeAllNotifications();
}

void GithubNotificationsModel::notificationsChanged()
{
    Q_D(GithubNotificationsModel);

    SocialCacheModelData data;
    QList<GithubNotification::ConstPtr> notificationsData = d->database.notifications();
    Q_FOREACH (const GithubNotification::ConstPtr &notification, notificationsData) {
        QMap<int, QVariant> eventMap;

        eventMap.insert(GithubNotificationsModel::NotificationId, notification->identifier());
        eventMap.insert(GithubNotificationsModel::Type, notification->type());
        eventMap.insert(GithubNotificationsModel::Title, notification->title());
        eventMap.insert(GithubNotificationsModel::From, notification->from());
        eventMap.insert(GithubNotificationsModel::Repo, notification->repo());
        eventMap.insert(GithubNotificationsModel::Avatar, notification->avatar());
        eventMap.insert(GithubNotificationsModel::Link, notification->url());
        eventMap.insert(GithubNotificationsModel::TimeStamp, notification->createdTime());

        QVariantList accountsVariant;
        accountsVariant.append(notification->accountId());
        //eventMap.insert(GithubNotificationsModel::Accounts, accountsVariant);

        //eventMap.insert(GithubNotificationsModel::ClientId, notification->clientId());

        data.append(eventMap);
    }

    updateData(data);
}
