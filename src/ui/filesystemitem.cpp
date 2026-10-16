#include "filesystemitem.h"

namespace tire {

// ======================================================================================
// ==================== FlatDirProxyModel ===============================================
// ======================================================================================

FlatDirProxyModel::FlatDirProxyModel( QAbstractItemModel* source, QObject* parent )
    : QSortFilterProxyModel( parent ) {
    //
    setDynamicSortFilter( true );
    setSourceModel( source );
    sort( 0, Qt::AscendingOrder );
}

void FlatDirProxyModel::setFilterText( const QString& text ) {
    beginFilterChange();
    _filterText = text;
    endFilterChange();
}

bool FlatDirProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const {
    const auto filenameIndex = sourceModel()->index( sourceRow, 0, sourceParent );

    if ( !_filterText.isEmpty() ) {
        QString name = sourceModel()->data( filenameIndex ).toString();
        if ( !name.contains( _filterText, Qt::CaseInsensitive ) ) {
            return false;
        }
    }

    return true;
}

QHash<int, QByteArray> FlatDirProxyModel::roleNames() const {
    auto roles = QSortFilterProxyModel::roleNames();

    roles[QFileSystemModel::FilePathRole] = "filePath";
    roles[QFileSystemModel::FileNameRole] = "fileName";
    roles[QFileSystemModel::FileIconRole] = "fileIcon";
    // roles[QFileSystemModel::FileSizeRole] = "fileSize";
    // roles[QFileSystemModel::FilePermsRole] = "filePermissions";

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
    _filesystemModel->setFilter( QDir::AllEntries | QDir::AllDirs | QDir::Hidden | QDir::System );
    _filesystemModel->setNameFilterDisables( true );

    _proxyModel->setSourceModel( _filesystemModel );

    const auto root = _filesystemModel->setRootPath( _nowDirectory.path() );
    //_proxyModel->setRoot( root );
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
    //_proxyModel->setRoot( root );

    emit nowDirectoryChanged();
    emit filesystemModelChanged();
}

}  // namespace tire
