
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
    Q_PROPERTY( QJsonObject colors READ colors WRITE setColors MEMBER _colors NOTIFY colorsChanged )
    Q_PROPERTY( QVariantMap fonts READ fonts MEMBER _fonts NOTIFY colorsChanged )
    Q_PROPERTY( QVariantMap units READ units MEMBER _units NOTIFY unitsChanged )

public:
    Appearance( QObject *parent = nullptr );

    Q_INVOKABLE QJsonObject colors();
    Q_INVOKABLE QVariantMap fonts();
    Q_INVOKABLE QVariantMap units();

    Q_INVOKABLE void setColors( QJsonObject &value );

    [[nodiscard]] auto getColor( const QString &value ) const -> QString;
    [[nodiscard]] auto getUnit( const QString &value ) const -> float;

private:
    auto buildFonts() -> void;
    auto buildUnits() -> void;

signals:
    void colorsChanged();
    void fontsChanged();
    void unitsChanged();

private:
    QJsonObject _colors{};
    QVariantMap _fonts{};
    QVariantMap _units{};

    float _scale{ 1.0f };
};

}  // namespace tire
