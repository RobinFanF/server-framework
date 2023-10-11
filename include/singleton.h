#pragma once
#include <memory>

namespace frb{
    template<class T>
    class Singleton{
        public:
            static T* GetInstance(){
                static T v;
                return &v;
            }
    };

    template<class T>
    class SingletonPtr{
        public:
            static std::shared_ptr<T> GetInstance(){
                std::shared_ptr<T> v = std::make_shared<T>();
                return v;
            }

    };

    
}