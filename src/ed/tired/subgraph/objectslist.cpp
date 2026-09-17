
#include <QAbstractListModel>

#include "objectslist.h"

namespace tire {

ObjectsList::ObjectsList( QObject *parent )
    : QAbstractItemModel( parent ) {
}

ObjectsList::~ObjectsList() {
}

QModelIndex ObjectsList::index( int row, int column, const QModelIndex &parent ) const {
    // if ( !hasIndex( row, column, parent ) ) return QModelIndex();

    // Node *parentNode = nodeFromIndex( parent );
    // if ( row < 0 || row >= parentNode->children.size() ) return QModelIndex();

    // Node *childNode = parentNode->children.at( row );

    // // Wrap the internal pointer into a QModelIndex
    // return createIndex( row, column, childNode );
}

QModelIndex ObjectsList::parent( const QModelIndex &child ) const {
    // if ( !child.isValid() ) return QModelIndex();

    // auto *childNode = static_cast<Node *>( child.internalPointer() );
    // Node *parentNode = childNode->parent;

    // if ( !parentNode || parentNode == m_root ) return QModelIndex();

    // // Find the row of parentNode in its own parent
    // Node *grandParent = parentNode->parent;
    // int row = grandParent ? grandParent->children.indexOf( parentNode ) : 0;

    // return createIndex( row, 0, parentNode );
}

int ObjectsList::rowCount( const QModelIndex &parent ) const {
    return _objectsList.size();
}

int ObjectsList::columnCount( const QModelIndex &parent ) const {
    return 1;
}

QVariant ObjectsList::data( const QModelIndex &index, int role ) const {
    if ( !index.isValid() ) {
        return {};
    }

    auto it = _objectsList.cbegin();
    it += index.row();

    if ( role == Roles::Object ) {
        return QVariant::fromValue<SceneObjectBase *>( *it );
    }

    // auto *node = static_cast<SceneObjectBase *>( index.internalPointer() );

    // if ( role == ObjectRole ) {
    // }  // node->obj is QObject* parented to the model
    // return QVariant::fromValue( node->obj );

    // if ( role == Qt::DisplayRole ) {
    //     return node->obj ? node->obj->objectName() : QString();
    // }

    return {};
}

QHash<int, QByteArray> ObjectsList::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();

    roles[Object] = "object";

    return roles;
}

QVariant ObjectsList::headerData( int section, Qt::Orientation orientation, int role ) const {
    // if ( orientation != Qt::Horizontal || role != Qt::DisplayRole ) return QVariant();

    // if ( section >= 0 && section < m_root->values.size() ) return m_root->values.at( section );

    // return QVariant();
}

Qt::ItemFlags ObjectsList::flags( const QModelIndex &index ) const {
    if ( !index.isValid() ) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool ObjectsList::setData( const QModelIndex &index, const QVariant &value, int role ) {
    // if ( !index.isValid() || role != Qt::EditRole ) return false;

    // auto *node = static_cast<Node *>( index.internalPointer() );
    // if ( !node || index.column() >= node->values.size() ) return false;

    // node->values[index.column()] = value.toString();
    // emit dataChanged( index, index, { role } );
    // return true;
}

bool ObjectsList::insertRows( int row, int count, const QModelIndex &parent ) {
    // if ( row < 0 || count <= 0 ) return false;

    // Node *parentNode = nodeFromIndex( parent );

    // beginInsertRows( parent, row, row + count - 1 );
    // for ( int i = 0; i < count; ++i ) {
    //     auto *node = new Node;
    //     node->parent = parentNode;
    //     node->values = QStringList{ QString( "New %1" ).arg( row + i ), "0", "new" };
    //     parentNode->children.insert( row + i, node );
    // }
    // endInsertRows();
    // return true;
}

bool ObjectsList::removeRows( int row, int count, const QModelIndex &parent ) {
    // if ( row < 0 || count <= 0 ) return false;

    // Node *parentNode = nodeFromIndex( parent );
    // if ( row + count > parentNode->children.size() ) return false;

    // beginRemoveRows( parent, row, row + count - 1 );
    // for ( int i = 0; i < count; ++i ) {
    //     Node *node = parentNode->children.takeAt( row );
    //     std::function<void( Node * )> destroy = [&]( Node *n ) {
    //         for ( Node *c : n->children ) destroy( c );
    //         delete n;
    //     };
    //     destroy( node );
    // }
    // endRemoveRows();
    // return true;
}

// ObjectsList::Node *ObjectsList::nodeFromIndex( const QModelIndex &index ) const {
//     if ( index.isValid() ) return static_cast<Node *>( index.internalPointer() );
//     return m_root;
// }

};  // namespace tire