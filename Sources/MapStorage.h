#ifndef DEF_MAP_STORAGE_H
#define DEF_MAP_STORAGE_H

#include "Entity.h"
#include <map>

template<typename ComponentType>
class MapStorage {
public:
    using component_type = ComponentType;
private:
    std::map<Entity, component_type> components;
public:
    using iterator = typename std::map<Entity, component_type>::iterator;
    using const_iterator = typename std::map<Entity, component_type>::const_iterator;

    class EntityIterator {
        friend MapStorage;
        using internal_iterator = MapStorage::const_iterator;
        internal_iterator it;

        EntityIterator(internal_iterator it) noexcept : it{it} {}
    public:
        using value_type = Entity;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::size_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        bool operator==(const EntityIterator& other) const noexcept {
            return it == other.it;
        }

        bool operator!=(const EntityIterator& other) const noexcept {
            return it != other.it;
        }

        EntityIterator & operator++() {
            ++it;
            return *this;
        }

        EntityIterator operator++(int) {
            EntityIterator previous = *this;

            ++(*this);

            return previous;
        }

        reference operator*() noexcept {
            return it->first;
        }

        pointer operator->() noexcept {
            return &it->first;
        }
    };

    class Entities {
        friend MapStorage;
    public:
        using iterator = EntityIterator;
    private:
        iterator begin_, end_;

        Entities(iterator begin, iterator end)
        : begin_{ begin }, end_{ end } {

        }
    public:
        iterator begin() const noexcept { return begin_; }
        iterator end() const noexcept { return end_; }
    };

    std::pair<const iterator, bool> attach(Entity entity, const component_type& component) {
        // TODO: Validate entity is not already there
        return components.insert(std::make_pair(entity, component));
    }

    std::pair<const iterator, bool> attach(Entity entity, component_type&& component) {
        // TODO: Validate entity is not already there
        return components.insert(std::make_pair(entity, std::move(component)));
    }

    void detach(Entity entity) {
        components.remove(entity);
    }

    bool is_attached(Entity entity) const noexcept {
        auto element = components.find(entity);

        return element != components.end();
    }

    iterator find(Entity entity) noexcept {
        return components.find(entity);
    }

    const_iterator find(Entity entity) const noexcept {
        return components.find(entity);
    }

    // Iterators on elements
    iterator begin() noexcept { return components.begin(); }
    iterator end() noexcept { return components.end(); }
    const_iterator begin() const noexcept { return components.begin(); }
    const_iterator end() const noexcept { return components.end(); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    // Iterators on entities
    const Entities entities() const noexcept {
        return Entities(EntityIterator{begin()}, EntityIterator{end()});
    }

    void clear() {
        components.clear();
    }
};

template<typename ComponentType>
struct is_storage<MapStorage<ComponentType>> {
    static constexpr const bool value = true;
};


#endif