#include <SFML/Graphics.hpp>
#include "stdio.h"
#include <string.h>
#include <string>
#include "math.h"
#include <thread>
#include <chrono>
#include "misc.h"
#include "Tunnel.h"


int main(int args, char** argv){

    float WIDTH = 120;
    float HEIGHT = 100;

    float WINDOW_WIDTH = 1200;
    float WINDOW_HEIGHT = 1000;

    uint block_size = (uint) (WINDOW_WIDTH / WIDTH);

    float simulation_time = 20;
    float delta = 0.04;
    uint threads = 1;
    std::string object = "wing";

    bool render = false;
    bool render_energy = false;
    bool render_velocities = false;
    bool render_pressure = false;

    for(int i = 0; i < args; i++){
        if(strcmp(argv[i], "-render") == 0){
            render = true;
        }

        if(strcmp(argv[i], "-energy") == 0){
            render_energy = true;
        }

        if(strcmp(argv[i], "-velocities") == 0){
            render_velocities = true;
        }

        if(strcmp(argv[i], "-time") == 0){
            simulation_time = atof(argv[i + 1]);
        }

        if(strcmp(argv[i], "-threads") == 0){
            threads = atoi(argv[i + 1]);
        }

        if(strcmp(argv[i], "-pressure") == 0){
            render_pressure = true;
        }

        if(strcmp(argv[i], "-object") == 0){
            object = std::string(argv[i + 1]);
        }

    }

    Tunnel t("objects/" + object + ".png", WIDTH, HEIGHT, 50.0 / HEIGHT, threads, 20, 50);

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Fluid Simulation");

    int frames = (int) (simulation_time / delta);
    for(int frame = 0; frame < frames; frame++){        

        auto start = std::chrono::high_resolution_clock::now();
        t.physics(delta);
        auto end = std::chrono::high_resolution_clock::now();

        window.clear();
        t.draw_object(window, block_size);
        t.drawParticles(window, block_size, render_energy, render_velocities, render_pressure);
        
        window.display();

        std::cout << "Lift: " << t.calculate_lift() << std::endl;
        std::cout << "Drag: " << t.calculate_drag() << std::endl;


        if(render){
            sf::Image screenshot = window.capture();
            
            screenshot.saveToFile("render/" + std::to_string(frame) + ".png");
            printf("Rendered frame %d at simulation time %fs\n", frame, frame*delta);
        }
    }

    if(render){
		printf("Rendering ...\n");
		system(std::string(("ffmpeg -y -framerate ") + std::to_string((int) (1 / delta)) + std::string(" -i render/%d.png render/output.mp4 > /dev/null")).c_str());
		printf("Deleting pngs ...\n");
		system("rm -r render/*.png");
		printf("Done.\n");
	}

}