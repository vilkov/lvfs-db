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

#include "lvfs_db_SelectableValueListDialog.h"
#include "../../../../model/items/lvfs_db_FileItem.h"


SelectableValueListDialog::SelectableValueListDialog(const Interface::Adaptor<IStorage> &container, const EntityValueReader &reader, QWidget *parent) :
    EditableValueListDialog(container, reader, parent)
{}

void SelectableValueListDialog::dblClick()
{
    QModelIndex index = currentIndex();

    if (index.isValid())
    {
        Item *item = static_cast<Item *>(index.internalPointer());

        if (item->isPath() && item->parent() != NULL)
            static_cast<FileItem *>(item)->open();
        else
        {
            if (item->parent())
            {
                do
                    item = static_cast<Item *>(item->parent());
                while (item->parent());

                setCurrentIndex(item);
            }

            accept();
        }
    }
    else
        warning(tr("You must choose one of the values."));
}
