#include <gtest/gtest.h>
#include "pipeline.h"
#include <fstream>
#include <filesystem>

// 
static string write_tmp(const string& name, const string& content, bool keep = false) {
    filesystem::path p = filesystem::temp_directory_path() / name;
    ofstream ofs(p);
    ofs << content;
    ofs.close();

    cout << "[Test] Created temporary file: " << p << endl;

    if (!keep) {
        filesystem::remove(p);
    }
    return p.string();
}

TEST(Pipeline, ConfigReadError){ // Create a Test for ConnfigReadError
    auto result = LoadConfig("non_existent_file.txt")
        .and_then([](const Config& c){ return ValidateData(c); });

    visit([&](auto&& e){
        using T = decay_t<decltype(e)>;
        if constexpr (is_same_v<T, ConfigReadError>) {
            EXPECT_EQ(e.filename, "non_existent_file.txt");  
        } else {
            FAIL() << "Expected ConfigReadError, got: " << ErrorToString((PipelineError)e);
        }
    }, result.error());
}


TEST(Pipeline, ValidationError) { // Create a Test for ValidationError
    string path = write_tmp("out_of_range.cfg", "threshold=1.5\n", true);
    auto cfg = LoadConfig(path);
    filesystem::remove(path);

    auto v = ValidateData(*cfg);
    visit([&](auto&& e){
        using T = decay_t<decltype(e)>;
        if constexpr (is_same_v<T, ValidationError>) {
            EXPECT_TRUE(e.reason.find("out of [0,1]") != string::npos);
        } else {
            FAIL() << "Expected ValidationError, got: " << ErrorToString((PipelineError)e);
        }
    }, v.error());
}

