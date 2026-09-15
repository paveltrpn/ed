
#pragma once

#include <atomic>
#include <mutex>

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

struct AppearanceData final {
    Q_GADGET
    Q_PROPERTY( QVariantMap data MEMBER _data )

public:
    void insert( const QString &key, QVariant value ) {
        //
        _data.insert( key, value );
    };

    QVariant &operator[]( const QString &key ) {
        //
        return _data[key];
    }

    const QVariant operator[]( const QString &key ) const {
        //
        return _data[key];
    }

    QVariantMap _data{};
};

struct AppearanceDataProxy final : QObject {
    Q_OBJECT
    Q_PROPERTY( QVariantMap data READ data NOTIFY dataChanged )

public:
    explicit AppearanceDataProxy( const AppearanceData *appearenceData, QObject *parent = nullptr )
        : QObject( parent )
        , _data( appearenceData ) {}

    QVariantMap data() const {
        //
        return this->_data->_data;
    };

signals:
    void dataChanged();

private:
    const AppearanceData *_data{};
};

// Colors and other theme related data provider. Instance of
// this object available in qml.
struct Appearance : QObject {
    Q_OBJECT
    Q_PROPERTY( AppearanceDataProxy *colors READ colors MEMBER _colors NOTIFY colorsChanged )
    Q_PROPERTY( AppearanceDataProxy *fonts READ fonts MEMBER _fonts NOTIFY colorsChanged )
    Q_PROPERTY( AppearanceDataProxy *units READ units MEMBER _units NOTIFY unitsChanged )

public:
    static void init();
    [[nodiscard]] static Appearance &instance();
    [[nodiscard]] static Appearance *pointer();

    AppearanceDataProxy *colors();
    AppearanceDataProxy *fonts();
    AppearanceDataProxy *units();

    [[nodiscard]] auto getColor( const QString &value ) const -> QString;
    [[nodiscard]] auto getUnit( const QString &value ) const -> float;

private:
    auto buildColors() -> void;
    auto buildFonts() -> void;
    auto buildUnits() -> void;

signals:
    void colorsChanged();
    void fontsChanged();
    void unitsChanged();

private:
    explicit Appearance( QObject *parent = nullptr );
    ~Appearance() = default;

    inline static std::atomic<Appearance *> _instance{ nullptr };
    inline static std::once_flag _initFlag;
    inline static bool _initSuccess{ false };

private:
    AppearanceData _colorsData{};
    AppearanceDataProxy *_colors{};

    AppearanceData _fontsData{};
    AppearanceDataProxy *_fonts{};

    AppearanceData _unitsData{};
    AppearanceDataProxy *_units;

    float _scale{ 1.0f };
};

}  // namespace tire
