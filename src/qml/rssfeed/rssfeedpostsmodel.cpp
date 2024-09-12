/*
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Lucien Xu <lucien.xu@jollamobile.com>
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

#include "rssfeedpostsmodel.h"
#include "abstractsocialcachemodel_p.h"
#include "rssfeedpostsdatabase.h"
#include <QtCore/QDebug>
#include "postimagehelper_p.h"

class RSSFeedModelPrivate: public AbstractSocialCacheModelPrivate
{
public:
    explicit RSSFeedModelPrivate(RSSFeedModel *q);

    RSSFeedDatabase database;

private:
    Q_DECLARE_PUBLIC(RSSFeedModel)
};

RSSFeedModelPrivate::RSSFeedModelPrivate(RSSFeedModel *q)
    : AbstractSocialCacheModelPrivate(q)
{
}

RSSFeedModel::RSSFeedModel(QObject *parent)
    : AbstractSocialCacheModel(*(new RSSFeedModelPrivate(this)), parent)
{
    Q_D(RSSFeedModel);

     connect(&d->database, &AbstractSocialPostCacheDatabase::postsChanged,
             this, &RSSFeedModel::postsChanged);
     connect(&d->database, SIGNAL(accountIdFilterChanged()),
             this, SIGNAL(accountIdFilterChanged()));
}

QHash<int, QByteArray> RSSFeedModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(FeedName, "feedName");
    roleNames.insert(Body, "body");
    roleNames.insert(Timestamp, "timestamp");
    roleNames.insert(Icon, "icon");
    roleNames.insert(Images, "images");
    roleNames.insert(Accounts, "accounts");
    return roleNames;
}

QVariantList RSSFeedModel::accountIdFilter() const
{
    Q_D(const RSSFeedModel);

    return d->database.accountIdFilter();
}

void RSSFeedModel::setAccountIdFilter(const QVariantList &accountIds)
{
    Q_D(RSSFeedModel);

    d->database.setAccountIdFilter(accountIds);
}

void RSSFeedModel::refresh()
{
    Q_D(RSSFeedModel);

    d->database.refresh();
}

void RSSFeedModel::postsChanged()
{
    Q_D(RSSFeedModel);

    SocialCacheModelData data;
    QList<SocialPost::ConstPtr> postsData = d->database.posts();
    Q_FOREACH (const SocialPost::ConstPtr &post, postsData) {
        QMap<int, QVariant> eventMap;
        eventMap.insert(RSSFeedModel::Body, post->body());
        eventMap.insert(RSSFeedModel::Timestamp, post->timestamp());
        eventMap.insert(RSSFeedModel::Icon, post->icon());

        QVariantList images;
        Q_FOREACH (const SocialPostImage::ConstPtr &image, post->images()) {
            images.append(createImageData(image));
        }
        eventMap.insert(RSSFeedModel::Images, images);

        eventMap.insert(RSSFeedModel::FeedName, d->database.feedName(post));

        QVariantList accountsVariant;
        Q_FOREACH (int account, post->accounts()) {
            accountsVariant.append(account);
        }
        eventMap.insert(RSSFeedModel::Accounts, accountsVariant);
        data.append(eventMap);
    }

    updateData(data);
}
