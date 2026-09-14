
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
    QImage requestedImg{ QString{ "%1/src/ed/ui/qml/icons/%2" }.arg(basePath).arg( id ) };

    if ( requestedImg.isNull() ) {
        std::println( "image \"{}\" not found!", id.toStdString() );
        return {};
    }

    return requestedImg;
}

// ======================================================================================
// ==================== Appearance ======================================================
// ======================================================================================

Appearance::Appearance( QObject *parent )
    : QObject{ parent } {
    //
    const auto basePath = Config::instance().basePath().string();

    // Load color scheme.
    QFile file( QString{"%1/src/ed/ui/qml/colorscheme.json"}.arg(basePath) );

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

    _colors = doc.object();

    buildFonts();
    buildUnits();
}

auto Appearance::buildFonts() -> void {
    const auto fontName = "LessPerfectDOSVGA";
    const auto baseFontSize = 14;

    auto title_big = QFont{ fontName, baseFontSize + 12, QFont::Medium };
    title_big.setPixelSize((static_cast<float>(baseFontSize) + 12.0f) * _scale);
    _fonts["title_big"] = title_big;

    auto title_accent = QFont{ fontName, baseFontSize + 12, QFont::ExtraBold };
    title_accent.setPixelSize((static_cast<float>(baseFontSize) + 12.0f) * _scale);
    _fonts["title_accent"] = title_accent;

    auto title = QFont{ fontName, baseFontSize + 8, QFont::Medium };
    title.setPixelSize((static_cast<float>(baseFontSize) + 8.0f) * _scale);
    _fonts["title"] = title;

    auto subtitle_accent = QFont{ fontName, baseFontSize + 8, QFont::ExtraBold };
    subtitle_accent.setPixelSize((static_cast<float>(baseFontSize) + 8.0f) * _scale);
    _fonts["subtitle_accent"] = subtitle_accent;

    auto subtitle = QFont{ fontName, baseFontSize + 8, QFont::Medium };
    subtitle.setPixelSize((static_cast<float>(baseFontSize) + 8.0f) * _scale);
    _fonts["subtitle"] = subtitle;

    auto text_body_accent = QFont{ fontName, baseFontSize + 4, QFont::ExtraBold };
    text_body_accent.setPixelSize((static_cast<float>(baseFontSize) + 4.0f) * _scale);
    _fonts["text_body_accent"] = text_body_accent;

    auto text_body = QFont{ fontName, baseFontSize + 4, QFont::Medium };
    text_body.setPixelSize((static_cast<float>(baseFontSize) + 4.0f) * _scale);
    _fonts["text_body"] = text_body;

    auto label_accent = QFont{ fontName, baseFontSize + 2, QFont::ExtraBold };
    label_accent.setPixelSize((static_cast<float>(baseFontSize) + 2.0f) * _scale);
    _fonts["label_accent"] = label_accent;

    auto label = QFont{ fontName, baseFontSize + 2, QFont::Medium };
    label.setPixelSize((static_cast<float>(baseFontSize) + 2.0f) * _scale);
    _fonts["label"] = label;

    auto subtext_accent = QFont{ fontName, baseFontSize + 0, QFont::ExtraBold };
    subtext_accent.setPixelSize((static_cast<float>(baseFontSize) + 0.0f) * _scale);
    _fonts["subtext_accent"] = subtext_accent;

    auto subtext = QFont{ fontName, baseFontSize + 0, QFont::Medium };
    subtext.setPixelSize((static_cast<float>(baseFontSize) + 0.0f) * _scale);
    _fonts["subtext"] = subtext;
}

auto Appearance::buildUnits() -> void {
    _units["eight"] = 2.0f;
    _units["quarter"] = 4.0f;
    _units["half"] = 8.0f;
    _units["full"] = 16.0f;

    for ( auto i{ 1 }; i < 1024; ++i ) {
        const auto keyString = QString{ "%1_%2" }.arg( "fixed" ).arg( i );
        _units.insert( keyString, static_cast<float>( i ) );
    }

    for ( auto i{ 1 }; i < 1024; ++i ) {
        const auto keyString = QString{ "%1_%2" }.arg( "scaled" ).arg( i );
        _units.insert( keyString, static_cast<float>( i ) * _scale );
    }

    _units["radiusEight"] = 2.0f;
    _units["radiusQuarter"] = 4.0f;
    _units["radiusHalf"] = 8.0f;
    _units["radiusFull"] = 16.0f;
}

QJsonObject Appearance::colors() {
    //
    return _colors;
};

QVariantMap Appearance::fonts() {
    //
    return _fonts;
}

QVariantMap Appearance::units() {
    //
    return _units;
}

void Appearance::setColors( QJsonObject &value ) {
    //
    _colors = value;
};

auto Appearance::getColor( const QString &value ) const -> QString {
    //
    return _colors.value( value ).toString();
}

auto Appearance::getUnit( const QString &value ) const -> float {
    return _units.value( value ).toFloat();
}

}  // namespace tire
