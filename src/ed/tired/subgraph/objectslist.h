#pragma once

#include <vector>

#include <QAbstractListModel>

#include "../scene_object/sceneobjectbase.h"

namespace tire {

struct ObjectsList final : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit ObjectsList( QObject *parent = nullptr );
    ~ObjectsList() override;

    // Required overrides
    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const override;
    QModelIndex parent( const QModelIndex &child ) const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

    // Optional but useful overrides
    Qt::ItemFlags flags( const QModelIndex &index ) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole ) override;
    bool insertRows( int row, int count, const QModelIndex &parent = QModelIndex() ) override;
    bool removeRows( int row, int count, const QModelIndex &parent = QModelIndex() ) override;

private:
    std::vector<SceneObjectBase *> _objectsList{};
};

};  // namespace tire
