#include"httplib.h"
#include"util.hpp"
#include"oj_model.hpp"
#include"oj_view.hpp"
#include<jsoncpp/json/json.h>
#include"compile.hpp"
int main(){
    OjModel model;
    model.Load();

    using namespace httplib;
    Server server;
    server.Get("/all_question",[&model](const Request& req,Response& resp){
            (void)req;
            std::vector<Question> all_questions;
            model.GetAllQuestions(&all_questions);
            std::string html;
            OjView::RenderAllQuestion(all_questions,&html);
            resp.set_content(html,"text/html");
            });
    server.Get(R"(/question/(\d+))",[&model](const Request& req,Response& resp){
            Question question;
            model.GetQuestion(req.matches[1].str(),&question);
            std::string html;
            OjView::RenderQuestion(question,&html);
            resp.set_content(html,"text/html");
            });
    server.Post("/compile",[](const Request& req,Response& resp){
            Question question;
            model.GetQuestion(req.matches[1].str(),&question);

            std::unordered_map<std::string,std::string> body_kv;
            UrlUtil::ParseBody(req.body,&body_kv);
            const std::string& user_code = body_kv["code"];
       
            Json::Value req_json;
            req_json["code"] = user_code + question.tail_cpp;
            Json::Value resp_json;
            Compiler::CompileAndRun(req_json,&resp_json);
            
            std::string html;
            OjView::RenderResult(resp_json["stdout"].asString(),
                    resp_json["reason"].asString(),&html);
            });
    server.set_base_dir("./wwwroot");
    server.listen("0.0.0.0",9092);
    return 0;
}
