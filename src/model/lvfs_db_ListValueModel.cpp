/**
 * This file is part of lvfs-db.
 *
 * Copyright (C) 2011-2015 Dmitriy Vilkov, <dav.daemon@gmail.com>
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

#include "lvfs_db_ListValueModel.h"
#include "items/lvfs_db_ValueItem.h"


namespace LVFS {
namespace Db {

ListValueModel::ListValueModel(const Interface::Adaptor<IStorage> &storage, const EntityValueReader &reader, QObject *parent) :
    ValueModel(parent),
    m_reader(reader),
    m_storage(storage)
{}

QVariant ListValueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

void ListValueModel::fetchMore(const QModelIndex &parent)
{
    Container items;
    EntityValue value;

    items.reserve(PrefetchLimit);

    if (m_reader.entity().type() == Entity::Composite)
        for (qint32 actualLimit = 0; actualLimit < PrefetchLimit; ++actualLimit)
            if ((value = m_reader.next()).isValid())
                items.push_back(new ValueItem(m_storage, value));
            else
                break;
    else
        for (qint32 actualLimit = 0; actualLimit < PrefetchLimit; ++actualLimit)
            if ((value = m_reader.next()).isValid())
                items.push_back(new ValueItem(value));
            else
                break;

    if (!items.empty())
    {
        beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + items.size() - 1);
        std::move(items.begin(), items.end(), std::back_inserter(m_items));
        endInsertRows();
    }
}

bool ListValueModel::canFetchMore(const QModelIndex &parent) const
{
    return !parent.isValid() && !m_reader.eof();
}

EntityValue ListValueModel::take(const QModelIndex &index)
{
    EntityValue res;

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    res = static_cast<ValueItem *>(m_items.at(index.row()))->take();
    delete m_items.at(index.row());
    m_items.erase(m_items.begin() + index.row());
    endRemoveRows();

    return res;
}

}}