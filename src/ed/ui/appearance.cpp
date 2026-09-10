
#include <print>

#include <QStringView>
#include <QFont>

#include "appearance.h"

namespace tire {

// ======================================================================================
// ==================== TiredImageProvider ==============================================
// ======================================================================================

TiredImageProvider::TiredImageProvider()
    : QQuickImageProvider( QQuickImageProvider::Image ) {
}

QImage TiredImageProvider::requestImage( const QString &id, QSize *size, const QSize &requestedSize ) {
    QImage requestedImg{ QString{ "/mnt/main/code/ed/src/ed/ui/qml/icons/%1" }.arg( id ) };

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
    auto wp = QDir{ QDir::currentPath() };
    wp.cdUp();

    // Load color scheme.
    QFile file( wp.path() + QDir::separator() + "src/ed/ui/qml/colorscheme.json" );

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
    const auto fontName = "Monospace";
    const auto baseFontSize = 8;

    _fonts["title_big"] = QFont{ fontName, baseFontSize + 12, QFont::ExtraBold };
    _fonts["title_accent"] = QFont{ fontName, baseFontSize + 8, QFont::ExtraBold };
    _fonts["title"] = QFont{ fontName, baseFontSize + 8, QFont::Medium };
    _fonts["subtitle_accent"] = QFont{ fontName, baseFontSize + 8, QFont::ExtraBold };
    _fonts["subtitle"] = QFont{ fontName, baseFontSize + 8, QFont::Medium };
    _fonts["text_body_accent"] = QFont{ fontName, baseFontSize + 4, QFont::ExtraBold };
    _fonts["text_body"] = QFont{ fontName, baseFontSize + 4, QFont::Medium };
    _fonts["label_accent"] = QFont{ fontName, baseFontSize + 2, QFont::ExtraBold };
    _fonts["label"] = QFont{ fontName, baseFontSize + 2, QFont::Medium };
    _fonts["subtext_accent"] = QFont{ fontName, baseFontSize, QFont::ExtraBold };
    _fonts["subtext"] = QFont{ fontName, baseFontSize, QFont::Medium };
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
