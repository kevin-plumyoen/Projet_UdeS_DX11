#include "LibraryLoader.h"

#include <Windows.h>

struct LibraryLoader::Impl {
    HMODULE hMod;

    Impl(std::wstring_view path)
    : hMod{LoadLibrary(path.data())} {
        if(!hMod) throw FailedToLoadLibrary{};
    }

    ~Impl() {
        FreeLibrary(hMod);
    }

    void* symbol(std::string_view name) {
        return GetProcAddress(hMod, name.data());
    }
};

LibraryLoader::LibraryLoader() noexcept 
: pimpl(nullptr) {

}

LibraryLoader::LibraryLoader(std::wstring_view path)
: pimpl(std::make_unique<Impl>(path)){

}

LibraryLoader::~LibraryLoader() = default;

LibraryLoader::LibraryLoader(LibraryLoader&& other) noexcept
: pimpl(std::move(other.pimpl)) {

}

LibraryLoader& LibraryLoader::operator=(LibraryLoader&& other) noexcept {
    swap(other);

    return *this;
}

void LibraryLoader::reset() {
    pimpl.reset();
}

void LibraryLoader::swap(LibraryLoader& other) {
    using std::swap;
    swap(pimpl, other.pimpl);
}

void* LibraryLoader::raw_symbol(std::string_view name) {
    void* data = pimpl->symbol(name);

    if (data == NULL) {
        throw SymbolNotFound{};
    }

    return data;
}