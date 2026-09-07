
#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <format>
#include <type_traits>
#include <filesystem>

#include "nlohmann/json.hpp"
#include "log/log.h"

namespace tire {

template <typename T>
concept ConfigParamType =
    std::is_same_v<bool, std::remove_cv_t<T>> || std::is_same_v<int, std::remove_cv_t<T>> ||
    std::is_same_v<float, std::remove_cv_t<T>> || std::is_same_v<std::string, std::remove_cv_t<T>> ||
    std::is_same_v<nlohmann::json, std::remove_cv_t<T>>;

struct Config final {
public:
    Config( const Config &rhs ) = delete;
    Config( Config &&rhs ) = delete;
    auto operator=( const Config &rhs ) -> Config & = delete;
    auto operator=( Config &&rhs ) -> Config & = delete;

    static void init( const std::filesystem::path &fname );
    [[nodiscard]] static Config &instance();

    [[nodiscard]] auto basePath() const -> const std::filesystem::path &;

    [[nodiscard]] auto getString( std::string_view param ) const -> std::string;
    [[nodiscard]] auto getBool( std::string_view param ) const -> bool;
    [[nodiscard]] auto getNumber( std::string_view param ) const -> double;
    [[nodiscard]] auto getJson( std::string_view param ) const -> nlohmann::json;

    template <ConfigParamType T>
    [[nodiscard]] auto get( std::string_view param, T dflt = {} ) const -> T {
        try {
            if ( config_.contains( param ) ) {
                return config_[param];
            } else {
                log::warning()( "no such config parameter \"{}\", default value used", param );
                return dflt;
            }
        } catch ( nlohmann::json::exception &e ) {
            log::warning()(
                "json exception handled... config param error \"{}\", what: "
                "{}, default value used",
                param, e.what() );
            return dflt;
        }
    }

private:
    explicit Config( const std::filesystem::path &fname );
    ~Config() = default;

    inline static std::atomic<Config *> _instance{ nullptr };
    inline static std::once_flag _initFlag;
    inline static bool _initSuccess{ false };

private:
    nlohmann::json config_;
    std::filesystem::path basePath_{};
};

}  // namespace tire
