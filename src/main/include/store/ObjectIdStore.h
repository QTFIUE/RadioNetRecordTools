#pragma once
#ifndef _OBJECTIDSTORE_H__
#define _OBJECTIDSTORE_H__

#define OBJECT_ID_STORE (store::ObjectIdStore::getinstance())
#define REGISTER_OBJECT(object_id, object) (OBJECT_ID_STORE->registerObject(object_id, object))
#define GET_REGISTERED_OBJECT(object_id, T) (OBJECT_ID_STORE->getObject<T>(object_id))

#include <map>
#include <QObject>
#include <string>

namespace store {

class ObjectIdStore {
    using ObjectMap = std::map<std::string, QObject*>;
public:
    static ObjectIdStore* getinstance()  // 对外的接口（方法），静态成员函数调用静态成员变量
    {
        return instance;
    };

    /**
     * @brief 注册对象
     * 
     * @param object_id 
     * @param object 
     */
    void registerObject(std::string object_id, QObject* object);

    /**
     * @brief 获取对象
     * 
     * @tparam T 
     * @param object_id 
     * @return T* 
     */
    template<typename T>
    T* getObject(std::string object_id) {
        if (this->object_id_map.find(object_id) == this->object_id_map.end()) {
            return nullptr;
        }
        return dynamic_cast<T*>(this->object_id_map[object_id]);
    }

private:
    static ObjectIdStore* instance;

    ObjectMap object_id_map;

    ObjectIdStore() {} //构造函数属性设置为私有
 
    ObjectIdStore(const ObjectIdStore & sc) {} //拷贝构造函数也需要设置为私有
};

}  // namespace store

#endif  //  _OBJECTIDSTORE_H__
