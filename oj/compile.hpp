#pragma once
#include<string>
#include<atomic>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<jsoncpp/json/json.h>
#include"util.hpp"


class Compiler{
    public:
        static std::string SrcPath(const std::string& name){
            return "./temp_files/" + name +".cpp";
        }
        static std::string CompileErrorPath(const std::string& name){
            return "./temp_files/" + name +".compile_error";
        }
        static std::string ExePath(const std::string& name){
            return "./temp_files/" + name +".exe";
        }
        static std::string StdinPath(const std::string& name){
            return "./temp_files/" +name +".stdin";
        }
        static std::string StdoutPath(const std::string& name){
            return "./temp_files/" + name +".stdout"; 
        }
        static std::string StderrPath(const std::string& name){
            return "./temp_files/" + name +".stderr";
        }
        
        
        static bool CompileAndRun(const Json::Value& req,Json::Value* resp){
            if(req["code"].empty()){
                (*resp)["error"] = 3;
                (*resp)["reason"] = "code empty";
                LOG(ERROR) << "code empty" << std::endl;
                return false;
            }
            const std::string& code = req["code"].asString();
            std::string file_name = WriteTmpFile(code,req["stdin"].asString());
            bool ret = Compile(file_name);
            if(!ret){
                (*resp)["error"] = 1;
                std::string reason;
                FileUtil::Read(CompileErrorPath(file_name),&reason);
                (*resp)["reason"] = reason;
                LOG(INFO) << "Compile failed!" << std::endl;
                return false;
            }
            int sig= Run(file_name);
            if(sig!=0){
                (*resp)["error"] = 2;
                (*resp)["reason"] = "Program exit by signo:" + std::to_string(sig);
                LOG(INFO) << "Program exit by signo" << std::to_string(sig) << std::endl;
                return false;
            }
            (*resp)["error"] = 0;
            (*resp)["reason"] ="";
            std::string str_stdout;
            FileUtil::Read(StdoutPath(file_name),&str_stdout);
            (*resp)["stdout"] = str_stdout;
            std::string str_stderr;
            FileUtil::Read(StderrPath(file_name),&str_stderr);
            (*resp)["stderr"] = str_stderr;
            LOG(INFO) << "Program" << file_name << "Done" << std::endl;
            return true;
        }
    private:
        static std::string WriteTmpFile(const std::string& code,const std::string& str_stdin){
            static std::atomic_int id(0);
            ++id;
            std::string file_name = "tmp_"+std::to_string(TimeUtil::TimeStamp())+"."+std::to_string(id);
            FileUtil::Write(SrcPath(file_name),code);
            FileUtil::Write(StdinPath(file_name),str_stdin);
            return file_name;
        }
        static bool Compile(const std::string& file_name){
            char* command[20]={0};
            char buf[20][50]={{0}};
            for(int i=0;i<20;i++){
                command[i]=buf[i];
            }
            sprintf(command[0],"%s","g++");
            sprintf(command[1],"%s",SrcPath(file_name).c_str());
            sprintf(command[2],"%s","-o");
            sprintf(command[3],"%s",ExePath(file_name).c_str());
            sprintf(command[4],"%s","-std=c++11");
            command[5]=NULL;

            int ret =fork();
            if(ret>0){
                waitpid(ret,NULL,0);
            }else{
                int fd = open(CompileErrorPath(file_name).c_str(),O_WRONLY|O_CREAT,0666);
                if(fd<0){
                    LOG(ERROR) << "open Compile file error" << std::endl;
                    exit(1);
                }
                dup2(fd,2);
                execvp(command[0],command);
                exit(0);
            }
            struct stat st;
            ret = stat(ExePath(file_name).c_str(),&st);
            if(ret<0){
                LOG(INFO) << "Compile failed!" << file_name << std::endl;
                return false;
            }
            LOG(INFO) << "Compile" << file_name << "ok" << std::endl;
            return true;
        }
        static int Run(const std::string& file_name){
            int ret = fork();
            if(ret>0){
                int status = 0;
                waitpid(ret,&status,0);
                return status & 0x7f;
            }else{
                int fd_stdin = open(StdinPath(file_name).c_str(),O_RDONLY);
                dup2(fd_stdin,0);
                int fd_stdout = open(StdoutPath(file_name).c_str(),O_WRONLY | O_CREAT,0666);
                dup2(fd_stdout,1);
                int fd_stderr = open(StderrPath(file_name).c_str(),O_WRONLY| O_CREAT,0666);
                dup2(fd_stderr,2);
                
                execl(ExePath(file_name).c_str(),ExePath(file_name).c_str(),NULL);
                exit(0);
            }

        }
};
