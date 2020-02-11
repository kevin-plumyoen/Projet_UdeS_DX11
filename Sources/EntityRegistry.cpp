#include "EntityRegistry.h"

#pragma warning(push, 0)
#include <cassert>
#include <algorithm>
#pragma warning(pop)

void EntityRegistryIterator::update_internal_entity() {
    if (index < generations.size()) {
        internal_entity = Entity(index, generations[index]);
    }
}

EntityRegistryIterator::EntityRegistryIterator(const std::vector<bool>& alives, const std::vector<Entity::generation_type>& generations)
: alives{alives}
, generations{generations}
, index{static_cast<Entity::index_type>(generations.size())}
, internal_entity{} {

}

EntityRegistryIterator::EntityRegistryIterator(const std::vector<bool>& alives, const std::vector<Entity::generation_type>& generations, Entity::index_type) noexcept
: EntityRegistryIterator{alives, generations} {
    assert(generations.size() == alives.size());
    const auto it = std::find(alives.begin(), alives.end(), true);

    if(it != alives.end()) {
        this->index = static_cast<Entity::index_type>(std::distance(alives.begin(), it));
        update_internal_entity();
    }
}

bool EntityRegistryIterator::operator==(const EntityRegistryIterator& other) const noexcept {
    return index == other.index;
}

bool EntityRegistryIterator::operator!=(const EntityRegistryIterator& other) const noexcept {
    return index != other.index;
}

EntityRegistryIterator::reference EntityRegistryIterator::operator*() const noexcept {
    return internal_entity;
}

EntityRegistryIterator::pointer EntityRegistryIterator::operator->() const noexcept {
    return &internal_entity;
}

EntityRegistryIterator& EntityRegistryIterator::operator++() {
    do {
        ++index;
    } while(index < alives.size() && !alives[index]);

    update_internal_entity();

    return *this;
}

EntityRegistryIterator EntityRegistryIterator::operator++(int) {
    EntityRegistryIterator prev = *this;

    ++(*this);

    return prev;
}
bool EntityRegistry::should_reuse_index() const noexcept {
    // C'est impossible d'agrandir le vecteur de génération, on doit donc réutiliser
    if (generations.size() > std::numeric_limits<Entity::index_type>::max()) {
        return true;
    }

    // On ne réutilise que si au moins 512 entités ont été détruits
    // cela aide à ne pas augmenter trop rapidement la réutilisation d'indices d'objets à courtes durée de vie
    return destroyed_index.size() > 512;
}

Entity EntityRegistry::reuse_index() {
    if(destroyed_index.empty()) throw FailedToCreateEntity{};

    const Entity::index_type index = destroyed_index.front();
    destroyed_index.pop_front();
    alives[index] = true;
	parent_list[index] = Entity();
	childs_list[index].clear();

    return Entity{index, generations[index]};
}

Entity EntityRegistry::reuse_index(std::nothrow_t) {
    if(destroyed_index.empty()) return Entity{};

    const Entity::index_type index = destroyed_index.front();
    destroyed_index.pop_front();
    alives[index] = true;
	parent_list[index] = Entity();
	childs_list[index].clear();

    return Entity{index, generations[index]};
}

Entity EntityRegistry::create_new_index() {
    const Entity::index_type index = static_cast<Entity::index_type>(generations.size());
    generations.push_back(0);
    alives.push_back(true);
	parent_list.emplace_back();
	childs_list.emplace_back();

    return Entity(index, 0);
}

void EntityRegistry::reserve(Entity::index_type size) {
    generations.reserve(size);
    alives.reserve(size);
}

Entity EntityRegistry::create() {
    if (should_reuse_index()) {
        return reuse_index();
    }

    return create_new_index();
}

void EntityRegistry::destroy(Entity entity) {
    if (exists(entity)) {
        ++generations[entity.index()];
        destroyed_index.push_back(entity.index());
        alives[entity.index()] = false;
    }
}

bool EntityRegistry::exists(Entity entity) const noexcept {
    assert(entity.index() < generations.size());
    
    return generations[entity.index()] == entity.generation();
}

EntityRegistry::iterator EntityRegistry::begin() const noexcept {
    return iterator{alives, generations, 0};
}

EntityRegistry::iterator EntityRegistry::end() const noexcept {
    return iterator{alives, generations};
}