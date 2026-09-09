
#include <print>

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
    QFile file( wp.path() + QDir::separator() + "src/ed/ui/qml/appearence/default.json" );

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

    const auto object = doc.object();
    _colors = object["colors"].toObject();
    _gaps = object["gaps"].toObject();
    _radius = object["radius"].toObject();

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

    _units["fixed_1"] = 1.0f;
    _units["fixed_2"] = 2.0f;
    _units["fixed_4"] = 4.0f;
    _units["fixed_6"] = 6.0f;
    _units["fixed_8"] = 8.0f;

    _units["fixed_10"] = 10.0f;
    _units["fixed_12"] = 12.0f;
    _units["fixed_14"] = 14.0f;
    _units["fixed_16"] = 16.0f;
    _units["fixed_18"] = 18.0f;

    _units["fixed_20"] = 20.0f;
    _units["fixed_22"] = 22.0f;
    _units["fixed_24"] = 24.0f;
    _units["fixed_26"] = 26.0f;
    _units["fixed_28"] = 28.0f;

    _units["fixed_32"] = 32.0f;
    _units["fixed_34"] = 34.0f;
    _units["fixed_36"] = 36.0f;
    _units["fixed_38"] = 38.0f;

    _units["fixed_40"] = 40.0f;
    _units["fixed_44"] = 44.0f;
    _units["fixed_46"] = 46.0f;
    _units["fixed_48"] = 48.0f;

    _units["fixed_52"] = 52.0f;
    _units["fixed_54"] = 54.0f;
    _units["fixed_56"] = 56.0f;
    _units["fixed_58"] = 58.0f;

    _units["fixed_64"] = 64.0f;

    _units["fixed_72"] = 72.0f;

    _units["fixed_80"] = 80.0f;
    _units["fixed_86"] = 86.0f;
    _units["fixed_88"] = 88.0f;

    _units["fixed_96"] = 96.0f;

    _units["scaled_1"] = 1.0f * _scale;
    _units["scaled_2"] = 2.0f * _scale;
    _units["scaled_4"] = 4.0f * _scale;
    _units["scaled_6"] = 6.0f * _scale;
    _units["scaled_8"] = 8.0f * _scale;

    _units["scaled_10"] = 10.0f * _scale;
    _units["scaled_12"] = 12.0f * _scale;
    _units["scaled_14"] = 14.0f * _scale;
    _units["scaled_16"] = 16.0f * _scale;
    _units["scaled_18"] = 18.0f * _scale;

    _units["scaled_20"] = 20.0f * _scale;
    _units["scaled_22"] = 22.0f * _scale;
    _units["scaled_24"] = 24.0f * _scale;
    _units["scaled_26"] = 26.0f * _scale;
    _units["scaled_28"] = 28.0f * _scale;

    _units["scaled_32"] = 32.0f * _scale;
    _units["scaled_34"] = 34.0f * _scale;
    _units["scaled_36"] = 36.0f * _scale;
    _units["scaled_38"] = 38.0f * _scale;

    _units["scaled_40"] = 40.0f * _scale;
    _units["scaled_44"] = 44.0f * _scale;
    _units["scaled_46"] = 46.0f * _scale;
    _units["scaled_48"] = 48.0f * _scale;

    _units["scaled_52"] = 52.0f * _scale;
    _units["scaled_54"] = 54.0f * _scale;
    _units["scaled_56"] = 56.0f * _scale;
    _units["scaled_58"] = 58.0f * _scale;

    _units["scaled_64"] = 64.0f * _scale;

    _units["scaled_72"] = 72.0f * _scale;

    _units["scaled_80"] = 80.0f * _scale;
    _units["scaled_86"] = 86.0f * _scale;
    _units["scaled_88"] = 88.0f * _scale;

    _units["scaled_96"] = 96.0f * _scale;

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

QJsonObject Appearance::gaps() {
    //
    return _gaps;
}

QJsonObject Appearance::radius() {
    //
    return _radius;
}

auto Appearance::getColor( const QString &value ) const -> QString {
    //
    return _colors.value( value ).toString();
}

auto Appearance::getGap( const QString &value ) -> int {
    //
    return _gaps.value( value ).toInt();
}

auto Appearance::getRadius( const QString &value ) -> int {
    //
    return _radius.value( value ).toInt();
}

}  // namespace tire
