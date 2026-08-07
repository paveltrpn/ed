
#pragma once

#include <QObject>
#include <QJsonObject>
#include <QDir>
#include <QQuickImageProvider>

namespace tire {

// Application-wide image provide.
struct TiredImageProvider : QQuickImageProvider {
public:
    TiredImageProvider();

    QImage requestImage( const QString &id, QSize *size, const QSize &requestedSize ) override;

private:
};

// =============================================================================

// Colors and other theme related provider. Instance of
// this object available in qml.
struct Appearance : QObject {
    Q_OBJECT
    Q_PROPERTY( QJsonObject colors READ colors WRITE setColors MEMBER colors_ NOTIFY colorsChanged )
    Q_PROPERTY( QJsonObject gaps READ gaps MEMBER gaps_ NOTIFY gapsChanged )
    Q_PROPERTY( QJsonObject radius READ radius MEMBER radius_ NOTIFY radiusChanged )
    Q_PROPERTY( QVariantMap fonts READ fonts MEMBER fonts_ NOTIFY colorsChanged )

public:
    Appearance( QObject *parent = nullptr );

    Q_INVOKABLE QJsonObject colors();
    Q_INVOKABLE void setColors( QJsonObject &value );
    Q_INVOKABLE QVariantMap fonts();
    Q_INVOKABLE QJsonObject gaps();
    Q_INVOKABLE QJsonObject radius();

    auto getColor( const QString &value ) const -> QString;
    auto getGap( const QString &value ) -> int;
    auto getRadius( const QString &value ) -> int;

signals:
    void colorsChanged();
    void fontsChanged();
    void gapsChanged();
    void radiusChanged();

private:
    QJsonObject colors_{};
    QJsonObject gaps_{};
    QJsonObject radius_{};
    QVariantMap fonts_{};
};

}  // namespace tire
