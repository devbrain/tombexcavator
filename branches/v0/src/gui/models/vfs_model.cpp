#include <QtCore/QDebug>
#include <QtGui/QStyle>
#include <QtGui/QApplication>

#include "gui/models/vfs_model.hpp"

#include "libvfs/fs.hpp"
#include "libvfs/fs_file.hpp"
#include "libvfs/fs_dir.hpp"

vfs_model_c::vfs_model_c (vfs::fs_c* fs, QObject* parent)
  : QAbstractItemModel (parent),
    m_fs (fs)
{
}
// ------------------------------------------------------------
QModelIndex vfs_model_c::index (int row, 
				int column, 
				const QModelIndex& parent) const
{
  vfs::fs_object_c* parent_item = 0;
  int prow = -1;
  if (!parent.isValid ())
    {
      parent_item = m_fs->root_object ();
    }
  else
    {
      parent_item = (vfs::fs_object_c*)parent.internalPointer ();
      prow = parent.row ();
    }
  vfs::fs_object_c* child_item = 0;
  if (parent_item->type () == vfs::fs_object_c::eDIRECTORY)
    {
      vfs::fs_dir_c* parent_dir = dynamic_cast <vfs::fs_dir_c*> (parent_item);
      if (row < parent_dir->size ())
	{
	  vfs::inode_num_t ino;
	  std::string      name;
	  parent_dir->read (row, name, ino);
	  items_map_t::const_iterator i = m_items.find (ino);
	  if (i == m_items.end ())
	    {
	      child_item = parent_dir->load_object (ino);
	      
	      m_parents.insert (ino, 
				parent_info_t (parent_item, prow)
				);
	      m_items.insert (ino, 
			      item_info_t (child_item,
					   QString (name.c_str ())
					   )
			      );
	    }
	  else
	    {
	      child_item = i.value ().first;
	    }
	}
    }
  if (child_item)
    {
      return createIndex (row, column, child_item);
    }
  return QModelIndex ();
}
// ------------------------------------------------------------
QModelIndex vfs_model_c::parent (const QModelIndex &index) const
{
  if (!index.isValid())
    {
      return QModelIndex();
    }
  vfs::fs_object_c* item = (vfs::fs_object_c*) index.internalPointer ();
  vfs::inode_num_t  ino = item->inode_num ();
  parent_map_t::const_iterator i = m_parents.find (ino);
  if (i == m_parents.end ())
    {
      return QModelIndex ();
    }
  parent_info_t pinfo = i.value ();
  vfs::fs_object_c* parent_item = pinfo.first;
  int               prow        = pinfo.second;
  if (prow == -1)
    {
      return QModelIndex ();
    }
  return createIndex (prow, 0, parent_item);
}
// ------------------------------------------------------------
Qt::ItemFlags vfs_model_c::flags (const QModelIndex& index) const
{
  return Qt::ItemIsEnabled;
}
// ------------------------------------------------------------
QVariant vfs_model_c::data (const QModelIndex& index, int role) const
{
  if (!index.isValid())
    {
      return QVariant();
    }
  vfs::fs_object_c* fso = (vfs::fs_object_c*)index.internalPointer ();
  vfs::inode_num_t  ino = fso->inode_num ();
  items_map_t::const_iterator i = m_items.find (ino);
  if (i == m_items.end ())
    {
      return QVariant();
    }
  QString name = i.value ().second;
  int column = index.column ();
  if (role == Qt::DisplayRole)
    {
      if (column == 0)
	{
	  return name;
	}
      if (column == 1)
	{
	  if (fso->type () == vfs::fs_object_c::eFILE)
	    {
	      return fso->size ();
	    }
	  else
	    {
	      return QVariant ();
	    }
	  
	}
    }
  if (role == Qt::DecorationRole && column == 0)
    {
      if (fso->type () == vfs::fs_object_c::eFILE)
	{
	  return qApp->style ()->standardIcon (QStyle::SP_FileIcon);
	}
      else
	{
	  return qApp->style ()->standardIcon (QStyle::SP_DirIcon);
	}
    }

  return QVariant();
}
// ------------------------------------------------------------
QVariant vfs_model_c::headerData (int section, 
				  Qt::Orientation orientation, 
				  int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
    switch (section)
      {
      case 0:
	return QVariant ("Name");
      case 1:
	return QVariant ("Size");
      };
    }
  return QVariant();
}
// ------------------------------------------------------------
int vfs_model_c::rowCount (const QModelIndex& parent) const
{
  vfs::fs_object_c* parent_item = 0;
  if (!parent.isValid ())
    {
      parent_item = m_fs->root_object ();
    }
  else
    {
      parent_item = (vfs::fs_object_c*)parent.internalPointer ();
    }
  if (parent_item->type () == vfs::fs_object_c::eDIRECTORY)
    {
      return parent_item->size ();
    }
  return 0;
}
// ------------------------------------------------------------
int vfs_model_c::columnCount (const QModelIndex & parent) const
{
  return 2;
}
