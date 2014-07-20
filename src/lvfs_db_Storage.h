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

#ifndef LVFS_DB_STORAGE_H_
#define LVFS_DB_STORAGE_H_

#include <lvfs/IEntry>
#include <lvfs/IFsFile>
#include <lvfs/IDirectory>
#include <lvfs-db/IStorage>


namespace LVFS {
namespace Db {

using namespace LiquidDb;


class PLATFORM_MAKE_PRIVATE Storage : public Implements<IEntry, IDirectory, IFsFile, IStorage>
{
public:
    Storage(const Interface::Holder &file, const Interface::Holder &storage);
    virtual ~Storage();

    /* IEntry */

    virtual const char *title() const;
    virtual const char *schema() const;
    virtual const char *location() const;
    virtual const IType *type() const;

    /* IDirectory */

    virtual const_iterator begin() const;
    virtual const_iterator end() const;
    virtual Interface::Holder entry(const char *name) const;

    virtual bool rename(const Interface::Holder &file, const char *name);
    virtual bool remove(const Interface::Holder &file);

    /* IFsFile */

    virtual time_t cTime() const;
    virtual time_t mTime() const;
    virtual time_t aTime() const;

    virtual int permissions() const;
    virtual bool setPermissions(int value);

    /* IStorage */

    virtual const LiquidDb::Storage::Entities &entities() const;

    virtual bool transaction();
    virtual bool commit();
    virtual void rollback();

    virtual EntityValueReader entityValues(const Entity &entity) const;
    virtual EntityValueReader entityValues(const Entity &entity, const Constraint &constraint) const;

    virtual Schemas schema(const Entity &entity) const;
    virtual bool setSchema(const Entity &entity, Schemas schema);

    virtual Entity createEntity(Entity::Type type, const EFC::String &name, const EFC::String &title);
    virtual bool removeEntity(const Entity &entity);

    virtual bool addProperty(const Entity &entity, const Entity &property, const EFC::String &name);
    virtual bool renameProperty(const Entity &entity, const Entity &property, const EFC::String &name);
    virtual bool removeProperty(const Entity &entity, const Entity &property);

    virtual EntityValue addValue(const Entity &entity);
    virtual bool addValue(const EntityValue &entityValue, const EntityValue &propertyValue);
    virtual bool addValue(const EntityValue &entityValue, const EntityValue::List &propertyValues);
    virtual EntityValue addValue(const Entity &entity, const EFC::Variant &value);
    virtual bool updateValue(const EntityValue &value, const EFC::Variant &newValue);
    virtual bool removeValue(const Entity &entity, const Entity::IdsList &ids);
    virtual bool removeValue(const EntityValue &entityValue, const EntityValue &propertyValue);

    /* COMMON */

    virtual const Error &lastError() const;

private:
    Interface::Holder m_file;
    Interface::Holder m_fileStorage;
    LiquidDb::Storage m_storage;
    mutable Error m_error;
    mutable const Error *m_lastError;
};

}}

#endif /* LVFS_DB_STORAGE_H_ */