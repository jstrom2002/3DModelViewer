#include "stdafx.h"
#include "Utils.hpp"
#include "structs.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "CPPfilesys.hpp"

namespace TDModelView 
{
    std::string getDateTime() 
    {//See: https://stackoverflow.com/questions/997512/string-representation-of-time-t
        std::time_t now = std::time(NULL);
        std::tm* ptm = std::localtime(&now);
        char buffer[32];
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        return std::string(buffer);
    }

    void WriteToLogFile(std::string str)
    {
        std::ofstream outp("runtime.log", std::ios::out | std::ios::binary | std::ios::app);
        if (!outp.is_open())
            return;
        outp << "\n" << getDateTime() << "\t MESSAGE: " << str;
        outp.close();
    }
    
    std::string replaceString(std::string str, std::string replace, std::string replacer) 
    {
        if (str.find(replace) == std::string::npos)
            return str;
        for (int i = 0; i <= str.length(); ++i) {
            std::string sub = str.substr(i, str.length());
            int pos = sub.find(replace) + i;
            if (sub.find(replace) == std::string::npos || pos < 0 || sub.find(replace) > str.length() - 1) {
                //break if done replacing
                i = str.length() + 1;
                pos = std::string::npos;//set position to NULL for string type
            }
            if (pos >= 0) {
                str.erase(pos, replace.length());
                str.insert(pos, replacer);
                i = pos + replacer.length() - 1;
            }
        }
        return str;
    }
    
    std::string removeNonAlphaChars(std::string str) 
    {
        if (str.size() <= 0) { return ""; }
        for (int i = 0; i < str.length(); ++i)
            if ((str[0] < 'a' || str[0] > 'z') &&
                (str[0] < 'A' || str[0] > 'Z') &&
                (str[0] < '0' || str[0] > '9'))
            {
                str.erase(str.begin() + i);
                --i;
            }
        return str;
    }

    std::string trimWhitespace(std::string str) 
    {
        while (str.length()>0&&str[0]==' ')
            str=str.substr(1);
        while (str.length()>0&& str[str.length()-1]==' ')
            str=str.substr(0,str.length()-1);
        return str;
    }
    
    std::string getDirectory(std::string str)
    {
        if (std::filesystem::is_directory(std::filesystem::path(str)) ||
            str.back() == '\\' || str.back() == '/')
            return str;
        if (str.find("/") != std::string::npos)
            str = str.substr(0, str.rfind("/") + 1);
        if (str.find("\\") != std::string::npos)
            str = str.substr(0, str.rfind("\\") + 1);
        while (str[0] == '\\')
            str = str.substr(1);
        str = replaceString(str, "\\\\", "\\");

        return str;
    }

    std::string getExtension(std::string str)
    {
        str = toLowercase(str);
        if (str.rfind(".") == std::string::npos)
            return "";
        str = str.substr(str.rfind("."));
        return str;
    }
    
    std::string getFilename(std::string str) 
    {
        while (str.rfind("/") != std::string::npos && str.length() > 0) {
            str = str.substr(str.rfind("/") + 1);
        }
        while (str.rfind("\\") != std::string::npos && str.length() > 0) {
            str = str.substr(str.rfind("\\") + 1);
        }
        return str;
    }
    
    std::vector<std::string> tokenize(std::string toTokenize, std::string token) 
    {
        std::vector<std::string> result;
        char* tk = strtok((char*)toTokenize.c_str(), token.c_str());
        while (tk != NULL){
            result.push_back(tk);
            tk = strtok(NULL, token.c_str());
        }
        return result;
    }
    
    bool checkError(std::string details)
    {
        int err = 0;
        {
            static const int MAX_ERRORS = 15;
            int errorCounter = 0;
            err = glGetError();
            while (err != GL_NO_ERROR && errorCounter < MAX_ERRORS)
            {
                std::string errString = "OpenGL Error: ";
                switch (err) {
                case GL_INVALID_ENUM:
                    errString.append("GL_INVALID_ENUM");
                    break;
                case GL_INVALID_VALUE:
                    errString.append("GL_INVALID_VALUE");
                    break;
                case GL_INVALID_OPERATION:
                    errString.append("GL_INVALID_OPERATION");
                    break;
                case GL_STACK_OVERFLOW:
                    errString.append("GL_STACK_OVERFLOW");
                    break;
                case GL_STACK_UNDERFLOW:
                    errString.append("GL_STACK_UNDERFLOW");
                    break;
                case GL_OUT_OF_MEMORY:
                    errString.append("GL_OUT_OF_MEMORY");
                    break;
                }
                errString.append("\nCode: " + std::to_string(err));
                errString.append("\nHas Current Context? " + std::to_string(glfwGetCurrentContext() != nullptr));
                errString.append("\nDetails: " + details);
                ErrorMessageBox(errString);
                errorCounter++;
                err = glGetError();
            }
        }
        return err > 0;
    }

    void ErrorMessageBox(std::string str)
    {
        errorString.append("\n" + str);

        if (eng && !eng->windowClose && !eng->silenceErrors)
        {
#ifdef _DEBUG
#ifdef _WIN32
            MessageBox(0, str.c_str(), 0, 0);
#endif
            throw new std::exception(str.c_str());
#endif
        }

        WriteToLogFile(str);
    }
    
    std::string toLowercase(std::string str)
    {
        std::transform(str.begin(),str.end(),str.begin(),[](unsigned char c){return std::tolower(c);});
        return str;
    }
    
    std::string checkFilepath(std::string filepath_, std::string local_directory)
    {
        std::string filepath = filepath_;
        trimWhitespace(filepath_);        
        CPPfilesys::FileDetails fd(filepath_, local_directory);
        return fd.absoluteFilepath;
    }
}