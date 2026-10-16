#include "filesystemitem.h"

namespace tire {

// ======================================================================================
// ==================== FlatDirProxyModel ===============================================
// ======================================================================================

FlatDirProxyModel::FlatDirProxyModel( QAbstractItemModel* source, QObject* parent )
    : QSortFilterProxyModel{ parent } {
    //
    setDynamicSortFilter( true );
    setSourceModel( source );
    sort( 0, Qt::AscendingOrder );
}

auto FlatDirProxyModel::filterText() const -> QString {
    return _filterText;
}

auto FlatDirProxyModel::setFilterText( const QString& text ) -> void {
    beginFilterChange();
    _filterText = text;
    endFilterChange();
}

auto FlatDirProxyModel::lessThan( const QModelIndex& left, const QModelIndex& right ) const -> bool {
    const auto leftName = sourceModel()->data( left, QFileSystemModel::FileNameRole ).toString();
    const auto rightName = sourceModel()->data( right, QFileSystemModel::FileNameRole ).toString();

    const auto isDot = []( const QString& n ) { return n == "." || n == ".."; };
    const bool leftDot = isDot( leftName ), rightDot = isDot( rightName );
    if ( leftDot || rightDot ) {
        if ( leftDot && rightDot ) {
            return leftName == ".";
        }
        return leftDot;
    }

    const auto* fsm = qobject_cast<QFileSystemModel*>( sourceModel() );
    const bool leftDir = fsm && fsm->isDir( left );
    const bool rightDir = fsm && fsm->isDir( right );
    if ( leftDir != rightDir ) {
        return leftDir;
    }

    const int cmp = leftName.compare( rightName, Qt::CaseInsensitive );
    if ( cmp != 0 ) {
        return cmp < 0;
    }
    return leftName < rightName;
}

auto FlatDirProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const -> bool {
    if ( !sourceParent.isValid() ) {
        return true;
    }

    const auto nameIndex = sourceModel()->index( sourceRow, 0, sourceParent );
    const auto name = sourceModel()->data( nameIndex, QFileSystemModel::FileNameRole ).toString();

    if ( name == "." || name == ".." ) {
        return true;
    }

    if ( _filterText.isEmpty() ) {
        return true;
    }

    return name.contains( _filterText, Qt::CaseInsensitive );
}

auto FlatDirProxyModel::roleNames() const -> QHash<int, QByteArray> {
    auto roles = QSortFilterProxyModel::roleNames();

    roles[QFileSystemModel::FileNameRole] = "name";
    roles[QFileSystemModel::FilePathRole] = "path";
    roles[IsDirectoryRole] = "isDirectory";
    roles[IsHiddenRole] = "isHidden";
    roles[SizeRole] = "size";
    roles[TypeRole] = "type";
    roles[LastModifiedRole] = "lastModified";
    roles[PermissionsRole] = "permissions";

    return roles;
}

auto FlatDirProxyModel::data( const QModelIndex& proxyIndex, int role ) const -> QVariant {
    const auto* fsm = qobject_cast<QFileSystemModel*>( sourceModel() );
    const auto srcIndex = mapToSource( proxyIndex );

    if ( role == IsDirectoryRole ) {
        if ( fsm ) {
            return fsm->isDir( srcIndex );
        }
        return false;
    }

    if ( role == IsHiddenRole ) {
        if ( fsm ) {
            return fsm->fileInfo( srcIndex ).isHidden();
        }
        return false;
    }

    if ( role == PermissionsRole ) {
        if ( fsm ) {
            return QVariant::fromValue( fsm->permissions( srcIndex ) );
        }
        return QVariant{};
    }

    if ( role == SizeRole ) {
        if ( fsm ) {
            return fsm->data( srcIndex.siblingAtColumn( 1 ), Qt::DisplayRole );
        }
        return QVariant{};
    }

    if ( role == TypeRole ) {
        if ( fsm ) {
            return fsm->data( srcIndex.siblingAtColumn( 2 ), Qt::DisplayRole );
        }
        return QVariant{};
    }

    if ( role == LastModifiedRole ) {
        if ( fsm ) {
            return fsm->data( srcIndex.siblingAtColumn( 3 ), Qt::DisplayRole );
        }
        return QVariant{};
    }

    return QSortFilterProxyModel::data( proxyIndex, role );
}

// ======================================================================================
// ==================== DirListModel ====================================================
// ======================================================================================

DirListModel::DirListModel( QAbstractItemModel* source, QObject* parent )
    : QAbstractListModel{ parent }
    , _source{ source } {
    //
    connect( _source, &QAbstractItemModel::dataChanged, this,
             [this]( const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles ) {
                 if ( topLeft.parent() != _root ) {
                     return;
                 }
                 emit dataChanged( index( topLeft.row() ), index( bottomRight.row() ), roles );
             } );

    connect( _source, &QAbstractItemModel::rowsAboutToBeInserted, this,
             [this]( const QModelIndex& parent, int first, int last ) {
                 if ( parent != _root ) {
                     return;
                 }
                 beginInsertRows( QModelIndex{}, first, last );
             } );

    connect( _source, &QAbstractItemModel::rowsInserted, this, [this]( const QModelIndex& parent, int, int ) {
        if ( parent != _root ) {
            return;
        }
        endInsertRows();
    } );

    connect( _source, &QAbstractItemModel::rowsAboutToBeRemoved, this,
             [this]( const QModelIndex& parent, int first, int last ) {
                 if ( parent != _root ) {
                     return;
                 }
                 beginRemoveRows( QModelIndex{}, first, last );
             } );

    connect( _source, &QAbstractItemModel::rowsRemoved, this, [this]( const QModelIndex& parent, int, int ) {
        if ( parent != _root ) {
            return;
        }
        endRemoveRows();
    } );

    connect( _source, &QAbstractItemModel::modelAboutToBeReset, this, [this] { beginResetModel(); } );
    connect( _source, &QAbstractItemModel::modelReset, this, [this] { endResetModel(); } );

    connect( _source, &QAbstractItemModel::layoutAboutToBeChanged, this, [this] { beginResetModel(); } );
    connect( _source, &QAbstractItemModel::layoutChanged, this, [this] { endResetModel(); } );
}

auto DirListModel::setRootIndex( const QModelIndex& index ) -> void {
    if ( index == _root ) {
        return;
    }

    beginResetModel();
    _root = index;
    endResetModel();
}

auto DirListModel::rowCount( const QModelIndex& parent ) const -> int {
    if ( parent.isValid() ) {
        return 0;
    }

    return _source->rowCount( _root );
}

auto DirListModel::data( const QModelIndex& index, int role ) const -> QVariant {
    if ( !index.isValid() ) {
        return QVariant{};
    }

    const auto srcIndex = _source->index( index.row(), 0, _root );
    return _source->data( srcIndex, role );
}

auto DirListModel::roleNames() const -> QHash<int, QByteArray> {
    return _source->roleNames();
}

// ======================================================================================
// ==================== FilesystemItem ==================================================
// ======================================================================================

FilesystemItem::FilesystemItem( QQuickItem* parent )
    : _filesystemModel{ new QFileSystemModel{ this } }
    , _proxyModel{ new FlatDirProxyModel{ _filesystemModel, this } }
    , _listModel{ new DirListModel{ _proxyModel, this } }
    , _nowDirectory{ QDir::homePath() } {
    //
    _filesystemModel->setFilter( QDir::AllEntries | QDir::AllDirs | QDir::Hidden | QDir::System );
    _filesystemModel->setNameFilterDisables( true );

    _filesystemModel->setRootPath( _nowDirectory.path() );

    _listModel->setRootIndex( rootIndex() );

    connect( _filesystemModel, &QFileSystemModel::directoryLoaded, this, [this]( const QString& path ) {
        if ( path == _nowDirectory.path() ) {
            _listModel->setRootIndex( rootIndex() );
            emit rootIndexChanged();
        }
    } );
}

auto FilesystemItem::filesystemModel() const -> QAbstractItemModel* {
    return _listModel;
}

auto FilesystemItem::rootIndex() const -> QModelIndex {
    return _proxyModel->mapFromSource( _filesystemModel->index( _nowDirectory.path() ) );
}

auto FilesystemItem::nowDirectory() const -> QString {
    return _nowDirectory.path();
}

auto FilesystemItem::setNowDirectory( const QString& value ) -> void {
    if ( QDir{ value } == _nowDirectory ) {
        return;
    }

    _nowDirectory = QDir{ value };

    _filesystemModel->setRootPath( _nowDirectory.path() );

    _listModel->setRootIndex( rootIndex() );

    emit nowDirectoryChanged();
    emit rootIndexChanged();
}

auto FilesystemItem::filterText() const -> QString {
    return _proxyModel->filterText();
}

auto FilesystemItem::setFilterText( const QString& value ) -> void {
    if ( _proxyModel->filterText() == value ) {
        return;
    }

    _proxyModel->setFilterText( value );

    emit filterTextChanged();
}

}  // namespace tire
