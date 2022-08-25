#ifndef LIGHT_H
#define LIGHT_H

#include "hittable.h"
#include "vec3.h"


class light {
public:
    virtual color effect_on_point(hit_record& rec, const hittable& world) const = 0;
};


class dir_light : public light {
public:
    dir_light() {}
    dir_light(vec3 dir, double b, color c)
        : direction(dir), brightness(b), light_color(c) {};

    virtual color effect_on_point(hit_record& rec, const hittable& world) const override;

public:
    vec3 direction;
    double brightness;
    color light_color;
};

color dir_light::effect_on_point(hit_record& rec, const hittable& world) const {
    vec3 dir_to_light = unit_vector(-direction);
    ray ray_to_light = ray(rec.p, dir_to_light);

    hit_record between_light_rec;
    double light_power;
    if (world.hit(ray_to_light, 0.001, infinity, between_light_rec)) {
        light_power = 0.0;
    }
    else {
        light_power = dot(rec.normal, dir_to_light) * brightness;
    }
    return light_power * light_color;
}

class point_light : public light {
public:
    point_light() {}
    point_light(point3 pos, double b, color c)
        : position(pos), brightness(b), light_color(c) {};

    virtual color effect_on_point(hit_record& rec, const hittable& world) const override;

public:
    point3 position;
    double brightness;
    color light_color;
};

color point_light::effect_on_point(hit_record& rec, const hittable& world) const {
    vec3 dir_to_light = position - rec.p;
    double distance_to_light = dir_to_light.length();
    double light_intensity = brightness / (12.0 * 3.14 * distance_to_light*distance_to_light*distance_to_light);
    
    
    ray ray_to_light = ray(rec.p, dir_to_light);
    hit_record between_light_rec;
    if (world.hit(ray_to_light, 0.001, infinity, between_light_rec)) {
        double distance_to_rec = (between_light_rec.p - rec.p).length();
        if (distance_to_rec < distance_to_light) {
            light_intensity = 0;
        }
    }

    return light_intensity * light_color;
}

#endif