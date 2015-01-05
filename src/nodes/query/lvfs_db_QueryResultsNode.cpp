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

#include "lvfs_db_QueryResultsNode.h"
#include "items/lvfs_db_QueryResultCompositeRootItem.h"


namespace LVFS {
namespace Db {

using namespace LiquidDb;


QueryResultsNode::QueryResultsNode(const Interface::Adaptor<IStorage> &container, const EntityValueReader &reader, const Interface::Holder &parent) :
    Complements<Core::Node, Db::INode>(container.interface(), parent),
    m_container(container),
    m_reader(reader),
    m_geometry(),
    m_sorting(0, ::Qt::AscendingOrder)
{
    ASSERT(m_sorting.first < columnCount(QModelIndex()));
}

QueryResultsNode::~QueryResultsNode()
{}

void QueryResultsNode::refresh(int depth)
{

}

void QueryResultsNode::opened(const Interface::Holder &view)
{

}

void QueryResultsNode::closed(const Interface::Holder &view)
{

}

void QueryResultsNode::clear()
{

}

void QueryResultsNode::fetchMore(const QModelIndex &parent)
{
    EntityValue item;
    EFC::Vector<Item *> list;
    EFC::List<Interface::Holder> files;

    list.reserve(PrefetchLimit);

    if (m_reader.entity().type() == Entity::Composite)
        for (int actualLimit = 0; actualLimit < PrefetchLimit; ++actualLimit)
            if ((item = m_reader.next()).isValid())
                list.push_back(new QueryResultCompositeRootItem(files, m_container, item));
            else
                break;
    else
        for (int actualLimit = 0; actualLimit < PrefetchLimit; ++actualLimit)
            if ((item = m_reader.next()).isValid())
                list.push_back(new QueryResultRootItem(item));
            else
                break;

    if (!list.empty())
    {
//        if (!files.empty())
//            handleTask(new ScanFilesTask(ModelEvent::UpdateFiles, this, files));

        beginInsertRows(parent, m_items.size(), m_items.size() + list.size() - 1);
        m_items.insert(m_items.begin(), list.begin(), list.end());
        endInsertRows();
    }
}

bool QueryResultsNode::canFetchMore(const QModelIndex &parent) const
{
    return !parent.isValid() && !m_reader.eof();
}

Qt::ItemFlags QueryResultsNode::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QAbstractItemModel *QueryResultsNode::model() const
{
    return const_cast<QueryResultsNode *>(this);
}

const QueryResultsNode::Geometry &QueryResultsNode::geometry() const
{
    return m_geometry;
}

const QueryResultsNode::Sorting &QueryResultsNode::sorting() const
{
    return m_sorting;
}

QModelIndex QueryResultsNode::currentIndex() const
{
    return m_currentIndex;
}

void QueryResultsNode::setCurrentIndex(const QModelIndex &index)
{
    m_currentIndex = index;
}

Interface::Holder QueryResultsNode::search(const QModelIndex &index, QWidget *parent)
{
    return Interface::Holder();
}

Interface::Holder QueryResultsNode::activated(const QModelIndex &index, QWidget *parent)
{
    return Interface::Holder();
}

QueryResultsNode::size_type QueryResultsNode::size() const
{
    return m_items.size();
}

QueryResultsNode::Item *QueryResultsNode::at(size_type index) const
{
    return m_items.at(index);
}

QueryResultsNode::size_type QueryResultsNode::indexOf(Item *item) const
{
    for (auto i = m_items.begin(), end = m_items.end(); i != end; ++i)
        if (*i == item)
            return i - m_items.begin();

    return InvalidIndex;
}

Interface::Holder QueryResultsNode::node(const Interface::Holder &file) const
{
    return Interface::Holder();
}

void QueryResultsNode::setNode(const Interface::Holder &file, const Interface::Holder &node)
{}

}}