
#pragma once

#include <QObject>
#include <QJsonObject>
#include <QDir>
#include <QQuickImageProvider>

namespace tire {

// ======================================================================================
// ==================== TiredImageProvider ==============================================
// ======================================================================================

// Application-wide image provide.
struct TiredImageProvider : QQuickImageProvider {
public:
    TiredImageProvider();

    QImage requestImage( const QString &id, QSize *size, const QSize &requestedSize ) override;

private:
};

// ======================================================================================
// ==================== Appearance ======================================================
// ======================================================================================

// Colors and other theme related provider. Instance of
// this object available in qml.
struct Appearance : QObject {
    Q_OBJECT
    Q_PROPERTY( QJsonObject gaps READ gaps MEMBER _gaps NOTIFY gapsChanged )
    Q_PROPERTY( QJsonObject radius READ radius MEMBER _radius NOTIFY radiusChanged )

    Q_PROPERTY( QJsonObject colors READ colors WRITE setColors MEMBER _colors NOTIFY colorsChanged )
    Q_PROPERTY( QVariantMap fonts READ fonts MEMBER _fonts NOTIFY colorsChanged )
    Q_PROPERTY( QVariantMap units READ units MEMBER _units NOTIFY unitsChanged )

public:
    Appearance( QObject *parent = nullptr );

    Q_INVOKABLE QJsonObject colors();
    Q_INVOKABLE QVariantMap fonts();
    Q_INVOKABLE QVariantMap units();

    Q_INVOKABLE void setColors( QJsonObject &value );

    Q_INVOKABLE QJsonObject gaps();
    Q_INVOKABLE QJsonObject radius();

    auto getColor( const QString &value ) const -> QString;
    auto getUnit( std::string_view value ) const -> float;

    auto getGap( const QString &value ) -> int;
    auto getRadius( const QString &value ) -> int;

private:
    auto buildFonts() -> void;
    auto buildUnits() -> void;

signals:
    void colorsChanged();
    void fontsChanged();
    void unitsChanged();

    void gapsChanged();
    void radiusChanged();

private:
    QJsonObject _gaps{};
    QJsonObject _radius{};

    QJsonObject _colors{};
    QVariantMap _fonts{};
    QVariantMap _units{};

    float _scale{ 1.0f };
};

}  // namespace tire
