
#pragma once

#include <QObject>
#include <QQuickItem>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QDir>

namespace tire {

// ======================================================================================
// ==================== FlatDirProxyModel ===============================================
// ======================================================================================

struct FlatDirProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit FlatDirProxyModel( QAbstractItemModel* source, QObject* parent = nullptr );

    void setFilterText( const QString& text );

    QHash<int, QByteArray> roleNames() const override;

    // auto mapFromSource( const QModelIndex& sourceIndex ) const -> QModelIndex override;
    auto mapToSource( const QModelIndex& proxyIndex ) const -> QModelIndex override;
    auto index( int row, int column, const QModelIndex& parent ) const -> QModelIndex override;
    auto parent( const QModelIndex& child ) const -> QModelIndex override;
    auto rowCount( const QModelIndex& parent ) const -> int override;
    auto columnCount( const QModelIndex& parent ) const -> int override;
    auto data( const QModelIndex& proxyIndex, int role ) const -> QVariant override;

protected:
    bool filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const override;

private:
    QString _filterText{};
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
