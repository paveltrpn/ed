#include "filesystemitem.h"

namespace tire {

// ======================================================================================
// ==================== FilesystemItem ==================================================
// ======================================================================================

FilesystemItem::FilesystemItem( QQuickItem* parent )
    : _filesystemModel{ new QFileSystemModel{ this } }
    , _nowDirectory{ QDir::homePath() } {
    //
    _filesystemModel->setRootPath( _nowDirectory.path() );
}

auto FilesystemItem::filesystemModel() const -> QFileSystemModel* {
    return _filesystemModel;
}

auto FilesystemItem::nowDirectory() const -> QString {
    return _nowDirectory.path();
}

auto FilesystemItem::setNowDirecory( const QString& value ) -> void {
    _nowDirectory = QDir{ value };

    _filesystemModel->setRootPath( _nowDirectory.path() );

    emit nowDirectoryChanged();
    emit filesystemModelChanged();
}

}  // namespace tire
