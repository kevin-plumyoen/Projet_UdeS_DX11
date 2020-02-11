#ifndef DEF_ECS_VECTOR_STORAGE_H
#define DEF_ECS_VECTOR_STORAGE_H

#include "Entity.h"
#include "StorageTraits.h"
#include <vector>
#include <utility>
#include <iterator>
#include <cassert>

// A storage must store components
// A storage must associate a component with an entity

struct EntityIndexAlreadyInUse{};

template<typename ComponentType>
class VectorStorage {
public:
    using component_type = ComponentType;

#pragma region Iterators
    class Iterator {
        friend VectorStorage;
    public:
        using value_type = std::pair<Entity, component_type>;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::size_t;
        using pointer = value_type*;
        using reference = value_type&;

    private:
        using component_pair = std::pair<Entity, component_type>;
        const std::vector<bool>& alives;
        std::vector<component_pair>& values;
        std::size_t index;

        Iterator(const std::vector<bool>& alives, std::vector<component_pair>& values) noexcept
        : alives{ alives }
        , values{ values }
        , index{ values.size() } {

        }

        Iterator(const std::vector<bool>& alives, std::vector<component_pair>& values, std::size_t idx) noexcept
        : Iterator{ alives, values } {
            index = idx;
            while (index < alives.size() && !alives[index]) {
                ++index;   
            }
        }
    public:
        bool operator==(const Iterator& other) const noexcept {
            return index == other.index;
        }

        bool operator!=(const Iterator& other) const noexcept {
            return index != other.index;
        }

        Iterator & operator++() {
            do {
                ++index;
            } while(index < values.size() && !alives[index]);

            return *this;
        }

        Iterator operator++(int) {
            Iterator previous = *this;

            ++(*this);

            return previous;
        }

        reference operator*() noexcept {
            return values[index];
        }

        pointer operator->() noexcept {
            return &values[index];
        }
    };

    class ConstIterator {
        friend VectorStorage;
    public:
        using value_type = std::pair<Entity, component_type>;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::size_t;
        using pointer = const value_type*;
        using reference = const value_type&;

    private:
        using component_pair = std::pair<Entity, component_type>;
        const std::vector<bool>& alives;
        const std::vector<component_pair>& values;
        std::size_t index;

        ConstIterator(const std::vector<bool>& alives, const std::vector<component_pair>& values)
        : alives{ alives }
        , values{ values }
        , index{ values.size() } {

        }

        ConstIterator(const std::vector<bool>& alives, const std::vector<component_pair>& values, std::size_t idx)
        : ConstIterator{ alives, values } {
            index = idx;
            while (index < alives.size() && !alives[index]) {
                ++index;   
            }
        }

    public:
        bool operator==(const ConstIterator& other) const noexcept {
            return index == other.index;
        }

        bool operator!=(const ConstIterator& other) const noexcept {
            return index != other.index;
        }

        ConstIterator & operator++() {
            do {
                ++index;
            } while(index < values.size() && !alives[index]);

            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator previous = *this;

            ++(*this);

            return previous;
        }

        reference operator*() const noexcept {
            return values[index];
        }

        pointer operator->() const noexcept {
            return &values[index];
        }
    };
#pragma endregion Iterators

#pragma region EntityIterators
class EntityIterator {
    friend VectorStorage;
public:
    using value_type = Entity;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::size_t;
    using pointer = const value_type*;
    using reference = const value_type&;
private:
    using component_pair = std::pair<Entity, component_type>;
    const std::vector<bool>& alives;
    const std::vector<component_pair>& values;
    std::size_t index;

    EntityIterator(const std::vector<bool>& alives, const std::vector<component_pair>& values)
    : alives{ alives }
    , values{ values }
    , index{ values.size() } {

    }

    EntityIterator(const std::vector<bool>& alives, const std::vector<component_pair>& values, std::size_t idx)
    : EntityIterator{ alives, values } {
        index = idx;
        while (index < alives.size() && !alives[index]) {
            ++index;   
        }
    }

public:
    bool operator==(const EntityIterator& other) const noexcept {
        return index == other.index;
    }

    bool operator!=(const EntityIterator& other) const noexcept {
        return index != other.index;
    }

    EntityIterator & operator++() {
        do {
            ++index;
        } while(index < values.size() && !alives[index]);

        return *this;
    }

    EntityIterator operator++(int) {
        EntityIterator previous = *this;

        ++(*this);

        return previous;
    }

    reference operator*() noexcept {
        return values[index].first;
    }

    pointer operator->() noexcept {
        return &values[index].first;
    }
};
#pragma endregion EntityIterators

class Entities {
    friend VectorStorage;
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

private:
    std::vector<bool> alives;
    std::vector<std::pair<Entity, component_type>> components;

    void prepare_space_for(Entity entity) {
        const std::size_t new_size = entity.index() + 1;

        if (new_size >= alives.size()) {
            alives.resize(new_size, false);
            components.resize(new_size);
        }
    }

public:
    using iterator = Iterator;
    using const_iterator = ConstIterator;

    std::pair<iterator, bool> attach(Entity entity, const component_type& component) {
        prepare_space_for(entity);

        // Entity's index already attached but not the same entity
        if (alives[entity.index()] && components[entity.index()].first != entity) {
            throw EntityIndexAlreadyInUse{};
        }

        const bool value_inserted = !alives[entity.index()];
        if(value_inserted) {
            alives[entity.index()] = true;
            components[entity.index()] = std::make_pair(entity, component);
        }

        return std::make_pair(iterator(alives, components, entity.index()), 
                              value_inserted);
    }

    std::pair<iterator, bool> attach(Entity entity, component_type&& component) {
        prepare_space_for(entity);

        // Entity's index already attached but not the same entity
        if (alives[entity.index()] && components[entity.index()].first != entity) {
            throw EntityIndexAlreadyInUse{};
        }

        const bool value_inserted = !alives[entity.index()];
        if(value_inserted) {
            alives[entity.index()] = true;
            components[entity.index()] = std::make_pair(entity, std::move(component));
        }

        return std::make_pair(iterator(alives, components, entity.index()), 
                              value_inserted);
    }

    void detach(Entity entity) {
        if (entity.index() < alives.size() && alives[entity.index()] && components[entity.index()].first == entity) {
            alives[entity.index()] = false;
        }
    }

    bool is_attached(Entity entity) const noexcept {
        if (entity.index() < components.size() && alives[entity.index()]) {
            return components[entity.index()].first == entity;
        }

        return false;
    }

    // Iterators on elements
    iterator begin() noexcept { return iterator{alives, components, 0}; }
    iterator end() noexcept { return iterator{alives, components}; }
    const_iterator begin() const noexcept { return const_iterator{alives, components, 0}; }
    const_iterator end() const noexcept { return const_iterator{alives, components}; }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    iterator find(Entity entity) noexcept {
        if (entity.index() < alives.size()
            && alives[entity.index()]
            && components[entity.index()].first == entity) {
            return iterator{alives, components, entity.index()};
        }

        return end();
    }

    const_iterator find(Entity entity) const noexcept {
        if (entity.index() < alives.size()
            && alives[entity.index()]
            && components[entity.index()].first == entity) {
            return const_iterator{alives, components, entity.index()};
        }

        return end();
    }

    // Iterators on entities
    const Entities entities() const noexcept {
        return Entities{EntityIterator{alives, components, 0}, EntityIterator{alives, components}};
    }

    void clear() {
        alives.clear();
        components.clear();
    }
};

template<typename ComponentType>
struct is_storage<VectorStorage<ComponentType>> {
    static constexpr const bool value = true;
};

#endif