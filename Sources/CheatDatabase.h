#ifndef DEF_CHEAT_DATABASE_H
#define DEF_CHEAT_DATABASE_H

struct CheatDatabase {
private:
    CheatDatabase() = default;
public:
    static CheatDatabase& get() noexcept {
        static CheatDatabase instance;

        return instance;
    }

    bool disable_culling = false;
};

#endif