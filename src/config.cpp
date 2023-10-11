#include "../include/config.h"

namespace frb{

     

     ConfigVarBase::ptr Config::LookUpBase(const std::string& name) {
          RWMutexType::ReadLock lock(GetMutex());
          auto it = Config::GetDatas().find(name);
          return it == Config::GetDatas().end() ? nullptr : it->second;

     }

     
     static void ListAllMember(const std::string& prefix, YAML::Node& node
                              , std::list<std::pair<std::string , YAML::Node>>& output) {
          if(prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
                    != std::string::npos) {
               LOG_ERROR(GET_LOG_ROOT, "Config invalid name: ");
               return;
          }

          output.push_back(std::make_pair(prefix, node));

          if(node.IsMap()) {
               for(auto it = node.begin();
                         it != node.end(); ++it) {
                    ListAllMember(prefix.empty() ? it->first.Scalar()
                              : prefix + "." + it->first.Scalar(), it->second, output);
               }
          }

     }

     void Config::LoadFromYaml(YAML::Node& root) {
          std::list<std::pair<std::string, YAML::Node>> all_nodes;
     


          //将yaml文件中的各参数记录在all_nodes中
          ListAllMember("", root, all_nodes);    

          for(auto& i : all_nodes) {
               std::string key = i.first;
               if(key.empty())
                    continue;
               
               std::transform(key.begin(), key.end(), key.begin(), ::tolower);

               //在全局的配置参数图s_datas中查找该参数是否存在
               //存在：用yaml文件中的值覆盖
               //不存在：说明系统中没有用到该参数，不用处理
               ConfigVarBase::ptr var = LookUpBase(key);

               if(var){

                    if(i.second.IsScalar()) {
                         var->fromString(i.second.Scalar());
                    } else {
                         std::stringstream ss;
                         ss << i.second;
                         var->fromString(ss.str());
                    }
               }
               
          }


     }
    
}