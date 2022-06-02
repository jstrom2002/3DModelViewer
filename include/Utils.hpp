#pragma once
#include <string>
#include <string>
#include <vector>

namespace TDModelView
{
    void ErrorMessageBox(std::string);
    std::string toLowercase(std::string str);
    std::string getExtension(std::string str);
    std::string replaceString(std::string str, std::string replace, std::string replacer);
    std::string removeNonAlphaChars(std::string str);
    std::string trimWhitespace(std::string str);
    std::string getDirectory(std::string str);
    std::string getFilename(std::string str);
    std::vector<std::string> tokenize(std::string toTokenize, std::string token);
    bool checkError(std::string details);
    void WriteToLogFile(std::string str);
    std::string getDateTime();
    std::string checkFilepath(std::string filepath, std::string local_directory = "");
}