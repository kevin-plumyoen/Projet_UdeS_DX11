#include "OptionDatabase.h"

OptionsDatabase& OptionsDatabase::get() {
    static OptionsDatabase instance;

    return instance;
}