
#include <print>

#include <QStringView>
#include <QFont>

#include "appearance.h"
#include "config/config.h"

namespace tire {

// ======================================================================================
// ==================== TiredImageProvider ==============================================
// ======================================================================================

TiredImageProvider::TiredImageProvider()
    : QQuickImageProvider( QQuickImageProvider::Image ) {
}

QImage TiredImageProvider::requestImage( const QString &id, QSize *size, const QSize &requestedSize ) {
    const auto basePath = Config::instance().basePath().string();
    QImage requestedImg{ QString{ "%1/src/ed/ui/qml/icons/%2" }.arg( basePath ).arg( id ) };

    if ( requestedImg.isNull() ) {
        std::println( "image \"{}\" not found!", id.toStdString() );
        return {};
    }

    return requestedImg;
}

// ======================================================================================
// ==================== Appearance ======================================================
// ======================================================================================

void Appearance::init() {
    if ( _initSuccess ) {
        log::error()( "Warning: Singleton already initialized. Ignoring new arguments." );
    }

    std::call_once( _initFlag, [&]() {
        _instance.store( new Appearance{} );
        _initSuccess = true;
    } );
}

Appearance &Appearance::instance() {
    auto *ptr = _instance.load();

    if ( !ptr ) {
        throw std::logic_error( "Singleton must be initialized via init( ... ) before calling instance()." );
    }
    return *ptr;
}

Appearance *Appearance::pointer() {
    auto *ptr = _instance.load();

    if ( !ptr ) {
        throw std::logic_error( "Singleton must be initialized via init( ... ) before calling instance()." );
    }

    QQmlEngine::setObjectOwnership( ptr, QQmlEngine::CppOwnership );

    return ptr;
}

Appearance::Appearance( QObject *parent )
    : _colors{ new AppearanceDataProxy{ &_colorsData, this } }
    , _fonts{ new AppearanceDataProxy{ &_fontsData, this } }
    , _units{ new AppearanceDataProxy{ &_unitsData, this } } {
    buildColors();
    buildFonts();
    buildUnits();
}

auto Appearance::buildColors() -> void {
    //
    const auto basePath = Config::instance().basePath().string();

    // Load color scheme.
    QFile file( QString{ "%1/src/ed/ui/qml/colorscheme.json" }.arg( basePath ) );

    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        std::println( "appearence file not exist : {}", file.fileName().toStdString() );
        std::terminate();
    }

    const QByteArray data = file.readAll();

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson( data, &parseError );
    if ( parseError.error != QJsonParseError::NoError ) {
        std::println( "appearence theme json parse failed with message: {}", parseError.errorString().toStdString() );
        std::terminate();
    }

    _colorsData._data = doc.object().toVariantMap();
}

auto Appearance::buildFonts() -> void {
    const auto fontName = "LessPerfectDOSVGA";
    const auto baseFontSize = 14;

    auto title_big = QFont{ fontName, baseFontSize + 12, QFont::Medium };
    title_big.setPixelSize( ( static_cast<float>( baseFontSize ) + 12.0f ) * _scale );
    _fontsData["title_big"] = title_big;

    auto title_accent = QFont{ fontName, baseFontSize + 12, QFont::ExtraBold };
    title_accent.setPixelSize( ( static_cast<float>( baseFontSize ) + 12.0f ) * _scale );
    _fontsData["title_accent"] = title_accent;

    auto title = QFont{ fontName, baseFontSize + 8, QFont::Medium };
    title.setPixelSize( ( static_cast<float>( baseFontSize ) + 8.0f ) * _scale );
    _fontsData["title"] = title;

    auto subtitle_accent = QFont{ fontName, baseFontSize + 8, QFont::ExtraBold };
    subtitle_accent.setPixelSize( ( static_cast<float>( baseFontSize ) + 8.0f ) * _scale );
    _fontsData["subtitle_accent"] = subtitle_accent;

    auto subtitle = QFont{ fontName, baseFontSize + 8, QFont::Medium };
    subtitle.setPixelSize( ( static_cast<float>( baseFontSize ) + 8.0f ) * _scale );
    _fontsData["subtitle"] = subtitle;

    auto text_body_accent = QFont{ fontName, baseFontSize + 4, QFont::ExtraBold };
    text_body_accent.setPixelSize( ( static_cast<float>( baseFontSize ) + 4.0f ) * _scale );
    _fontsData["text_body_accent"] = text_body_accent;

    auto text_body = QFont{ fontName, baseFontSize + 4, QFont::Medium };
    text_body.setPixelSize( ( static_cast<float>( baseFontSize ) + 4.0f ) * _scale );
    _fontsData["text_body"] = text_body;

    auto label_accent = QFont{ fontName, baseFontSize + 2, QFont::ExtraBold };
    label_accent.setPixelSize( ( static_cast<float>( baseFontSize ) + 2.0f ) * _scale );
    _fontsData["label_accent"] = label_accent;

    auto label = QFont{ fontName, baseFontSize + 2, QFont::Medium };
    label.setPixelSize( ( static_cast<float>( baseFontSize ) + 2.0f ) * _scale );
    _fontsData["label"] = label;

    auto subtext_accent = QFont{ fontName, baseFontSize + 0, QFont::ExtraBold };
    subtext_accent.setPixelSize( ( static_cast<float>( baseFontSize ) + 0.0f ) * _scale );
    _fontsData["subtext_accent"] = subtext_accent;

    auto subtext = QFont{ fontName, baseFontSize + 0, QFont::Medium };
    subtext.setPixelSize( ( static_cast<float>( baseFontSize ) + 0.0f ) * _scale );
    _fontsData["subtext"] = subtext;
}

auto Appearance::buildUnits() -> void {
    _unitsData["eight"] = 2.0f;
    _unitsData["quarter"] = 4.0f;
    _unitsData["half"] = 8.0f;
    _unitsData["full"] = 16.0f;

    for ( auto i{ 1 }; i < 1024; ++i ) {
        const auto keyString = QString{ "%1_%2" }.arg( "fixed" ).arg( i );
        _unitsData.insert( keyString, static_cast<float>( i ) );
    }

    for ( auto i{ 1 }; i < 1024; ++i ) {
        const auto keyString = QString{ "%1_%2" }.arg( "scaled" ).arg( i );
        _unitsData.insert( keyString, static_cast<float>( i ) * _scale );
    }

    _unitsData["radiusEight"] = 2.0f;
    _unitsData["radiusQuarter"] = 4.0f;
    _unitsData["radiusHalf"] = 8.0f;
    _unitsData["radiusFull"] = 16.0f;
}

AppearanceDataProxy *Appearance::colors() {
    //
    return _colors;
};

AppearanceDataProxy *Appearance::fonts() {
    //
    return _fonts;
}

AppearanceDataProxy *Appearance::units() {
    //
    return _units;
}

auto Appearance::getColor( const QString &value ) const -> QString {
    //
    return _colorsData[value].toString();
}

auto Appearance::getUnit( const QString &value ) const -> float {
    return _unitsData[value].toFloat();
}

}  // namespace tire
