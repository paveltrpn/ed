
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
    enum Role { IsDirectoryRole = Qt::UserRole + 100 };

    explicit FlatDirProxyModel( QAbstractItemModel* source, QObject* parent = nullptr );

    auto filterText() const -> QString;
    auto setFilterText( const QString& text ) -> void;

    auto roleNames() const -> QHash<int, QByteArray> override;
    auto data( const QModelIndex& proxyIndex, int role ) const -> QVariant override;

protected:
    auto lessThan( const QModelIndex& left, const QModelIndex& right ) const -> bool override;
    auto filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const -> bool override;

private:
    QString _filterText{};
};

// ======================================================================================
// ==================== FilesystemItem ==================================================
// ======================================================================================

struct FilesystemItem : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QAbstractItemModel* filesystemModel READ filesystemModel NOTIFY filesystemModelChanged )
    Q_PROPERTY( QString nowDirectory READ nowDirectory WRITE setNowDirecory NOTIFY nowDirectoryChanged )
    Q_PROPERTY( QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged )
    Q_PROPERTY( QModelIndex rootIndex READ rootIndex NOTIFY rootIndexChanged )

public:
    explicit FilesystemItem( QQuickItem* parent = nullptr );

    auto filesystemModel() const -> QAbstractItemModel*;
    auto rootIndex() const -> QModelIndex;

    auto nowDirectory() const -> QString;
    auto setNowDirecory( const QString& value ) -> void;

    auto filterText() const -> QString;
    auto setFilterText( const QString& value ) -> void;

signals:
    void filesystemModelChanged();
    void nowDirectoryChanged();
    void filterTextChanged();
    void rootIndexChanged();

private:
    QFileSystemModel* _filesystemModel{};
    FlatDirProxyModel* _proxyModel{};
    QDir _nowDirectory{};
};

}  // namespace tire
