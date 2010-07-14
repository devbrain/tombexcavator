#ifndef __MODELS_VFS_MODEL_HPP__
#define __MODELS_VFS_MODEL_HPP__

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include "libvfs/inode_defs.hpp"

namespace vfs
{
  class fs_c;
  class fs_object_c;
} // ns vfs

class vfs_model_c : public QAbstractItemModel
{
  Q_OBJECT
public:
  vfs_model_c (vfs::fs_c* fs, QObject* parent = 0);

  QModelIndex index (int row, 
		     int column, 
		     const QModelIndex& parent) const;
  
  QModelIndex parent (const QModelIndex &index) const;

  Qt::ItemFlags flags (const QModelIndex& index) const;
  QVariant      data  (const QModelIndex& index, int role) const;
  QVariant      headerData (int section, 
			    Qt::Orientation orientation, 
			    int role) const;
  int           rowCount (const QModelIndex& parent) const;
  int           columnCount (const QModelIndex & parent) const;
private:
  typedef QPair <vfs::fs_object_c*, int> parent_info_t;
  typedef QMap <vfs::inode_num_t, parent_info_t> parent_map_t;

  typedef QPair <vfs::fs_object_c*, QString> item_info_t;

  typedef QMap <vfs::inode_num_t, item_info_t> items_map_t;
private:
  mutable vfs::fs_c*    m_fs;
  mutable items_map_t   m_items;
  mutable parent_map_t  m_parents;
};


#endif
