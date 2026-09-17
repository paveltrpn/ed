#pragma once

#include <vector>

#include <QAbstractListModel>

#include "../scene_object/sceneobjectbase.h"

namespace tire {

// ======================================================================================
// ==================== ObjectsList =====================================================
// ======================================================================================

struct ObjectsList final : public QAbstractItemModel {
    Q_OBJECT

public:
    enum Roles {
        Object = Qt::UserRole + 1,
    };

    explicit ObjectsList( QObject *parent = nullptr );
    ~ObjectsList() override;

    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const override;
    QModelIndex parent( const QModelIndex &child ) const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

    Qt::ItemFlags flags( const QModelIndex &index ) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole ) override;
    bool insertRows( int row, int count, const QModelIndex &parent = QModelIndex() ) override;
    bool removeRows( int row, int count, const QModelIndex &parent = QModelIndex() ) override;

    auto addObject( std::shared_ptr<SceneObjectBase> object ) -> void;
    auto findObject( const QUuid &uid ) const -> std::shared_ptr<SceneObjectBase>;

private:
    std::vector<std::shared_ptr<SceneObjectBase>> _objectsList{};
};

};  // namespace tire
