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
    return cmp != 0 ? cmp < 0 : leftName < rightName;
}

auto FlatDirProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const -> bool {
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

    return roles;
}

auto FlatDirProxyModel::data( const QModelIndex& proxyIndex, int role ) const -> QVariant {
    if ( role == IsDirectoryRole ) {
        const auto* fsm = qobject_cast<QFileSystemModel*>( sourceModel() );
        return fsm ? fsm->isDir( mapToSource( proxyIndex ) ) : false;
    }

    return QSortFilterProxyModel::data( proxyIndex, role );
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

    _filesystemModel->setRootPath( _nowDirectory.path() );

    connect( _filesystemModel, &QFileSystemModel::directoryLoaded, this, [this]( const QString& path ) {
        if ( path == _nowDirectory.path() ) {
            emit rootIndexChanged();
        }
    } );
}

auto FilesystemItem::filesystemModel() const -> QAbstractItemModel* {
    return _proxyModel;
}

auto FilesystemItem::rootIndex() const -> QModelIndex {
    return _proxyModel->mapFromSource( _filesystemModel->index( _nowDirectory.path() ) );
}

auto FilesystemItem::nowDirectory() const -> QString {
    return _nowDirectory.path();
}

auto FilesystemItem::setNowDirecory( const QString& value ) -> void {
    _nowDirectory = QDir{ value };

    _filesystemModel->setRootPath( _nowDirectory.path() );

    emit nowDirectoryChanged();
    emit filesystemModelChanged();
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
