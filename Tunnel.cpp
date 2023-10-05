#include "Tunnel.h"

Tunnel::Tunnel(std::string object_file, uint width, uint height, float dx, uint threads, uint gs_iters, float speed) : Fluid(width, height, dx) {

    Fluid(width, height, dx);

    threads = threads;
    gs_iters = gs_iters;

    this -> speed = speed;

    for(uint i = 0; i < width; i++){
        for(uint j = 0; j < height; j++){
            particles[coords2index(i, j, width)].vx = speed;
        }
    }

    object.loadFromFile(object_file);

    set_boundaries = tunnel_boundaries;

}

Tunnel::~Tunnel(){
}

void Tunnel::physics(float delta){

    Fluid::physics(delta);

}



void Tunnel::draw_object(sf::RenderWindow& window, uint block_size){

    sf::RectangleShape rect(sf::Vector2f(block_size, block_size));


    for(uint i = 0; i < width; i++){

        for(uint j = 0; j < height; j++){

            rect.setPosition(i * block_size, j * block_size);
            rect.setFillColor(object.getPixel(i, j));
            window.draw(rect);
        }

    }

}

void tunnel_boundaries(Particle* particles, uint width, uint height, uint identifier){
    float speed = 50;
    sf::Image object;
    object.loadFromFile("objects/sphere.png");
    
    //top and bottom
    for(uint i = 0; i < width; i++){
        if(identifier == 1){
            particles[coords2index(i, 0, width)].vx = speed;
            particles[coords2index(i, height - 1, width)].vx = speed;
        }
        
        if(identifier == 2){
            particles[coords2index(i, 0, width)].vy = particles[coords2index(i, 1, width)].vy;
            particles[coords2index(i, height - 1, width)].vy = particles[coords2index(i, height - 2, width)].vy;
        }
        
        if(identifier == 3){
            particles[coords2index(i, 0, width)].div = particles[coords2index(i, 1, width)].div;
            particles[coords2index(i, height - 1, width)].div = particles[coords2index(i, height - 2, width)].div;
        }
        
        if(identifier == 4){
            particles[coords2index(i, 0, width)].p = particles[coords2index(i, 1, width)].p;
            particles[coords2index(i, height - 1, width)].p = particles[coords2index(i, height - 2, width)].p;        
        }
        
        if(identifier == 5){
            particles[coords2index(i, 0, width)].smoke = particles[coords2index(i, 1, width)].smoke;
            particles[coords2index(i, height - 1, width)].smoke = particles[coords2index(i, height - 2, width)].smoke;
        }
    }

    //left and right
    for(uint j = 1; j < height - 1; j++){
        if(identifier == 1){
            particles[coords2index(0, j, width)].vx = speed;
            particles[coords2index(width - 1, j, width)].vx = particles[coords2index(width - 2, j, width)].vx;
        }

        if(identifier == 2){
            particles[coords2index(0, j, width)].vy = particles[coords2index(1, j, width)].vy;
            particles[coords2index(width - 1, j, width)].vy = particles[coords2index(width - 2, j, width)].vy;
        }

        if(identifier == 3){
            particles[coords2index(0, j, width)].div = particles[coords2index(1, j, width)].div;
            particles[coords2index(width - 1, j, width)].div = particles[coords2index(width - 2, j, width)].div;
        }

        if(identifier == 4){
            particles[coords2index(0, j, width)].p = particles[coords2index(1, j, width)].p;
            particles[coords2index(width - 1, j, width)].p = particles[coords2index(width - 2, j, width)].p;
        }

        if(identifier == 5){
            particles[coords2index(0, j, width)].smoke = particles[coords2index(1, j, width)].smoke;
            particles[coords2index(width - 1, j, width)].smoke = particles[coords2index(width - 2, j, width)].smoke;
        }
    }

    uint smoke_start = (uint) (0.40 * height);
    uint smoke_end = (uint) (0.60 * height);

    if(identifier == 5){
        for(uint j = smoke_start; j < smoke_end; j++){
            particles[coords2index(0, j, width)].smoke = 0.5;
        }        
    }


    for(uint i = 1; i < width - 1; i++){
        for(uint j = 1; j < height - 1; j++){
            Particle& p = particles[coords2index(i, j, width)];
            const sf::Color& c = object.getPixel(i, j);

            if(c.a == 255){

                p.vx = 0;
                p.vy = 0;

                const sf::Color& c1 = object.getPixel(i - 1, j);
                const sf::Color& c2 = object.getPixel(i + 1, j);
                const sf::Color& c3 = object.getPixel(i, j - 1);
                const sf::Color& c4 = object.getPixel(i, j + 1);

                Particle& p1 = particles[coords2index(i - 1, j, width)];
                Particle& p2 = particles[coords2index(i + 1, j, width)];
                Particle& p3 = particles[coords2index(i, j - 1, width)];
                Particle& p4 = particles[coords2index(i, j + 1, width)];

                if(c1.a != 255){
                    if(identifier == 1)
                        p.vx += -p1.vx;
                    if(identifier == 2)
                        p.vy += 0;

                    if(identifier == 4)
                        p.p = p1.p;

                    if(identifier == 3)
                        p.div = p1.div;
                }

                if(c2.a != 255){
                    if(identifier == 1)
                        p.vx += -p2.vx;

                    if(identifier == 2)
                        p.vy += 0;

                    if(identifier == 4)
                        p.p = p2.p;

                    if(identifier == 5)
                        p.div = p2.div;
                }

                if(c3.a != 255){
                    if(identifier == 1)
                        p.vx += 0;

                    if (identifier == 2)
                        p.vy += -p3.vy;

                    if(identifier == 4)
                        p.p = p3.p;

                    if(identifier == 5)
                        p.div = p3.div;
                }

                if(c4.a != 255){
                    if(identifier == 1)
                        p.vx += 0;

                    if(identifier == 2)
                        p.vy += -p4.vy;
                
                    if(identifier == 4)
                        p.p = p4.p;
                    
                    if(identifier == 3)
                        p.div = p4.div;
                }

            }
        }

    }

}