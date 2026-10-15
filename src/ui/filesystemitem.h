
#pragma once

#include <QObject>
#include <QQuickItem>
#include <QFileSystemModel>
#include <QDir>

namespace tire {

// ======================================================================================
// ==================== FilesystemItem ==================================================
// ======================================================================================

struct FilesystemItem : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QFileSystemModel* filesystemModel READ filesystemModel NOTIFY filesystemModelChanged )
    Q_PROPERTY( QString nowDirectory READ nowDirectory WRITE setNowDirecory NOTIFY nowDirectoryChanged )

public:
    explicit FilesystemItem( QQuickItem* parent = nullptr );

    auto filesystemModel() const -> QFileSystemModel*;

    auto nowDirectory() const -> QString;
    auto setNowDirecory( const QString& value ) -> void;

signals:
    void filesystemModelChanged();
    void nowDirectoryChanged();

private:
    QFileSystemModel* _filesystemModel{};
    QDir _nowDirectory{};
};

}  // namespace tire
