/*
 * Copyright (C) 2014-2015 Jolla Ltd.
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

#ifndef GITHUBNOTIFICATIONSMODEL_H
#define GITHUBNOTIFICATIONSMODEL_H

#include "abstractsocialcachemodel.h"

class GithubNotificationsModelPrivate;
class GithubNotificationsModel : public AbstractSocialCacheModel
{
    Q_OBJECT
    Q_PROPERTY(QVariantList accountIdFilter READ accountIdFilter WRITE setAccountIdFilter NOTIFY accountIdFilterChanged)

    Q_ENUMS(GithubNotificationsRole)
public:
    enum GithubNotificationsRole {
        NotificationId = 0,
        From,
        To,
        Timestamp,
        Title,
        Link,
        AppId,
        Object,
        Accounts,
        ClientId,
        Unread
    };
    explicit GithubNotificationsModel(QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const;

    QVariantList accountIdFilter() const;
    void setAccountIdFilter(const QVariantList &accountIds);

    void refresh();

    Q_INVOKABLE void remove(const QString &notificationId);
    Q_INVOKABLE void clear();

signals:
    void accountIdFilterChanged();

private Q_SLOTS:
    void notificationsChanged();

private:
    Q_DECLARE_PRIVATE(GithubNotificationsModel)
};

#endif // GITHUBNOTIFICATIONSMODEL_H
