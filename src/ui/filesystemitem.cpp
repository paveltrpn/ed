#include "filesystemitem.h"

namespace tire {

// ======================================================================================
// ==================== DirSortFilterProxy (internal) ===================================
// ======================================================================================

struct DirSortFilterProxy : QSortFilterProxyModel {
    QString filterText{};

    explicit DirSortFilterProxy( QAbstractItemModel* source, QObject* parent = nullptr )
        : QSortFilterProxyModel{ parent } {
        //
        setDynamicSortFilter( true );
        setSourceModel( source );
        sort( 0, Qt::AscendingOrder );
    }

    auto applyFilterText( const QString& text ) -> void {
        beginFilterChange();
        filterText = text;
        endFilterChange();
    }

protected:
    // Sort order: "." and ".." always first ("." before ".."), then directories,
    // then files; entries within each group ordered by case-insensitive name
    // compare, with case-sensitive name compare as a tie-breaker.
    auto lessThan( const QModelIndex& left, const QModelIndex& right ) const -> bool override {
        const auto leftName = sourceModel()->data( left, QFileSystemModel::FileNameRole ).toString();
        const auto rightName = sourceModel()->data( right, QFileSystemModel::FileNameRole ).toString();

        const auto isDot = []( const QString& n ) -> bool {
            //
            return n == "." || n == "..";
        };

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

    auto filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const -> bool override {
        if ( !sourceParent.isValid() ) {
            return true;
        }

        const auto nameIndex = sourceModel()->index( sourceRow, 0, sourceParent );
        const auto name = sourceModel()->data( nameIndex, QFileSystemModel::FileNameRole ).toString();

        if ( name == "." || name == ".." ) {
            return true;
        }

        if ( filterText.isEmpty() ) {
            return true;
        }

        return name.contains( filterText, Qt::CaseInsensitive );
    }
};

// ======================================================================================
// ==================== FlatDirProxyModel ===============================================
// ======================================================================================

FlatDirProxyModel::FlatDirProxyModel( QFileSystemModel* source, QObject* parent )
    : QAbstractListModel{ parent }
    , _source{ source }
    , _sortFilter{ new DirSortFilterProxy{ source, this } } {
    //
    connect( _sortFilter, &QAbstractItemModel::dataChanged, this,
             [this]( const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles ) -> void {
                 if ( topLeft.parent() != _root ) {
                     return;
                 }
                 emit dataChanged( index( topLeft.row() ), index( bottomRight.row() ), roles );
             } );

    connect( _sortFilter, &QAbstractItemModel::rowsAboutToBeInserted, this,
             [this]( const QModelIndex& parent, int first, int last ) -> void {
                 if ( parent != _root ) {
                     return;
                 }
                 beginInsertRows( QModelIndex{}, first, last );
             } );

    connect( _sortFilter, &QAbstractItemModel::rowsInserted, this,
             [this]( const QModelIndex& parent, int, int ) -> void {
                 if ( parent != _root ) {
                     return;
                 }
                 endInsertRows();
             } );

    connect( _sortFilter, &QAbstractItemModel::rowsAboutToBeRemoved, this,
             [this]( const QModelIndex& parent, int first, int last ) -> void {
                 if ( parent != _root ) {
                     return;
                 }
                 beginRemoveRows( QModelIndex{}, first, last );
             } );

    connect( _sortFilter, &QAbstractItemModel::rowsRemoved, this,
             [this]( const QModelIndex& parent, int, int ) -> void {
                 if ( parent != _root ) {
                     return;
                 }
                 endRemoveRows();
             } );

    connect( _sortFilter, &QAbstractItemModel::modelAboutToBeReset, this, [this]() -> void {
        //
        beginResetModel();
    } );

    connect( _sortFilter, &QAbstractItemModel::modelReset, this, [this]() -> void {
        //
        endResetModel();
    } );

    connect( _sortFilter, &QAbstractItemModel::layoutAboutToBeChanged, this, [this]() -> void {
        //
        beginResetModel();
    } );

    connect( _sortFilter, &QAbstractItemModel::layoutChanged, this, [this]() -> void {
        //
        endResetModel();
    } );
}

auto FlatDirProxyModel::filterText() const -> QString {
    return static_cast<DirSortFilterProxy*>( _sortFilter )->filterText;
}

auto FlatDirProxyModel::setFilterText( const QString& text ) -> void {
    static_cast<DirSortFilterProxy*>( _sortFilter )->applyFilterText( text );
}

auto FlatDirProxyModel::setRootIndex( const QModelIndex& sourceIndex ) -> void {
    const auto proxyIndex = _sortFilter->mapFromSource( sourceIndex );

    if ( proxyIndex == _root ) {
        return;
    }

    beginResetModel();
    _root = proxyIndex;
    endResetModel();
}

auto FlatDirProxyModel::rowCount( const QModelIndex& parent ) const -> int {
    if ( parent.isValid() ) {
        return 0;
    }

    return _sortFilter->rowCount( _root );
}

auto FlatDirProxyModel::data( const QModelIndex& index, int role ) const -> QVariant {
    if ( !index.isValid() ) {
        return QVariant{};
    }

    const auto proxyIndex = _sortFilter->index( index.row(), 0, _root );
    const auto srcIndex = _sortFilter->mapToSource( proxyIndex );

    if ( role == IsDirectoryRole ) {
        return _source->isDir( srcIndex );
    }

    if ( role == IsHiddenRole ) {
        return _source->fileInfo( srcIndex ).isHidden();
    }

    if ( role == PermissionsRole ) {
        return QVariant::fromValue( _source->permissions( srcIndex ) );
    }

    if ( role == SizeRole ) {
        return _source->data( srcIndex.siblingAtColumn( 1 ), Qt::DisplayRole );
    }

    if ( role == TypeRole ) {
        return _source->data( srcIndex.siblingAtColumn( 2 ), Qt::DisplayRole );
    }

    if ( role == LastModifiedRole ) {
        return _source->data( srcIndex.siblingAtColumn( 3 ), Qt::DisplayRole );
    }

    return _sortFilter->data( proxyIndex, role );
}

auto FlatDirProxyModel::roleNames() const -> QHash<int, QByteArray> {
    auto roles = QAbstractListModel::roleNames();

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

// ======================================================================================
// ==================== FilesystemItem ==================================================
// ======================================================================================

FilesystemItem::FilesystemItem( QQuickItem* parent )
    : _filesystemModel{ new QFileSystemModel{ this } }
    , _proxyModel{ new FlatDirProxyModel{ _filesystemModel, this } }
    , _nowDirectory{ QDir::homePath() } {
    //
    _filesystemModel->setFilter( QDir::AllEntries | QDir::AllDirs /*| QDir::Hidden*/ | QDir::System );
    _filesystemModel->setNameFilterDisables( true );

    _filesystemModel->setRootPath( _nowDirectory.path() );

    _proxyModel->setRootIndex( _filesystemModel->index( _nowDirectory.path() ) );

    connect( _filesystemModel, &QFileSystemModel::directoryLoaded, this, [this]( const QString& path ) -> void {
        if ( path == _nowDirectory.path() ) {
            _proxyModel->setRootIndex( _filesystemModel->index( _nowDirectory.path() ) );
        }
    } );
}

auto FilesystemItem::filesystemModel() const -> QAbstractItemModel* {
    return _proxyModel;
}

auto FilesystemItem::nowDirectory() const -> QString {
    return _nowDirectory.path();
}

auto FilesystemItem::setNowDirectory( const QString& value ) -> void {
    const auto dir = QDir{ QDir::cleanPath( value ) };

    if ( dir == _nowDirectory ) {
        return;
    }

    _nowDirectory = dir;

    _filesystemModel->setRootPath( _nowDirectory.path() );

    _proxyModel->setRootIndex( _filesystemModel->index( _nowDirectory.path() ) );

    emit nowDirectoryChanged();
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
