#include "store/ObjectIdStore.h"
#include <stdexcept>

namespace store {
ObjectIdStore* ObjectIdStore::instance = new ObjectIdStore();

void ObjectIdStore::registerObject(std::string object_id, QObject* object) {
    if (this->object_id_map.find(object_id) != this->object_id_map.end()) {
        throw std::runtime_error("object_id: '" + object_id + "' already exists");
    }

    this->object_id_map.insert(std::make_pair(object_id, object));
}

}
