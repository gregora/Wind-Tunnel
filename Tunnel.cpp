#include "Tunnel.h"

Tunnel::Tunnel(std::string object_file, uint width, uint height, float dx, uint threads, uint gs_iters, float speed) : Fluid(width, height, dx) {

    threads = threads;
    gs_iters = gs_iters;

    this -> speed = speed;

    object.loadFromFile(object_file);

    float scalex = (float) width / object.getSize().x;
    float scaley = (float) height / object.getSize().y;

    sf::Image scaled_object;
    scaled_object.create(width, height);

    for(uint i = 0; i < width; i++){
        for(uint j = 0; j < height; j++){
            scaled_object.setPixel(i, j, object.getPixel(i / scalex, j / scaley));
        }
    }

    object = scaled_object;

}

Tunnel::~Tunnel(){
}



void Tunnel::draw_object(sf::RenderWindow& window, uint block_size){

    sf::RectangleShape rect(sf::Vector2f(block_size, block_size));

    sf::Color c(9, 31, 0);

    for(uint i = 0; i < width; i++){

        for(uint j = 0; j < height; j++){

            rect.setPosition(i * block_size, j * block_size);
            c.a = object.getPixel(i, j).a;
            rect.setFillColor(c);
            window.draw(rect);
        }

    }

}

void Tunnel::set_boundaries(Particle* particles, uint width, uint height, uint identifier){  

    //external forces (force of gravity)

    for(uint i = 0; i < width; i++){
        for(uint j = 0; j < height; j++){
            //particles[coords2index(i, j, width)].Fy = 10;
        }
    }



    //top and bottom
    for(uint i = 0; i < width; i++){
        if(identifier == 1){
            particles[coords2index(i, 0, width)].vx = speed;
            particles[coords2index(i, height - 1, width)].vx = speed;
        }
        
        if(identifier == 2){
            particles[coords2index(i, 0, width)].vy = -particles[coords2index(i, 1, width)].vy;
            particles[coords2index(i, height - 1, width)].vy = -particles[coords2index(i, height - 2, width)].vy;
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
            particles[coords2index(0, j, width)].smoke = 0;
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
                if(identifier == 1)
                    p.vx = 0;
                if(identifier == 2)
                    p.vy = 0;
                if(identifier == 3)
                    p.div = 0;
                if(identifier == 4)
                    p.p = 0;
                if(identifier == 5)
                    p.smoke = 0;

                uint count = 0;

                int neighbours[][2] = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};

                for(int k = 0; k < 8; k++){

                    float sx = neighbours[k][0];
                    float sy = neighbours[k][1];

                    const sf::Color& c1 = object.getPixel(i + sx, j + sy);
                    Particle& n = particles[coords2index(i + sx, j + sy, width)];


                    if(c1.a != 255){

                        count ++;

                        float len_sqr = sx*sx + sy*sy;

                        float vx = ((n.vx*sx + n.vy*sy) / len_sqr) * sx;
                        float vy = ((n.vx*sx + n.vy*sy) / len_sqr) * (+sy);

                        if(identifier == 1)
                            p.vx += -vx;
                        
                        if(identifier == 2)
                            p.vy += -vy;

                        if(identifier == 3)
                            p.div += n.div;

                        if(identifier == 4)
                            p.p += n.p;

                        if(identifier == 5)
                            p.smoke += n.smoke;
                    }
                
                }
                

                if(count > 0){
                    if(identifier == 1)
                        p.vx = p.vx;
                    
                    if(identifier == 2)
                        p.vy = p.vy;

                    if(identifier == 3)
                        p.div = p.div / count;
                    
                    if(identifier == 4)
                        p.p = p.p / count;

                    if(identifier == 5)
                        p.smoke = p.smoke / count;
                }
           }
            
        }

    }

}

float Tunnel::calculate_lift(){
    float lift = 0;

    for(uint i = 0; i < width; i++){
        for(uint j = 0; j < height; j++){
            Particle& p = particles[coords2index(i, j, width)];

            const sf::Color& c = object.getPixel(i, j);
            if(c.a == 255){
                lift += p.vy;
            }

        }
    }
       
    return lift;
}

float Tunnel::calculate_drag(){
    float drag = 0;

    for(uint i = 0; i < width; i++){
        for(uint j = 0; j < height; j++){
            Particle& p = particles[coords2index(i, j, width)];

            const sf::Color& c = object.getPixel(i, j);
            if(c.a == 255){
                drag += -p.vx;
            }

        }
    }
       
    return drag;
}