#ifndef DEF_LIBRARY_LOADER_H
#define DEF_LIBRARY_LOADER_H

#include <string_view>
#include <memory>

struct SymbolNotFound {};
struct FailedToLoadLibrary {};

class LibraryLoader {
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    void* raw_symbol(std::string_view name);
public:
    LibraryLoader() noexcept;
    LibraryLoader(std::wstring_view path);
    ~LibraryLoader();

    LibraryLoader(LibraryLoader&& other) noexcept;
    LibraryLoader& operator=(LibraryLoader&& other) noexcept;

    void reset();
    void swap(LibraryLoader& other);

    template<typename T>
    T symbol(std::string_view name) {
        return static_cast<T>(raw_symbol(name));
    }
};

#endif