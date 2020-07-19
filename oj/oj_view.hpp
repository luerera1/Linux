#pragma once

#include"oj_model.hpp"
#include<ctemplate/template.h>

class OjView{
public:
    static void RenderAllQuestion(const std::vector<Question>& all_question,std::string* html){
        ctemplate::TemplateDictionary dict("all_question");
        for(const auto& question:all_question){
            ctemplate::TemplateDictionary* table_dict=dict.AddSectionDictionary("question");
            table_dict->SetValue("id",question.id);
            table_dict->SetValue("name",question.name);
            table_dict->SetValue("star",question.star);
        }
        ctemplate::Template* tpl;
        tpl = ctemplate::Template::GetTemplate("./template/all_question.html",
                ctemplate::DO_NOT_STRIP);
        tpl->Expand(html,&dict);
    }
    static void RenderQuestion(const Question& question,std::string* html){
        ctemplate::TemplateDictionary dict("question");
        dict.SetValue("id",question.id);
        dict.SetValue("name",question.name);
        dict.SetValue("star",question.star);
        dict.SetValue("desc",question.desc);
        dict.SetValue("header",question.header_cpp);
        ctemplate::Template* tpl;
        tpl = ctemplate::Template::GetTemplate("./template/question.html",
                  ctemplate::DO_NOT_STRIP);
        tpl->Expand(html,&dict);
    }
};
