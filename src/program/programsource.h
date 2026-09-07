#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include "definitions.h"

namespace tire {

struct ProgramSource {
public:
    ProgramSource() = delete;

    ProgramSource( std::string programName )
        : _programName{ std::move( programName ) } {
            //
        };

    ProgramSource( const ProgramSource& other ) = default;
    ProgramSource( ProgramSource&& other ) = default;

    auto operator=( const ProgramSource& other ) -> ProgramSource& = default;
    auto operator=( ProgramSource&& other ) -> ProgramSource& = default;

    virtual ~ProgramSource() = default;

protected:
    auto listDirectory( std::filesystem::path path, const std::string& programName, std::string_view targetExt )
        -> std::vector<std::string> ;

    auto stageType( const std::string& shaderFileName ) -> ShaderStageType ;

    // Split given string by seperator
    auto split( const std::string& string, const char* sep ) -> std::vector<std::string> ;

private:
    // Finds out that given shader file name contains somthing from
    // shader stage suffix set ("VERTEX", "FRAGMENT" etc.)
    auto isValidName( const std::string& name ) -> bool ;

private:
    std::string _programName{};
};

}  // namespace tire
