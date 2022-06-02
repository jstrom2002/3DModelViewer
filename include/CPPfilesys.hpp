/* MIT License
Copyright (c) 2022 JH Strom
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#pragma once
// Must at least be using c++17 for 'filesystem'. Note that MSVC doesn't use __cplusplus macro the same as other compilers.
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L) 
#include <exception>
#include <filesystem>
#include <string>
#ifdef _CPPFSTESTS__
#include "CPPfsTests.hpp"
#endif

namespace CPPfilesys
{
    // Class for parsing file path details from a string.
    class FileDetails
    {
    private:
        enum SystemType { UNKNOWN_FILESYSTEM = 0u, DOS_FILESYSTEM = 1u, POSIX_FILESYSTEM = 2u, APPLE_FILESYSTEM = 3u, Z_FILESYSTEM = 4u };
        char PATH_SEP = '\\';
        std::string PATH_LOCAL = ".\\";
        std::string PATH_UP = "..\\";

        FileDetails()
        {
            throw std::exception("ERROR! Cannot instantiate class with default constructor.");
        }

    public:

#ifdef _CPPFSTESTS__
        friend class CPPfsTests;
#endif

        std::string absoluteFilepath = "";
        std::string directory = "";
        std::string relativeFilepath = "";
        SystemType type = UNKNOWN_FILESYSTEM;


        // Classify input file into 
        FileDetails(const std::string& inp_filepath, const std::string& source_filepath = "") {
            // Detect system OS and determine filesystem type.
#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
            type = DOS_FILESYSTEM;
#elif defined(unix)||defined(__unix)||defined(__unix__)||defined(__linux__)
            type = POSIX_FILESYSTEM;
#elif defined(__APPLE__ )||defined(__MACH__)
            type = APPLE_FILESYSTEM;
#elif defined(__FreeBSD__)
            type = Z_FILESYSTEM;
#endif
            // Begin parsing filepath strings.
            bool hasLocalDir = source_filepath.length() > 0;
            std::string fp_str = trimWhitespace(inp_filepath);
            std::string fp_filename = getFilename(fp_str);
            std::string fp_directory = getDirectory(fp_str);
            std::filesystem::path fp(inp_filepath);

            // Bail early conditions: existing filesystem is not parseable or filepath is not to an actual file.
            if (type == UNKNOWN_FILESYSTEM || fp_filename.length() == 0) {
                CPP_FILESYSTEM_EXCEPTION("ERROR! System file system type could not be determined by compiler.");
                return;
            }

            relativeFilepath = fp_str;

            // Check quit conditions. 
            if (fp.is_absolute()) {
                absoluteFilepath = fp_str;
                return;
            }

            // Handle relative path folder navigation, ie "..\\..\\filename.ext".
            if (hasLocalDir) {
                doNavigation(fp_str, source_filepath);
                fp = std::filesystem::path(fp_str);
                // Check quit conditions. 
                if (fp.is_absolute()) {
                    absoluteFilepath = fp_str;
                    return;
                }
            }

            // If all else fails, search all subfolders recursively for this file.
            try {
                for (auto& p : std::filesystem::recursive_directory_iterator(directory, std::filesystem::directory_options::skip_permission_denied))
                {
                    if (std::filesystem::is_regular_file(p) && p.path().filename().string() == fp_filename)
                    {
                        absoluteFilepath = p.path().string();
                        return;
                    }
                }
            }
            catch (std::exception e1) {
                // NO OP.
            }

            // Fail by default if no absolute filepath is found for given file.
            CPP_FILESYSTEM_EXCEPTION("ERROR! Could not find absolute path to file.");
        }

    protected:
        void CPP_FILESYSTEM_EXCEPTION(std::string str) {
            throw new std::exception(str.c_str());
        }

        std::string toLowercase(std::string str) { for (size_t i = 0; i < str.length(); ++i) { str[i] = std::tolower(str[i]); } return str; }
        std::string replaceString(std::string str, std::string replace, std::string replacer) {
            if (str.find(replace) == std::string::npos)
                return str;
            for (int i = 0; i <= str.length(); ++i) {
                std::string sub = str.substr(i, str.length());
                int pos = sub.find(replace) + i;
                if (sub.find(replace) == std::string::npos || pos < 0 || sub.find(replace) > str.length() - 1) {
                    i = str.length() + 1;
                    pos = std::string::npos;
                }
                if (pos >= 0) {
                    str.erase(pos, replace.length());
                    str.insert(pos, replacer);
                    i = pos + replacer.length() - 1;
                }
            }
            return str;
        }
        std::string trimWhitespace(std::string str) {
            while (str.length() > 0 && str[0] == ' ')
                str = str.substr(1);
            while (str.length() > 0 && str[str.length() - 1] == ' ')
                str = str.substr(0, str.length() - 1);
            return str;
        }

        std::string getDirectory(std::string str) {
            if (std::filesystem::is_directory(std::filesystem::path(str)) ||
                str.back() == PATH_SEP)
                return str;
            if (str.find("/") != std::string::npos)
                str = str.substr(0, str.rfind("/") + 1);
            if (str.find(PATH_SEP) != std::string::npos)
                str = str.substr(0, str.rfind(PATH_SEP) + 1);
            while (str[0] == PATH_SEP)
                str = str.substr(1);
            str = replaceString(str, "\\\\", "\\");//some filesystems allow multiple slashes in paths, treated similarly to a single path separator

            return str;
        }

        std::string getExtension(const std::string& str) {
            std::string ret_ = "";

            if (type == DOS_FILESYSTEM) { // DOS standard files: 'name.extension'
                ret_ = toLowercase(ret_);
                if (ret_.rfind(".") == std::string::npos)
                    return "";
                ret_ = str.substr(ret_.rfind("."));
            }
            else
            {
                // TO DO: handle other file systems, ie Linux does not require extensions.
            }

            return ret_;
        }

        std::string getFilename(std::string str) {
            while (str.rfind("/") != std::string::npos && str.length() > 0) {
                str = str.substr(str.rfind("/") + 1);
            }
            while (str.rfind(PATH_SEP) != std::string::npos && str.length() > 0) {
                str = str.substr(str.rfind(PATH_SEP) + 1);
            }
            return str;
        }

        void doNavigation(std::string& fp, const std::string& fp_dir) {
            if (fp_dir.length() == 0)
                return;
            std::string new_dir = fp_dir;
            if (fp.substr(0, PATH_UP.length()) == PATH_UP) {
                int inc_folders_up = 0;
                while (fp.substr(0, PATH_UP.length()) == PATH_UP) {
                    fp = fp.substr(PATH_UP.length());
                    inc_folders_up++;
                }
                while (new_dir[new_dir.length() - 1] == PATH_SEP) // remove ending char if it's '\\' etc to prevent false positives in backtracking up file tree.
                    new_dir = new_dir.substr(0, new_dir.length() - 1);
                while (inc_folders_up > 0)
                {
                    new_dir = new_dir.substr(0, new_dir.rfind(PATH_SEP));
                    inc_folders_up--;
                }

            }
            else if (fp.substr(0, PATH_LOCAL.length()) == PATH_LOCAL)
                fp = fp.substr(PATH_LOCAL.length());
            fp = new_dir + PATH_SEP + fp;
        }
    };
}
#endif