#ifndef DEF_OPTIONS_DATABASE_H
#define DEF_OPTIONS_DATABASE_H

struct OptionsDatabase {
    bool enable_bloom = true;
    bool enable_radial_blur = true;
private:
    OptionsDatabase() = default;

public:
    static OptionsDatabase& get();
};

#endif