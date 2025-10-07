#include <TString.h>
#include <TSystem.h>
#include <iostream>
#include <vector>
#include <string>

// Function to parse a JSON-like string array: e.g., "[\"file1.root\", \"file2.root\"]"
std::vector<std::string> parseJsonStringArray(const char* jsonArray) {
    std::vector<std::string> files;
    std::string str = jsonArray;
    
    // Remove leading '[' and trailing ']'
    size_t first = str.find('[');
    size_t last = str.rfind(']');
    if (first == std::string::npos || last == std::string::npos) {
        std::cerr << "Error: Invalid JSON array format." << std::endl;
        return files;
    }
    str = str.substr(first + 1, last - first - 1);

    // Split by comma
    std::string delimiter = ",";
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        // Remove quotes and whitespace
        token.erase(0, token.find_first_not_of(" \t\n\r\""));
        token.erase(token.find_last_not_of(" \t\n\r\"") + 1);
        if (!token.empty()) {
            files.push_back(token);
        }
        str.erase(0, pos + delimiter.length());
    }
    // Add the last element
    str.erase(0, str.find_first_not_of(" \t\n\r\""));
    str.erase(str.find_last_not_of(" \t\n\r\"") + 1);
    if (!str.empty()) {
        files.push_back(str);
    }
    
    return files;
}

void merge_files(const char* output_file, const char* input_files_json) {
    std::cout << "Processing merge_files.C(\"" << output_file << "\", " << input_files_json << ") ..." << std::endl;

    std::vector<std::string> input_files = parseJsonStringArray(input_files_json);

    if (input_files.empty()) {
        std::cerr << "Error: No input files to merge." << std::endl;
        return;
    }

    TString command = "hadd -f ";
    command += output_file;

    for (const auto& file : input_files) {
        // Check if file exists before adding
        if (gSystem->AccessPathName(file.c_str())) {
            std::cerr << "Warning: Input file does not exist and will be skipped: " << file << std::endl;
            continue;
        }
        command += " ";
        command += file;
    }

    std::cout << "Executing command: " << command << std::endl;
    int result = gSystem->Exec(command);

    if (result == 0) {
        std::cout << "Successfully merged files into " << output_file << std::endl;
    } else {
        std::cerr << "Error: hadd command failed with exit code " << result << std::endl;
    }
}

