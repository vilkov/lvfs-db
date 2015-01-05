/**
 * This file is part of lvfs-db.
 *
 * Copyright (C) 2011-2014 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * lvfs-db is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lvfs-db is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lvfs-db. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LVFS_DB_QUERYRESULTPATHVALUEITEM_H_
#define LVFS_DB_QUERYRESULTPATHVALUEITEM_H_

#include <liquiddb/EntityValue>
#include <QtCore/QCoreApplication>
#include "lvfs_db_QueryResultPathItem.h"


namespace LVFS {
namespace Db {

class QueryResultPathValueItem : public QueryResultPathItem
{
    Q_DECLARE_TR_FUNCTIONS(QueryResultPathValueItem)

public:
//    QueryResultPathValueItem(const IFileContainer *container, SnapshotItem *item, Item *parent);

    /* QueryResultItem */
    virtual bool isRootPathValue();

    /* QueryResultPathItem */
    virtual void open() const;

private:
//    const IFileContainer *m_container;
//    IFileContainer::Holder m_thisContainer;
};

}}

#endif /* LVFS_DB_QUERYRESULTPATHVALUEITEM_H_ */