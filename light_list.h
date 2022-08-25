#ifndef LIGHT_LIST_H
#define LIGHT_LIST_H

#include "light.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class light_list : public light {
public:
    light_list() {}
    light_list(shared_ptr<light> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<light> object) { objects.push_back(object); }

    virtual color effect_on_point(hit_record& rec, const hittable& world) const override;

public:
    std::vector<shared_ptr<light>> objects;
};

color light_list::effect_on_point(hit_record& rec, const hittable& world) const {
    color final_color = color(0, 0, 0);
    for (const auto& object : objects) {
        final_color += object->effect_on_point(rec, world);
    }

    return final_color;
}

#endif