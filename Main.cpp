#include <iostream>
#include <fstream>
#include "math_constants.h"
#include "color.h"
#include "hittable_list.h"
#include "light_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
using namespace std;

color ray_color(const ray& r, const hittable& world, const light& lights, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);


    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return (attenuation * lights.effect_on_point(rec, world)) + (0.5 * ray_color(scattered, world, lights, depth - 1));
    }

    return color(0.0, 0.0, 0.0);

}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.45, 0.45, 0.45));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    return world;
}

int main() {

    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    // World
    auto world = random_scene();
    
    auto material1 = make_shared<dielectric>(1.5);
    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    //center
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material2));
    //left
    world.add(make_shared<sphere>(point3(-3, 1, 0), 1.0, material3));
    //right
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material1));

    auto ground_material = make_shared<lambertian>(color(0.45, 0.45, 0.45));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    // Lights
    light_list lights;
    lights.add(make_shared<dir_light>(unit_vector(vec3(2,-1,-0.1)), 0.05, color(1.0, 1.0, 1.0)));
    lights.add(make_shared<point_light>(point3(-6.4, 1.0, 2.3), 40.0, color(1.0, 1.0, 1.0)));
    lights.add(make_shared<point_light>(point3(6.0, 1.0, 2.8), 40.0, color(1.0, 1.0, 1.0)));

    // Camera
    point3 lookfrom(-10, 2.2, 3.0);
    point3 lookat(2, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 7.5;
    auto aperture = 0.25;

    camera cam(lookfrom, lookat, vup, 30, aspect_ratio, aperture, dist_to_focus);

    // Render
    ofstream image_file;
    image_file.open("image.ppm");
    image_file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, lights, max_depth);
            }
            write_color(image_file, pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
    std::cerr << "Image output is at image.ppm";
}