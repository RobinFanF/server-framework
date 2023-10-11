#pragma once
#include <string>
#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>

#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>


#include "log.h"
#include "utils.h"


namespace frb{

    class ConfigVarBase{
        public:
            typedef std::shared_ptr<ConfigVarBase> ptr;

            ConfigVarBase(const std::string& name, const std::string& des):
                m_name(name), m_description(des){
                    std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
                }
            
            virtual ~ConfigVarBase() {};

            virtual std::string toString()  = 0;
            virtual bool fromString(const std::string& val) = 0;

            virtual std::string getTypeName() const = 0;


        protected:
            std::string m_name;
            std::string m_description;
    };

    //类型转换模板类
    template<class S, class T>
    class LexicalCast{
        public:
            T operator()(const S& v){
               return boost::lexical_cast<T>(v);
            }
    };

    //类型装换偏特化：（YAML string 到 vector<T>）
    template<class T>
    class LexicalCast<std::string, std::vector<T>>{
        public:
            std::vector<T> operator()(const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::vector<T> vec;
                std::stringstream ss;
                for(size_t i = 0; i < node.size(); ++i){
                    ss.str("");
                    ss << node[i];
                    vec.push_back(LexicalCast<std::string, T>()(ss.str()));
                }

                return vec;
            }
    };

    /**
    * @brief 类型转换模板类片特化(std::vector<T> 转换成 YAML String)
     */
    template<class T>
    class LexicalCast<std::vector<T>, std::string> {
    public:
        std::string operator()(const std::vector<T>& v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for(auto& i : v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief 类型转换模板类片特化(YAML String 转换成 std::list<T>)
     */
    template<class T>
    class LexicalCast<std::string, std::list<T> > {
    public:
        std::list<T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::list<T> vec;
            std::stringstream ss;
            for(size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

    /**
     * @brief 类型转换模板类片特化(std::list<T> 转换成 YAML String)
     */
    template<class T>
    class LexicalCast<std::list<T>, std::string> {
    public:
        std::string operator()(const std::list<T>& v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for(auto& i : v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief 类型转换模板类片特化(YAML String 转换成 std::set<T>)
     */
    template<class T>
    class LexicalCast<std::string, std::set<T> > {
    public:
        std::set<T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::set<T> vec;
            std::stringstream ss;
            for(size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

    /**
     * @brief 类型转换模板类片特化(std::set<T> 转换成 YAML String)
     */
    template<class T>
    class LexicalCast<std::set<T>, std::string> {
    public:
        std::string operator()(const std::set<T>& v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for(auto& i : v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_set<T>)
     */
    template<class T>
    class LexicalCast<std::string, std::unordered_set<T> > {
    public:
        std::unordered_set<T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_set<T> vec;
            std::stringstream ss;
            for(size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

    /**
     * @brief 类型转换模板类片特化(std::unordered_set<T> 转换成 YAML String)
     */
    template<class T>
    class LexicalCast<std::unordered_set<T>, std::string> {
    public:
        std::string operator()(const std::unordered_set<T>& v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for(auto& i : v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief 类型转换模板类片特化(YAML String 转换成 std::map<std::string, T>)
     */
    template<class T>
    class LexicalCast<std::string, std::map<std::string, T> > {
    public:
        std::map<std::string, T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::map<std::string, T> vec;
            std::stringstream ss;
            for(auto it = node.begin();
                    it != node.end(); ++it) {
                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                            LexicalCast<std::string, T>()(ss.str())));
            }
            return vec;
        }
    };

    /**
     * @brief 类型转换模板类片特化(std::map<std::string, T> 转换成 YAML String)
     */
    template<class T>
    class LexicalCast<std::map<std::string, T>, std::string> {
    public:
        std::string operator()(const std::map<std::string, T>& v) {
            YAML::Node node(YAML::NodeType::Map);
            for(auto& i : v) {
                node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    /**
     * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_map<std::string, T>)
     */
    template<class T>
    class LexicalCast<std::string, std::unordered_map<std::string, T> > {
    public:
        std::unordered_map<std::string, T> operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_map<std::string, T> vec;
            std::stringstream ss;
            for(auto it = node.begin();
                    it != node.end(); ++it) {
                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                            LexicalCast<std::string, T>()(ss.str())));
            }
            return vec;
        }
    };
    
    /**
     * @brief 类型转换模板类片特化(std::unordered_map<std::string, T> 转换成 YAML String)
     */
    template<class T>
    class LexicalCast<std::unordered_map<std::string, T>, std::string> {
        public:
            std::string operator()(const std::unordered_map<std::string, T>& v) {
                YAML::Node node(YAML::NodeType::Map);
                for(auto& i : v) {
                    node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
        };



    template<class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
    class ConfigVar : public ConfigVarBase{
        public:
            typedef std::shared_ptr<ConfigVar> ptr;
            typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;
            typedef RWMutex RWMutexType;


            ConfigVar(const std::string& name,
                      const T& default_value,
                      const std::string& des = "")
                    : ConfigVarBase(name, des),
                      m_val(default_value){  
            }

            std::string toString()  override {
                try {
                    //其它线程可以读但是不能写
                    RWMutexType::ReadLock lock(m_mutex);
                    return ToStr()(m_val);
                }  catch (std::exception& e) {
                    LOG_ERROR(GET_LOG_ROOT,  "ConfigVar::toString exception");
                    // LOG_ERROR(GET_LOG_ROOT(),  "ConfigVar::toString exception " + e.what().
                    //             + "convert: " + TypeToName<T>() + " to string" + " name=" + m_name);
                }

                return "";
            }
            
            //设置值的时候，触发监听函数
            bool fromString(const std::string& val) override {
                try {
                    setValue(FromStr()(val));
                } catch (std::exception& e) {
                    LOG_ERROR(GET_LOG_ROOT,  "ConfigVar::fromString exception ");
                    // LOG_ERROR(GET_LOG_ROOT(),  "ConfigVar::fromString exception " + e.what()
                    //             + "convert string to " + TypeToName<T>() + " name=" + m_name + " - " << val);
                }

                return false;
            }

            void setValue(const T& v) {
                {
                    RWMutexType::ReadLock lock(m_mutex);
                    if(m_val == v){
                        return;
                    }

                    for(auto &i : m_cbs){
                        i.second(m_val, v);
                    }
                }
                RWMutexType::WriteLock lock(m_mutex);
                m_val = v;
            }

            const T getValue() {
                //其它线程可能在写
                RWMutexType::ReadLock lock(m_mutex);
                return m_val;
            }

            std::string getTypeName() const override {return TypeToName<T>();}

            //全局变量来记录监听函数的id
            //
            //
            void addListener(on_change_cb cb){
                static uint64_t s_fun_id = 0;
                RWMutexType::WriteLock lock(m_mutex);
                ++s_fun_id;
                m_cbs[s_fun_id] = cb;
            }

            void delListener(uint64_t key){
                RWMutexType::WriteLock lock(m_mutex);
                m_cbs.erase(key);
            }

            on_change_cb getListener(uint64_t key){
                RWMutexType::ReadLock lock(m_mutex);
                auto it = m_cbs.find(key);

                return it == m_cbs.end() ? nullptr : it->second;
            }

            void clearListener() {
                RWMutexType::WriteLock lock(m_mutex);
                m_cbs.clear();
            }           
        private:
            RWMutexType m_mutex;
            T m_val;
            std::map<uint64_t, on_change_cb> m_cbs;

    };

    class Config{
        public:
            typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
            typedef RWMutex RWMutexType;

            template<class T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name,
                                                     const T& default_value,
                                                     const std::string& des = ""){
                
                //避免同时写全局的config
                RWMutexType::WriteLock lock(GetMutex());                                    
                auto it = GetDatas().find(name);
                if(it != GetDatas().end()){
                    auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);

                    if(tmp){
                        //参数名为name的配置参数存在,直接返回
                        LOG_INFO(GET_LOG_ROOT, "Lookup name=" + name + "exists");
                        return tmp;
                    } else {
                        //参数名为name的配置参数存在,但是类型不匹配
                        LOG_ERROR(GET_LOG_ROOT, "Lookup name=" + name + " exists but type not " + TypeToName<T>()  + " real_type=" + it->second->getTypeName()
                        + " " + it->second->toString());
                        return nullptr;
                    }
                }

                //参数名包含非法字符[^0-9a-z_.] 抛出异常 std::invalid_argument
                if(name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") 
                    != std::string::npos) {
                        LOG_ERROR(GET_LOG_ROOT, "Lookup name invalid" + name);
                        throw std::invalid_argument(name);
                }

                //参数不存在,创建参数并用default_value赋值,同时放入全局的配置参数图中
                typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, des));
                GetDatas()[name] = v;
                return v;
            }


            template<class T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name){
                
                RWMutexType::ReadLock lock(GetMutex());
                auto it = GetDatas().find(name);
                if(it != GetDatas().end()){
                    return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
                }
                return nullptr;
            }

            static ConfigVarBase::ptr LookUpBase(const std::string& name);
            static void LoadFromYaml(YAML::Node& root);
        
        private:
            static ConfigVarMap& GetDatas(){
                static ConfigVarMap s_datas;
                return s_datas;
            }
            
            static RWMutexType& GetMutex() {
                static RWMutexType s_mutex;
                return s_mutex;
            }         
    };
}
