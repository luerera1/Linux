#pragma once
#include<string>
#include<vector>
#include<fstream>
#include<map>
#include<unordered_map>
#include"util.hpp"
struct Question{
    std::string id;
    std::string name;
    std::string dir;
    std::string star;
    std::string desc;
    std::string header_cpp;
    std::string tail_cpp;

};

class OjModel{
    private:
        std::map<std::string,Question> model_;
    public:
        bool Load(){
            std::ifstream file("./oj_data/oj_config.cfg");
            if(!file.is_open()){
                return false;
            }
            std::string line;
            while(std::getline(file,line)){
                std::vector<std::string> tokens;
                StringUtil::Split(line,"&",&tokens);
                if(tokens.size()==4){
                    LOG(ERROR) <<"config file format error!\n";
                    continue;
                }
                Question q;
                q.id = tokens[0];
                q.name = tokens[1];
                q.star = tokens[2];
                q.dir = tokens[3];
                FileUtil::Read(q.dir+"/desc.txt",&q.desc);
                FileUtil::Read(q.dir+"/header.cpp",&q.desc);
                FileUtil::Read(q.dir+"/tail.cpp",&q.desc);
                model_[q.id] = q;
            }
            file.close();
            LOG(INFO) << "Load" << model_.size() << "question\n";
            return true;
        }
        bool GetAllQuestions(std::vector<Question>* question){
             question->clear();
             for(const auto& kv:model_){
                question->push_back(kv.second);
             }
             return true;
        }
        bool GetQuestion(const std::string& id,Question* q)const {
            auto pos = model_.find(id);
            if(pos == model_.end()){
                return false;
            }
            *q = pos->second;
            return true;
        }



};
