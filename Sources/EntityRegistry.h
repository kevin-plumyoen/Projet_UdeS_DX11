#ifndef DEF_ECS_ENTITY_REGISTRY
#define DEF_ECS_ENTITY_REGISTRY

#include "Entity.h"
#include "StorageTraits.h"
#include <vector>
#include <deque>
#include <iterator>
#include <algorithm>
#include <set>

struct FailedToCreateEntity{};

class EntityRegistry;

class EntityRegistryIterator {
    friend EntityRegistry;
    const std::vector<bool>& alives;
    const std::vector<Entity::generation_type>& generations;
    Entity::index_type index;
    Entity internal_entity;

    EntityRegistryIterator(const std::vector<bool>& alives, const std::vector<Entity::generation_type>& generations, Entity::index_type index) noexcept;
    EntityRegistryIterator(const std::vector<bool>& alives, const std::vector<Entity::generation_type>& generations);

    void update_internal_entity();
public:
    using value_type = const Entity;
    using iterator_category = std::forward_iterator_tag;
    using pointer = const Entity*;
    using reference = const Entity&;
    using difference_type = void;

    bool operator==(const EntityRegistryIterator& other) const noexcept;
    bool operator!=(const EntityRegistryIterator& other) const noexcept;
    reference operator*() const noexcept;
    pointer operator->() const noexcept;
    EntityRegistryIterator& operator++();
    EntityRegistryIterator operator++(int);
};

struct EntityAlreadyLinked {};

class EntityRegistry {
    std::vector<Entity::generation_type> generations;
    std::vector<bool> alives;
    std::deque<Entity::index_type> destroyed_index;
	std::set<Entity> disabled;
	std::vector<Entity> parent_list;
	std::vector<std::vector<Entity>> childs_list;

    bool should_reuse_index() const noexcept;
    Entity reuse_index();
    Entity reuse_index(std::nothrow_t);
    Entity create_new_index();

public:
    using iterator = EntityRegistryIterator;

    void reserve(Entity::index_type size);

    Entity create();

    template<typename OutIt>
    OutIt create(std::size_t count, OutIt out) {
        for (std::size_t i = 0; i < count; ++i) {
            if constexpr(std::is_same_v<std::iterator_traits<OutIt>::iterator_category, std::output_iterator_tag>) {
                out = create();
            }
            else {
                *out = create();
            }
            ++out;
        }

        return out;
    }

    void destroy(Entity entity);

    template<typename InIt>
    void destroy(InIt begin, InIt end) {
        std::for_each(begin, end, [this](const Entity e) {
            destroy(e);
        });
    }

bool exists(Entity entity) const noexcept;

iterator begin() const noexcept;
iterator end() const noexcept;

// Here we take in consideration storages are synced with registry
template<typename OutIt, typename Storage>
OutIt entities_with(OutIt out, const Storage& storage) const {
    static_assert(is_storage_v<Storage>, "you must pass a valid component storage");

    return std::copy_if(storage.entities().begin(), storage.entities().end(), out, [this](const Entity& e) {return disabled.find(e) == disabled.end(); });
}

template<typename OutIt, typename Storage, typename... Storages>
OutIt entities_with(OutIt out, const Storage& storage, const Storages&... storages) const {
    static_assert((... && is_storage_v<Storages>), "You must pass valid component storages");

    std::vector<Entity> entities;
    entities.reserve(std::distance(storage.entities().begin(), storage.entities().end()));
    entities_with(std::back_inserter(entities), storage);

    std::vector<Entity> other_entities;
    entities_with(std::back_inserter(other_entities), storages...);

    return std::set_intersection(entities.begin(), entities.end(),
        other_entities.begin(), other_entities.end(),
        out);
}

void disable(const Entity&  e) {
    disabled.insert(e);
    std::for_each(childs_list[e.index()].begin(), childs_list[e.index()].end(), [this](const Entity& e) {disable(e); });
}
void enable(const Entity&  e) {
    if (disabled.find(e) != disabled.end()) {
        disabled.erase(e);
        std::for_each(childs_list[e.index()].begin(), childs_list[e.index()].end(), [this](const Entity& e) {enable(e); });
    }
}

void reset_link(Entity parent, Entity child) {
    // Si child est un enfant de parent
    if (parent_list[child.index()].valid()) {
        parent_list[child.index()] = Entity{};

        childs_list[parent.index()].erase(std::find(childs_list[parent.index()].begin(), childs_list[parent.index()].end(), child));
    }
}

void set_link(const Entity& parent, const Entity& child) {
    if (!parent_list[child.index()].valid()) {
        parent_list[child.index()] = parent;
        childs_list[parent.index()].push_back(child);
    }
    else {
        throw EntityAlreadyLinked{};
    }
}

void clear() {
    generations.clear();
    alives.clear();
    destroyed_index.clear();
    disabled.clear();
    parent_list.clear();
    childs_list.clear();
}

bool is_child(Entity e) const noexcept {
    if (e.valid() && e.index() < generations.size() && e.generation() == generations[e.index()]) {
        return parent_list[e.index()].valid();
    }

    return false;
}

Entity parent_of(Entity e) const noexcept {
    if (is_child(e)) {
        return parent_list[e.index()];
    }

    return Entity{};
}

bool has_children(Entity e) const noexcept {
    if (e.valid() && e.index() < generations.size() && e.generation() == generations[e.index()]) {
        return !childs_list[e.index()].empty();
    }

    return false;
}
	const Entity& getParent(const Entity& e) const{
		return parent_list[e.index()];
	}
	const std::vector<Entity>& getChilds(const Entity& e) const {
		return childs_list[e.index()];
	}

private:
    template<typename OutIt>
    OutIt all_children_impl(Entity e, OutIt out) const noexcept {
        out = e;
        ++out;


        for (Entity child : childs_list[e.index()]) {
            all_children_impl(child, out);
        }

        return out;
    }
public:
    template<typename OutIt>
    OutIt all_children(Entity e, OutIt out) const noexcept {
        if (e.valid() && e.index() < generations.size() && e.generation() == generations[e.index()]) {
            for (Entity child : childs_list[e.index()]) {
                out = all_children_impl(child, out);
            }
        }

        return out;
    }
};

#endif