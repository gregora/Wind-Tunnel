#if !defined _TUNNEL_H
#define TUNNEL_H 1

#include "Fluid.h"
#include <thread>
#include <cstdint>

class Tunnel : public Fluid {

    public:

        float speed = 5;
        
        uint8_t* object_mask = NULL;
        uint8_t* object_mask_CUDA = NULL;

        Tunnel(std::string object_file, uint width, uint height, float dx = 1, float scale = 1, uint threads = 1, uint gs_iters = 20, float speed = 50, float angle = 0.0);
        ~Tunnel();

        void draw_object(sf::RenderTarget& window, float block_size = 20);

        void set_boundaries_sector(Particle* particles, uint start, uint end, uint identifier);
        void set_boundaries(Particle* particles, uint width, uint height, uint identifier);

        float calculate_lift();
        float calculate_drag();

    private:
        sf::Uint8* object_pixels;


};

void tunnel_boundaries(Particle* particles, uint width, uint height, uint identifier);

#endif