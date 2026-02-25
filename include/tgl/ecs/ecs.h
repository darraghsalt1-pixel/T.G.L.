#ifndef TGL_ECS_ECS_H
#define TGL_ECS_ECS_H

#include "../core/types.h"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <algorithm>
#include <cassert>
#include <memory>

namespace tgl {

using Entity = u32;
constexpr Entity NULL_ENTITY = 0;

// Type-erased component storage
class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual void remove(Entity e) = 0;
    virtual bool has(Entity e) const = 0;
};

template<typename T>
class ComponentPool : public IComponentPool {
public:
    T& add(Entity e, const T& component = T{}) {
        assert(!has(e));
        entity_to_index_[e] = components_.size();
        index_to_entity_.push_back(e);
        components_.push_back(component);
        return components_.back();
    }

    void remove(Entity e) override {
        if (!has(e)) return;
        size_t idx = entity_to_index_[e];
        size_t last = components_.size() - 1;
        if (idx != last) {
            components_[idx] = std::move(components_[last]);
            Entity moved = index_to_entity_[last];
            entity_to_index_[moved] = idx;
            index_to_entity_[idx] = moved;
        }
        components_.pop_back();
        index_to_entity_.pop_back();
        entity_to_index_.erase(e);
    }

    bool has(Entity e) const override {
        return entity_to_index_.count(e) > 0;
    }

    T& get(Entity e) {
        return components_[entity_to_index_.at(e)];
    }

    const T& get(Entity e) const {
        return components_[entity_to_index_.at(e)];
    }

    // Iterate all components
    size_t size() const { return components_.size(); }
    T& at(size_t i) { return components_[i]; }
    Entity entity_at(size_t i) const { return index_to_entity_[i]; }

    // Range-based for support
    using iterator = typename std::vector<T>::iterator;
    iterator begin() { return components_.begin(); }
    iterator end() { return components_.end(); }

private:
    std::vector<T> components_;
    std::unordered_map<Entity, size_t> entity_to_index_;
    std::vector<Entity> index_to_entity_;
};

class World {
public:
    Entity create() {
        return next_entity_++;
    }

    void destroy(Entity e) {
        for (auto& [type, pool] : pools_) {
            pool->remove(e);
        }
    }

    template<typename T>
    T& add(Entity e, const T& component = T{}) {
        return get_or_create_pool<T>()->add(e, component);
    }

    template<typename T>
    void remove(Entity e) {
        auto pool = get_pool<T>();
        if (pool) pool->remove(e);
    }

    template<typename T>
    bool has(Entity e) const {
        auto pool = get_pool<T>();
        return pool && pool->has(e);
    }

    template<typename T>
    T& get(Entity e) {
        return get_pool<T>()->get(e);
    }

    template<typename T>
    const T& get(Entity e) const {
        return get_pool<T>()->get(e);
    }

    // Iterate all entities with component T
    template<typename T>
    void each(std::function<void(Entity, T&)> fn) {
        auto pool = get_pool<T>();
        if (!pool) return;
        for (size_t i = 0; i < pool->size(); i++) {
            fn(pool->entity_at(i), pool->at(i));
        }
    }

    // Iterate entities that have both T1 and T2
    template<typename T1, typename T2>
    void each(std::function<void(Entity, T1&, T2&)> fn) {
        auto pool1 = get_pool<T1>();
        auto pool2 = get_pool<T2>();
        if (!pool1 || !pool2) return;
        // Iterate the smaller pool
        auto* smaller = pool1;
        for (size_t i = 0; i < smaller->size(); i++) {
            Entity e = smaller->entity_at(i);
            if (pool2->has(e)) {
                fn(e, pool1->get(e), pool2->get(e));
            }
        }
    }

    // Iterate entities that have T1, T2, and T3
    template<typename T1, typename T2, typename T3>
    void each(std::function<void(Entity, T1&, T2&, T3&)> fn) {
        auto pool1 = get_pool<T1>();
        auto pool2 = get_pool<T2>();
        auto pool3 = get_pool<T3>();
        if (!pool1 || !pool2 || !pool3) return;
        for (size_t i = 0; i < pool1->size(); i++) {
            Entity e = pool1->entity_at(i);
            if (pool2->has(e) && pool3->has(e)) {
                fn(e, pool1->get(e), pool2->get(e), pool3->get(e));
            }
        }
    }

    template<typename T>
    ComponentPool<T>* get_pool() {
        auto it = pools_.find(std::type_index(typeid(T)));
        if (it == pools_.end()) return nullptr;
        return static_cast<ComponentPool<T>*>(it->second.get());
    }

    template<typename T>
    const ComponentPool<T>* get_pool() const {
        auto it = pools_.find(std::type_index(typeid(T)));
        if (it == pools_.end()) return nullptr;
        return static_cast<const ComponentPool<T>*>(it->second.get());
    }

private:
    template<typename T>
    ComponentPool<T>* get_or_create_pool() {
        auto key = std::type_index(typeid(T));
        auto it = pools_.find(key);
        if (it != pools_.end()) return static_cast<ComponentPool<T>*>(it->second.get());
        auto pool = std::make_unique<ComponentPool<T>>();
        auto* ptr = pool.get();
        pools_[key] = std::move(pool);
        return ptr;
    }

    Entity next_entity_ = 1;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools_;
};

} // namespace tgl

#endif // TGL_ECS_ECS_H
