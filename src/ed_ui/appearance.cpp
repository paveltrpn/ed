
#include <print>

#include <QFont>

#include "appearance.h"

namespace tire {

TiredImageProvider::TiredImageProvider()
    : QQuickImageProvider( QQuickImageProvider::Image ) {
}

QImage TiredImageProvider::requestImage( const QString &id, QSize *size, const QSize &requestedSize ) {
    QImage requestedImg{ QString{ "/mnt/main/code/ed/src/ed_ui/qml/icons/%1" }.arg( id ) };

    if ( requestedImg.isNull() ) {
        std::println( "image \"{}\" not found!", id.toStdString() );
        return {};
    }

    return requestedImg;
}

// =============================================================================

Appearance::Appearance( QObject *parent )
    : QObject{ parent } {
    //
    auto wp = QDir{ QDir::currentPath() };
    wp.cdUp();

    // Load color scheme.
    QFile file( wp.path() + QDir::separator() + "src/ed_ui/qml/appearence/default.json" );

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
    colors_ = object["colors"].toObject();
    gaps_ = object["gaps"].toObject();
    radius_ = object["radius"].toObject();

    const auto fontName = object["main_font"].toString();
    const auto baseFontSize = object["base_font_size"].toInt();

    // Build fonts map
    fonts_["title_big"] = QFont{ fontName, baseFontSize + 12, QFont::ExtraBold };
    fonts_["title_accent"] = QFont{ fontName, baseFontSize + 8, QFont::ExtraBold };
    fonts_["title"] = QFont{ fontName, baseFontSize + 8, QFont::Medium };
    fonts_["subtitle_accent"] = QFont{ fontName, baseFontSize + 8, QFont::ExtraBold };
    fonts_["subtitle"] = QFont{ fontName, baseFontSize + 8, QFont::Medium };
    fonts_["text_body_accent"] = QFont{ fontName, baseFontSize + 4, QFont::ExtraBold };
    fonts_["text_body"] = QFont{ fontName, baseFontSize + 4, QFont::Medium };
    fonts_["label_accent"] = QFont{ fontName, baseFontSize + 2, QFont::ExtraBold };
    fonts_["label"] = QFont{ fontName, baseFontSize + 2, QFont::Medium };
    fonts_["subtext_accent"] = QFont{ fontName, baseFontSize, QFont::ExtraBold };
    fonts_["subtext"] = QFont{ fontName, baseFontSize, QFont::Medium };
}

QJsonObject Appearance::colors() {
    //
    return colors_;
};

void Appearance::setColors( QJsonObject &value ) {
    //
    colors_ = value;
};

QVariantMap Appearance::fonts() {
    //
    return fonts_;
}

QJsonObject Appearance::gaps() {
    //
    return gaps_;
}

QJsonObject Appearance::radius() {
    //
    return radius_;
}

auto Appearance::getColor( const QString &value ) const -> QString {
    //
    return colors_.value( value ).toString();
}

auto Appearance::getGap( const QString &value ) -> int {
    //
    return gaps_.value( value ).toInt();
}

auto Appearance::getRadius( const QString &value ) -> int {
    //
    return radius_.value( value ).toInt();
}

}  // namespace tire
