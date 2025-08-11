#pragma once
#include <expected>
#include <string>
#include <variant>
using namespace std;

struct ConfigReadError { string filename; };
struct ConfigParseError { int line_num{}; string message; };
struct ValidationError { string reason; };

using PipelineError = variant<ConfigReadError, ConfigParseError, ValidationError>;

struct Config { double threshold{0.2}; };

[[nodiscard]] expected<Config, PipelineError> LoadConfig(const string& filename);
[[nodiscard]] expected<double, PipelineError> ValidateData(const Config& cfg);

[[nodiscard]] string ErrorToString(const PipelineError& err);
