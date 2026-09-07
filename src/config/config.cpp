
#include <fstream>

#include "config.h"

namespace tire {

void Config::init( const std::filesystem::path &fname ) {
    if ( _initSuccess ) {
        log::error()( "Warning: Singleton already initialized. Ignoring new arguments." );
    }

    std::call_once( _initFlag, [&]() {
        _instance.store( new Config( fname ) );
        _initSuccess = true;
    } );
}

Config &Config::instance() {
    Config *ptr = _instance.load();

    if ( !ptr ) {
        throw std::logic_error( "Singleton must be initialized via init() before calling getInstance()." );
    }
    return *ptr;
}

Config::Config( const std::filesystem::path &fname )
    : basePath_{ std::filesystem::canonical( "/proc/self/exe" ).parent_path().parent_path() } {
    const auto path = std::filesystem::path{ basePath_ / fname };

    std::ifstream file{ path };
    if ( file ) {
        try {
            config_ = nlohmann::json::parse( file );
        } catch ( const nlohmann::json::parse_error &e ) {
            log::error()(
                "config json parse error\n"
                "message:\t{}\n"
                "exception id:\t{}\n"
                "byte position of error:\t{}",
                e.what(), e.id, e.byte );
        }
    } else {
        throw std::runtime_error( std::format( "file not found: {}\n", path.string() ) );
    }
}

auto Config::basePath() const -> const std::filesystem::path & {
    //
    return basePath_;
};

auto Config::getString( std::string_view param ) const -> std::string {
    //
    return config_[param];
}

auto Config::getBool( std::string_view param ) const -> bool {
    //
    return config_[param];
}

auto Config::getNumber( std::string_view param ) const -> double {
    //
    return config_[param];
}

auto Config::getJson( std::string_view param ) const -> nlohmann::json {
    //
    return config_[param];
}

}  // namespace tire