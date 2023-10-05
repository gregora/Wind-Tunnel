#if !defined _TUNNEL_H
#define TUNNEL_H 1

#include "Fluid.h"


class Tunnel : public Fluid {

    public:

        float speed = 50;
        sf::Image object;

        Tunnel(std::string object_file, uint width, uint height, float dx = 1, uint threads = 1, uint gs_iters = 20, float speed = 50);
        ~Tunnel();

        void physics(float delta);

        void draw_object(sf::RenderWindow& window, uint block_size = 20);

};

void tunnel_boundaries(Particle* particles, uint width, uint height, uint identifier);

#endif