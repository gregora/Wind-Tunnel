#include "Tunnel.h"

Tunnel::Tunnel(std::string object_file, uint width, uint height, float dx, float scale, uint threads, uint gs_iters, float speed) : Fluid(width, height, dx) {

    threads = threads;
    gs_iters = gs_iters;

    this -> speed = speed;

    sf::Image object;
    object.loadFromFile(object_file);

    object_mask = new uint8_t[width * height];

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){

            
            float object_x = object.getSize().x/2 + ((i - (float) width/2) / scale);
            float object_y = object.getSize().y/2 + ((j - (float) height/2) / scale);

            if(object_x >= 0 && object_x < object.getSize().x && object_y >= 0 && object_y < object.getSize().y){
                object_mask[coords2index(i, j, width)] = object.getPixel((int) object_x, (int) object_y).a;
            } else {
                object_mask[coords2index(i, j, width)] = 0;
            }
        }
    }

    uint8_t *d_object_mask = NULL;

    cudaMalloc(&d_object_mask, width * height);
    cudaMemcpy(d_object_mask, object_mask, width * height, cudaMemcpyHostToDevice);

    object_mask_CUDA = d_object_mask;
}

Tunnel::~Tunnel(){
    delete object_mask;
    cudaFree(object_mask_CUDA);
}



void Tunnel::draw_object(sf::RenderWindow& window, float block_size){

    sf::RectangleShape rect(sf::Vector2f(block_size, block_size));

    sf::Color c(255, 255, 255);

    for(uint i = 0; i < width; i++){

        for(uint j = 0; j < height; j++){

            rect.setPosition(i * block_size, j * block_size);
            c.a = object_mask[coords2index(i, j, width)];
            rect.setFillColor(c);
            window.draw(rect);
        }

    }

}


void Tunnel::set_boundaries_sector(Particle* particles, uint start, uint end, uint identifier){

}

__global__
void set_boundaries_kernel(Particle* particles, uint width, uint height, uint identifier, float speed, uint8_t *object_mask){

    uint i = blockIdx.x;
    uint j = threadIdx.x;


    uint smoke_start = (uint) (0.40 * height);
    uint smoke_end = (uint) (0.60 * height);

    if((i == 0 || i == width - 1) && (j >= 1 && j < height - 1)){


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
        
            if(j >= smoke_start && j <= smoke_end){
                particles[coords2index(0, j, width)].smoke = 0.5;
            }

        }
    }

    if(j == 0 || j == height - 1) {

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


    if(i > 0 && i < width - 1 && j > 0 && j < height - 1){

        particles[coords2index(i, j, width)].Fy = 9.81;


        Particle& p = particles[coords2index(i, j, width)];

        const uint8_t c = object_mask[coords2index(i, j, width)];         

        if(c == 255){
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

                const uint8_t c1 = object_mask[coords2index(i + sx, j + sy, width)];
                Particle& n = particles[coords2index(i + sx, j + sy, width)];


                if(c1 != 255){

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


void Tunnel::set_boundaries(Particle* particles, uint width, uint height, uint identifier){  
    set_boundaries_kernel<<<width, height>>>(particles, width, height, identifier, speed, object_mask_CUDA);
}


float Tunnel::calculate_lift(){
    float lift = 0;

    for(uint i = 0; i < width; i++){
        for(uint j = 0; j < height; j++){
            Particle& p = particles[coords2index(i, j, width)];

            const uint8_t c = object_mask[coords2index(i, j, width)];
            if(c == 255){
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

            const uint8_t c = object_mask[coords2index(i, j, width)];
            if(c == 255){
                drag += -p.vx;
            }

        }
    }
       
    return drag;
}