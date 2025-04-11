#ifndef TEST_VALUEBINDER_H
#define TEST_VALUEBINDER_H

#include <QObject>
#include <QVector>
#include <functional>

namespace binder {

template<typename T>
class ValueBinder {
public:
    ValueBinder() = default;

    inline T getValue() const {
        return value;
    }

    inline void setValue(T v) {
        this->value = v;
        notify();
    }

    inline void bindCallback(std::function<void(T)> callback) {
        callbacks.push_back(callback);
    }

    inline T& operator=(const T& v) {
        setValue(v);
        notify();
        return value;
    }

private:

    T value;

    QVector<std::function<void(T)>> callbacks;

    /**
     * 提示
     */
    void notify() {
        for (auto &callback : callbacks) {
            callback(value);
        }
    }
};

}

#endif  // TEST_VALUEBINDER_H
