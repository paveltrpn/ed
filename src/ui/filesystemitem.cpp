#include "filesystemitem.h"

namespace tire {

// ======================================================================================
// ==================== FilesystemItem ==================================================
// ======================================================================================

// ======================================================================================
// ==================== FlatDirProxyModel ===============================================
// ======================================================================================

FlatDirProxyModel::FlatDirProxyModel( QAbstractItemModel* source, QObject* parent )
    : QAbstractProxyModel{ parent } {
    setSourceModel( source );

    connect( source, &QAbstractItemModel::modelAboutToBeReset, this, [this] { beginResetModel(); } );
    connect( source, &QAbstractItemModel::modelReset, this, [this] { endResetModel(); } );

    connect( source, &QAbstractItemModel::rowsAboutToBeInserted, this,
             [this]( const QModelIndex& parent, int first, int last ) {
                 if ( parent == _root ) {
                     beginInsertRows( QModelIndex{}, first, last );
                 }
             } );

    connect( source, &QAbstractItemModel::rowsInserted, this,
             [this]( const QModelIndex& parent, int, int ) {
                 if ( parent == _root ) {
                     endInsertRows();
                 }
             } );

    connect( source, &QAbstractItemModel::rowsAboutToBeRemoved, this,
             [this]( const QModelIndex& parent, int first, int last ) {
                 if ( parent == _root ) {
                     beginRemoveRows( QModelIndex{}, first, last );
                 }
             } );

    connect( source, &QAbstractItemModel::rowsRemoved, this,
             [this]( const QModelIndex& parent, int, int ) {
                 if ( parent == _root ) {
                     endRemoveRows();
                 }
             } );

    connect( source, &QAbstractItemModel::dataChanged, this,
             [this]( const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles ) {
                 if ( topLeft.parent() == _root ) {
                     emit dataChanged( index( topLeft.row(), 0 ), index( bottomRight.row(), 0 ), roles );
                 }
             } );
}

auto FlatDirProxyModel::setRoot( const QModelIndex& root ) -> void {
    beginResetModel();
    _root = root;
    endResetModel();
}

auto FlatDirProxyModel::mapFromSource( const QModelIndex& sourceIndex ) const -> QModelIndex {
    if ( !sourceIndex.isValid() || sourceIndex.parent() != _root ) {
        return {};
    }

    return index( sourceIndex.row(), sourceIndex.column() );
}

auto FlatDirProxyModel::mapToSource( const QModelIndex& proxyIndex ) const -> QModelIndex {
    if ( !proxyIndex.isValid() || sourceModel() == nullptr ) {
        return {};
    }

    return sourceModel()->index( proxyIndex.row(), proxyIndex.column(), _root );
}

auto FlatDirProxyModel::index( int row, int column, const QModelIndex& parent ) const -> QModelIndex {
    return hasIndex( row, column, parent ) ? createIndex( row, column ) : QModelIndex{};
}

auto FlatDirProxyModel::parent( const QModelIndex& child ) const -> QModelIndex {
    return {};
}

auto FlatDirProxyModel::rowCount( const QModelIndex& parent ) const -> int {
    if ( parent.isValid() || sourceModel() == nullptr ) {
        return 0;
    }

    return sourceModel()->rowCount( _root );
}

auto FlatDirProxyModel::columnCount( const QModelIndex& parent ) const -> int {
    return parent.isValid() ? 0 : 1;
}

auto FlatDirProxyModel::data( const QModelIndex& proxyIndex, int role ) const -> QVariant {
    if ( sourceModel() == nullptr ) {
        return {};
    }

    return sourceModel()->data( mapToSource( proxyIndex ), role );
}

auto FlatDirProxyModel::roleNames() const -> QHash<int, QByteArray> {
    if ( sourceModel() != nullptr ) {
        return sourceModel()->roleNames();
    }

    return QAbstractProxyModel::roleNames();
}

auto FlatDirProxyModel::canFetchMore( const QModelIndex& parent ) const -> bool {
    if ( parent.isValid() || sourceModel() == nullptr ) {
        return false;
    }

    return sourceModel()->canFetchMore( _root );
}

auto FlatDirProxyModel::fetchMore( const QModelIndex& parent ) -> void {
    if ( parent.isValid() || sourceModel() == nullptr ) {
        return;
    }

    sourceModel()->fetchMore( _root );
}

// ======================================================================================
// ==================== FilesystemItem ==================================================
// ======================================================================================

FilesystemItem::FilesystemItem( QQuickItem* parent )
    : _filesystemModel{ new QFileSystemModel{ this } }
    , _proxyModel{ new FlatDirProxyModel{ _filesystemModel, this } }
    , _nowDirectory{ QDir::homePath() } {
    //
    _filesystemModel->setFilter( QDir::AllEntries | QDir::AllDirs | QDir::Hidden | QDir::System );
    _filesystemModel->setNameFilterDisables( true );

    _proxyModel->setSourceModel( _filesystemModel );

    const auto root = _filesystemModel->setRootPath( _nowDirectory.path() );
    _proxyModel->setRoot( root );
}

auto FilesystemItem::filesystemModel() const -> QAbstractItemModel* {
    return _proxyModel;
}

auto FilesystemItem::nowDirectory() const -> QString {
    return _nowDirectory.path();
}

auto FilesystemItem::setNowDirecory( const QString& value ) -> void {
    _nowDirectory = QDir{ value };

    const auto root = _filesystemModel->setRootPath( _nowDirectory.path() );
    _proxyModel->setRoot( root );

    emit nowDirectoryChanged();
    emit filesystemModelChanged();
}

}  // namespace tire
