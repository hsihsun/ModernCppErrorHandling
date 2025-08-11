#include "pipeline.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <variant>

static expected<string, PipelineError> ReadAll(const string& filename) {
    ifstream ifs(filename);
    if (!ifs) {
        return unexpected(PipelineError{ConfigReadError{filename}});
    }
    ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

// analyze key=value，only "threshold=<number>"
static expected<Config, PipelineError> ParseConfigText(const string& text) {
    istringstream iss(text);
    string line;
    int line_no = 0;
    Config cfg{};

    while (getline(iss, line)) {
        ++line_no;

        if (line.empty() || line[0] == '#') continue;

        auto pos = line.find('=');
        if (pos == string::npos) {
            return unexpected(PipelineError{ConfigParseError{line_no, "expect key=value"}});
        }
        string key = line.substr(0, pos);
        string val = line.substr(pos + 1);

        if (key != "threshold") {
            return unexpected(PipelineError{ConfigParseError{line_no, "unknown key: " + key}});
        }

        try {
            size_t idx = 0;
            double x = stod(val, &idx);
            if (idx != val.size()) {
                return unexpected(PipelineError{ConfigParseError{line_no, "invalid number: " + val}});
            }
            cfg.threshold = x;
        } catch (...) {
            return unexpected(PipelineError{ConfigParseError{line_no, "invalid number: " + val}});
        }
    }

    return cfg;
}

expected<Config, PipelineError> LoadConfig(const string& filename) {
    return ReadAll(filename).and_then(ParseConfigText);
}

expected<double, PipelineError> ValidateData(const Config& cfg) {
    if (cfg.threshold < 0.0 || cfg.threshold > 1.0) {
        ostringstream oss;
        oss << "threshold=" << cfg.threshold << " out of [0,1]";
        return unexpected(PipelineError{ValidationError{oss.str()}});
    }
    return cfg.threshold;
}


string ErrorToString(const PipelineError& err) {
    return visit([&](auto&& e)->string{
        using T = decay_t<decltype(e)>;
        ostringstream oss;
        if constexpr (is_same_v<T, ConfigReadError>) {
            oss << "[ConfigReadError] file='" << e.filename << "'";
        } else if constexpr (is_same_v<T, ConfigParseError>) {
            oss << "[ConfigParseError] line=" << e.line_num << " msg='" << e.message << "'";
        } else if constexpr (is_same_v<T, ValidationError>) {
            oss << "[ValidationError] " << e.reason;
        } else {
            oss << "unknown error";
        }
        return oss.str();
    }, err);
}
