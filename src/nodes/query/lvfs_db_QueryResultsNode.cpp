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

#include "lvfs_db_QueryResultsNode.h"

#include "../../model/items/lvfs_db_FileItem.h"
#include "../../model/items/lvfs_db_ValueItem.h"
#include "../../model/items/lvfs_db_PropertyItem.h"

#include "../../gui/value/lvfs_db_SelectValueDialog.h"
#include "../../lvfs_db_common.h"

#include <lvfs/IEntry>
#include <lvfs/IDirectory>
#include <lvfs-core/IView>
#include <lvfs-core/models/Qt/IView>

#include <QtGui/QMessageBox>


namespace LVFS {
namespace Db {

using namespace LiquidDb;


QueryResultsNode::QueryResultsNode(const Interface::Adaptor<IStorage> &storage, const EntityValueReader &reader, const Interface::Holder &parent) :
    ValueModel(storage, reader),
    Complements(storage.interface(), parent),
    m_geometry(),
    m_sorting(0, ::Qt::AscendingOrder)
{
    ASSERT(m_sorting.first < columnCount(QModelIndex()));
}

QueryResultsNode::~QueryResultsNode()
{}

void QueryResultsNode::refresh()
{
    if (m_reader.entity().type() == Entity::Composite)
    {
        QModelIndex idx;
        PropertyItem *property;

        for (auto &item : ValueModel::m_items)
            for (Db::Item::size_type i = 0, size = item->size(); i < size; ++i)
                if (m_storage->schema((property = static_cast<PropertyItem *>(item->at(i)))->entity()) == IStorage::Path)
                {
                    for (Db::Item::size_type i = 0, size = property->size(); i < size; ++i)
                        if (property->at<FileItem>(i)->refresh(m_storage))
                        {
                            idx = index(property->at<FileItem>(i));
                            emit dataChanged(idx, idx);
                        }

                    break;
                }
    }
}

void QueryResultsNode::opened(const Interface::Holder &view)
{

}

void QueryResultsNode::closed(const Interface::Holder &view)
{

}

Interface::Holder QueryResultsNode::accept(const Interface::Holder &view, Core::INode::Files &files)
{
    QModelIndex index = view->as<Core::Qt::IView>()->currentIndex();

    if (index.isValid())
    {
        Db::Item *item = static_cast<Db::Item *>(index.internalPointer());
        PropertyItem *property;

        if (item->parent())
            do
                item = item->parent();
            while (item->parent());

        for (Db::Item::size_type i = 0, size = item->size(); i < size; ++i)
            if (m_storage->schema((property = item->at<PropertyItem>(i))->entity()) == IStorage::Path)
            {
                char buffer[Module::MaxUriLength];
                char prefix[Module::MaxUriLength];

                if (property->size() > 0)
                {
                    ::strcpy(prefix, property->at<ValueItem>(0)->value().value().asString());

                    if (char * tok = ::strrchr(prefix, '/'))
                        tok[1] = 0;
                    else
                        prefix[0] = 0;
                }
                else
                    prefix[0] = 0;

                Interface::Holder dest = m_storage->file()->as<IDirectory>()->entry(prefix);

                if (!dest.isValid())
                {
                    QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->file()->as<IDirectory>()->lastError().description()));
                    return Interface::Holder();
                }

                if (m_storage->transaction())
                {
                    EntityValue localValue;
                    EntityValue::List list;
                    ValueModel::Files dbFiles;

                    for (auto &i : files)
                        for (auto &j : i.second)
                        {
                            if (UNLIKELY(std::snprintf(buffer, sizeof(buffer), "%s%s", prefix, j->as<IEntry>()->title()) < 0))
                            {
                                m_storage->rollback();
                                return Interface::Holder();
                            }

                            localValue = m_storage->addValue(property->entity(), buffer);

                            if (localValue.isValid())
                            {
                                list.push_back(localValue);
                                dbFiles[localValue.id()] = j;
                            }
                            else
                            {
                                QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
                                m_storage->rollback();
                                return Interface::Holder();
                            }
                        }

                    if (m_storage->addValue(static_cast<ValueItem *>(item)->value(), list))
                    {
                        EntityValueDialog dialog(m_storage, static_cast<ValueItem *>(item)->value(), dbFiles, view->as<Core::IView>()->widget());

                        if (dialog.exec() != EntityValueDialog::Accepted)
                        {
                            m_storage->rollback();
                            return Interface::Holder();
                        }
                        else
                            if (m_storage->commit())
                            {
                                index = this->index(item);
                                localValue = static_cast<ValueItem *>(item)->value();

                                beginRemoveRows(index, 0, item->size());
                                static_cast<ValueItem *>(item)->clear();
                                endRemoveRows();

                                beginInsertColumns(index, 0, localValue.entity().properties().size() - 1);
                                static_cast<ValueItem *>(item)->reset(m_storage, localValue);
                                endInsertRows();

                                return dest;
                            }
                            else
                            {
                                QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
                                m_storage->rollback();
                                return Interface::Holder();
                            }

                        m_storage->setEditorGeometry(static_cast<ValueItem *>(item)->value().entity(), fromQRect(dialog.geometry()));
                    }
                    else
                    {
                        QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
                        m_storage->rollback();
                        return Interface::Holder();
                    }
                }
                else
                {
                    QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
                    return Interface::Holder();
                }
            }
    }

    return Interface::Holder();
}

void QueryResultsNode::clear()
{

}

Interface::Holder QueryResultsNode::node(const Interface::Holder &file) const
{
    return Interface::Holder();
}

void QueryResultsNode::setNode(const Interface::Holder &file, const Interface::Holder &node)
{}

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

Interface::Holder QueryResultsNode::activated(const Interface::Holder &view, const QModelIndex &index)
{
    ASSERT(index.isValid());
    Db::Item *item = static_cast<Db::Item *>(index.internalPointer());

    if (item->isPath())
        static_cast<FileItem *>(item)->open();
    else
    {
        if (item->parent())
            do
                item = item->parent();
            while (item->parent());

        for (int i = 0, size = item->size(); i < size; ++i)
            if (m_storage->schema(static_cast<PropertyItem *>(item->at(i))->entity()) == IStorage::Path)
            {
                view->as<Core::Qt::IView>()->select(this->index(item->at(i)), true);
                break;
            }
    }

    return Interface::Holder();
}

Interface::Holder QueryResultsNode::search(const Interface::Holder &view, const QModelIndex &index)
{
    return Interface::Holder();
}

void QueryResultsNode::insert(const Interface::Holder &view, const QModelIndex &index)
{
    ASSERT(index.isValid());
    Db::Item *item = static_cast<Db::Item *>(index.internalPointer());

    if (item->isProperty() && m_storage->schema(static_cast<PropertyItem *>(item)->entity()) != IStorage::Path)
    {
        PropertyItem *property = static_cast<PropertyItem *>(item);

        if (m_storage->transaction())
        {
            EntityValueReader reader(m_storage->entityValues(property->entity()));
            SelectValueDialog dialog(m_storage, reader, view->as<Core::IView>()->widget());

            if (dialog.exec() == SelectValueDialog::Accepted)
            {
                EntityValue value = dialog.value();

                if (m_storage->addValue(static_cast<ValueItem *>(property->parent())->value(), value))
                    if (m_storage->commit())
                    {
                        beginInsertRows(index, property->size(), property->size());
                        property->add(value);
                        endInsertRows();
                    }
                    else
                    {
                        QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
                        m_storage->rollback();
                    }
                else
                {
                    QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
                    m_storage->rollback();
                }
            }
            else
                m_storage->rollback();
        }
        else
            QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
    }
}

void QueryResultsNode::remove(const Interface::Holder &view, const QModelIndex &index)
{
    ASSERT(index.isValid());
    Db::Item *item = static_cast<Db::Item *>(index.internalPointer());

    if (item->isValue() && m_storage->schema(static_cast<ValueItem *>(item)->value().entity()) != IStorage::Path)
    {
        if (m_storage->transaction())
        {
            ValueItem *valueItem = static_cast<ValueItem *>(item);
            PropertyItem *property = static_cast<PropertyItem *>(valueItem->parent());

            if (m_storage->removeValue(static_cast<ValueItem *>(property->parent())->value(), valueItem->value()))
            {
                PropertyItem::size_type idx = property->indexOf(valueItem);

                beginRemoveRows(ValueModel::parent(index), idx, idx);
                property->remove(idx);
                endRemoveRows();
            }
            else
            {
                QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
                m_storage->rollback();
                return;
            }

            if (!m_storage->commit())
            {
                QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
                m_storage->rollback();
            }
        }
        else
            QMessageBox::critical(view->as<Core::IView>()->widget(), tr("Error"), toUnicode(m_storage->lastError()));
    }
}

}}
