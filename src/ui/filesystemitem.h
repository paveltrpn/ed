
#pragma once

#include <QObject>
#include <QQuickItem>
#include <QFileSystemModel>
#include <QAbstractProxyModel>
#include <QModelIndex>
#include <QDir>

namespace tire {

// ======================================================================================
// ==================== FlatDirProxyModel ===============================================
// ======================================================================================

struct FlatDirProxyModel : public QAbstractProxyModel {
    Q_OBJECT

public:
    explicit FlatDirProxyModel( QAbstractItemModel* source, QObject* parent = nullptr );

    auto setRoot( const QModelIndex& root ) -> void;

    auto mapFromSource( const QModelIndex& sourceIndex ) const -> QModelIndex override;
    auto mapToSource( const QModelIndex& proxyIndex ) const -> QModelIndex override;

    auto index( int row, int column, const QModelIndex& parent = QModelIndex() ) const -> QModelIndex override;
    auto parent( const QModelIndex& child ) const -> QModelIndex override;

    auto rowCount( const QModelIndex& parent = QModelIndex() ) const -> int override;
    auto columnCount( const QModelIndex& parent = QModelIndex() ) const -> int override;

    auto data( const QModelIndex& proxyIndex, int role = Qt::DisplayRole ) const -> QVariant override;
    auto roleNames() const -> QHash<int, QByteArray> override;

    auto canFetchMore( const QModelIndex& parent ) const -> bool override;
    auto fetchMore( const QModelIndex& parent ) -> void override;

private:
    QPersistentModelIndex _root{};
};

// ======================================================================================
// ==================== FilesystemItem ==================================================
// ======================================================================================

struct FilesystemItem : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QAbstractItemModel* filesystemModel READ filesystemModel NOTIFY filesystemModelChanged )
    Q_PROPERTY( QString nowDirectory READ nowDirectory WRITE setNowDirecory NOTIFY nowDirectoryChanged )

public:
    explicit FilesystemItem( QQuickItem* parent = nullptr );

    auto filesystemModel() const -> QAbstractItemModel*;

    auto nowDirectory() const -> QString;
    auto setNowDirecory( const QString& value ) -> void;

signals:
    void filesystemModelChanged();
    void nowDirectoryChanged();

private:
    QFileSystemModel* _filesystemModel{};
    FlatDirProxyModel* _proxyModel{};
    QDir _nowDirectory{};
};

}  // namespace tire
