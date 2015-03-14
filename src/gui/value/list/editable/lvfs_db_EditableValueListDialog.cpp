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

#include "lvfs_db_EditableValueListDialog.h"
#include "../../../../model/items/lvfs_db_FileItem.h"


EditableValueListDialog::EditableValueListDialog(const Interface::Adaptor<IStorage> &storage, const EntityValueReader &reader, QWidget *parent) :
    NestedPlainDialog(parent),
    m_handler(this),
    m_widget(storage, reader, &m_handler, this)
{
    QRect geometry = toQRect(storage->listGeometry(m_widget.entity()));

    if (geometry.isValid())
        setGeometry(geometry);

    setWindowTitle(tr("Values of \"%1\"").arg(toUnicode(m_widget.entity().name())));

    m_handler.registerMouseDoubleClickEventHandler(&EditableValueListDialog::dblClick);
    m_handler.registerShortcut(Qt::NoModifier, Qt::Key_Insert, &EditableValueListDialog::addValue);
    m_handler.registerShortcut(Qt::NoModifier, Qt::Key_Delete, &EditableValueListDialog::removeValue);
    m_handler.registerShortcut(Qt::CTRL, Qt::Key_F, &EditableValueListDialog::setFocusToFilter);
    m_widget.setViewToolTip(tr("INS - add value\nDEL - remove value\nCTRL+F - activate filter field"));

    setCentralWidget(&m_widget);
}

EditableValueListDialog::~EditableValueListDialog()
{}

EntityValue EditableValueListDialog::takeValue()
{
    return m_widget.takeValue();
}

QModelIndex EditableValueListDialog::currentIndex() const
{
    return m_widget.currentIndex();
}

void EditableValueListDialog::setCurrentIndex(Item *item)
{
    m_widget.setCurrentIndex(item);
}

void EditableValueListDialog::dblClick()
{
    QModelIndex index = currentIndex();

    if (index.isValid())
    {
        Item *item = static_cast<Item *>(index.internalPointer());

        if (item->isPath())
            static_cast<FileItem *>(item)->open();
    }
}

void EditableValueListDialog::addValue()
{
    m_widget.addValue();
}

void EditableValueListDialog::removeValue()
{
    m_widget.removeValue();
}

void EditableValueListDialog::setFocusToFilter()
{
    m_widget.setFocusToFilter();
}
