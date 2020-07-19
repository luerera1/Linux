#pragma once
#include<iostream>
#include<cstdlib>
#include<unordered_map>
#include<vector>
#include<string>
#include<stdint.h>
#include<sys/time.h>
#include<fstream>
#include<boost/algorithm/string.hpp>

class TimeUtil{
    public:
        static int64_t TimeStamp(){
            struct timeval tv;
            ::gettimeofday(&tv,NULL);
            return tv.tv_sec;
        }
        static int64_t TimeStampMS(){
            struct timeval tv;
            ::gettimeofday(&tv,NULL);
            return tv.tv_sec*1000+tv.tv_usec/1000;
        }
};

enum Level{
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

inline std::ostream& Log(Level level,const std::string& file_name,int line_num){
    std::string prefix = "[";
    if(level==INFO){
        prefix += "I";
    }else if(level==WARNING){
        prefix += "w";
    }else if(level==ERROR){
        prefix += "E";
    }else if(level==FATAL){
        prefix += "F";
    }
    prefix += std::to_string(TimeUtil::TimeStamp());
    prefix += " ";
    prefix += file_name;
    prefix += ":";
    prefix += std::to_string(line_num);
    prefix += "]";
    std::cout << prefix;
    return std::cout;
}

#define LOG(level) Log(level,__FILE__,__LINE__)

class FileUtil{
    public:
        static bool Read(const std::string& file_path,std::string* content){
            content->clear();
            std::ifstream file(file_path.c_str());
            if(!file.is_open()){
                return false;
            }
            std::string line;
            while(std::getline(file,line)){
                *content += line + "\n";
            }
            file.close();
            return true;
       }
        static bool Write(const std::string& file_path,const std::string& content){
            std::ofstream file(file_path.c_str());
            if(!file.is_open()){
                return false;
            }
            file.write(content.c_str(),content.size());
            file.close();
            return true;
        }
};

class StringUtil{
    public:
        static void Split(const std::string& input,const std::string& split_char,std::vector<std::string>*output){
            boost::split(*output,input,boost::is_any_of(split_char),boost::token_compress_on);
        }
};

class UrlUtil{
    public:
        static void ParseBody(const std::string& body,std::unordered_map<std::string,std::string>*params){
        std::vector<std::string> kvs;
        StringUtil::Split(body,"&",&kvs);
        for(size_t i=0;i<kvs.size();++i){
            std::vector<std::string> kv;
            StringUtil::Split(kv[i],"=",&kv);
            if(kv.size()!=2){
                continue;
            }
            (*params)[kv[0]] =UrlDecode(kv[1]);
        }
    }
      static  unsigned char ToHex(unsigned char x){ 
            return  x > 9 ? x + 55 : x + 48; 
    }
 
      static  unsigned char FromHex(unsigned char x) 
        { 
            unsigned char y;
            if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
            else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
            else if (x >= '0' && x <= '9') y = x - '0';
            else assert(0);
            return y;
        }
 
      static  std::string UrlEncode(const std::string& str)
        {
            std::string strTemp = "";
            size_t length = str.length();
            for (size_t i = 0; i < length; i++)
            {
                if (isalnum((unsigned char)str[i]) || 
                    (str[i] == '-') ||
                    (str[i] == '_') || 
                    (str[i] == '.') || 
                    (str[i] == '~'))
                    strTemp += str[i];
                else if (str[i] == ' ')
                strTemp += "+";
                else
                {
                    strTemp += '%';
                    strTemp += ToHex((unsigned char)str[i] >> 4);
                    strTemp += ToHex((unsigned char)str[i] % 16);
                }
            }
        return strTemp;
        }
 
        static std::string UrlDecode(const std::string& str)
        {
            std::string strTemp = "";
            size_t length = str.length();
            for (size_t i = 0; i < length; i++)
            {
                if (str[i] == '+') strTemp += ' ';
                else if (str[i] == '%')
                {
                    assert(i + 2 < length);
                    unsigned char high = FromHex((unsigned char)str[++i]);
                    unsigned char low = FromHex((unsigned char)str[++i]);
                    strTemp += high*16 + low;
                }
            else strTemp += str[i];
        }
            return strTemp;
        }

};

