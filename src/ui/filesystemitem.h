
#pragma once

#include <QObject>
#include <QQuickItem>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QAbstractListModel>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QDir>

namespace tire {

// ======================================================================================
// ==================== FlatDirProxyModel ===============================================
// ======================================================================================

struct FlatDirProxyModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        IsDirectoryRole = Qt::UserRole + 100,
        IsHiddenRole,
        SizeRole,
        TypeRole,
        LastModifiedRole,
        PermissionsRole
    };

    explicit FlatDirProxyModel( QFileSystemModel* source, QObject* parent = nullptr );

    auto filterText() const -> QString;
    auto setFilterText( const QString& text ) -> void;

    auto setRootIndex( const QModelIndex& sourceIndex ) -> void;

    auto rowCount( const QModelIndex& parent = QModelIndex{} ) const -> int override;
    auto data( const QModelIndex& index, int role ) const -> QVariant override;
    auto roleNames() const -> QHash<int, QByteArray> override;

private:
    QFileSystemModel* _source{};
    QSortFilterProxyModel* _proxy{};
    QPersistentModelIndex _root{};
};

// ======================================================================================
// ==================== FilesystemItem ==================================================
// ======================================================================================

struct FilesystemItem : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QAbstractItemModel* filesystemModel READ filesystemModel NOTIFY filesystemModelChanged )
    Q_PROPERTY( QString nowDirectory READ nowDirectory WRITE setNowDirectory NOTIFY nowDirectoryChanged )
    Q_PROPERTY( QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged )

public:
    explicit FilesystemItem( QQuickItem* parent = nullptr );

    auto filesystemModel() const -> QAbstractItemModel*;

    auto nowDirectory() const -> QString;
    auto setNowDirectory( const QString& value ) -> void;

    auto filterText() const -> QString;
    auto setFilterText( const QString& value ) -> void;

signals:
    void filesystemModelChanged();
    void nowDirectoryChanged();
    void filterTextChanged();

private:
    QFileSystemModel* _filesystemModel{};
    FlatDirProxyModel* _proxyModel{};
    QDir _nowDirectory{};
};

}  // namespace tire
