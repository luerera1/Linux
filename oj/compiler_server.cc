#include"httplib.h"
#include"compile.hpp"

int main(){
    using namespace httplib;
    Server server;

    server.Post("/compile",[](const Request& req,Response& resp){
        (void)req;
	std::unordered_map<std::string,std::string> body_kv;
	ParseBody(req.body,&body_kv);
        Json::Value req_json;
	for(auto p :body_kv){
		req_json[p.first] = p.second;
	}
        Json::Value resp_json;
        Compiler::CompileAndRun(req_json,&resp_json);
        Json::FastWriter writer;
        resp.set_content(writer.write(resp_json),"text/plain");
            });
    server.listen("0.0.0.0",9092);
    return 0;
}
