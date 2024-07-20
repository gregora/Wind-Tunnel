#include <SFML/Graphics.hpp>
#include "stdio.h"
#include <string.h>
#include <string>
#include "math.h"
#include <thread>
#include <chrono>
#include "misc.h"
#include "Tunnel.h"
#include <fstream>
#include <iostream>

int main(int args, char** argv){

    float WIDTH = 400;
    float HEIGHT = 80;

    float WINDOW_WIDTH = 1000;
    float WINDOW_HEIGHT = 200;

    uint block_size = (uint) (WINDOW_WIDTH / WIDTH);

    float simulation_time = 20;
    float delta = 0.001;
    uint threads = 1;
    std::string object = "wing";
    float object_scale = HEIGHT * 0.2 / 1000;
    object_scale = 1;

    int subcomputatoins = 1;

    bool render = false;
    bool render_energy = false;
    bool render_velocities = false;
    bool render_pressure = false;

    bool auto_delta = false;

    std::string save_data = "";
    std::fstream save_file;

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

        if(strcmp(argv[i], "-auto") == 0){
            auto_delta = true;
        }

        if(strcmp(argv[i], "-delta") == 0){
            delta = atof(argv[i + 1]);
        }

        if(strcmp(argv[i], "-subcomputations") == 0){
            subcomputatoins = atof(argv[i + 1]);
        }

        if(strcmp(argv[i], "-save") == 0){
            save_data = std::string(argv[i + 1]);
            save_file.open(save_data, std::ios::out | std::ios::trunc);
        }

    }

    Tunnel t("objects/" + object + ".png", WIDTH, HEIGHT, 50.0 / HEIGHT, object_scale, threads, 20, 50);
    t.debug_performance = true;
    t.threads = threads;
    t.gs_iters = 100;
    if (auto_delta && render){
            printf("ERROR: Rendering with auto delta is not possible!\n");
            return 1;
    }

    if (!auto_delta){
        t.show_warnings = true;
    }
    

    sf::Image icon;
    icon.loadFromFile("icon.png");

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Wind Tunnel Simulation");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    int frame = 0;
    float time_elapsed = 0;
    while (time_elapsed < simulation_time){

        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        if(auto_delta){
            delta = t.max_delta()*0.1;
        }

        auto start = std::chrono::high_resolution_clock::now();
        t.physics(delta);
        auto end = std::chrono::high_resolution_clock::now();

        if(frame % subcomputatoins == 0){
            window.clear();
            t.drawParticles(window, block_size, render_energy, render_velocities, render_pressure);
            t.draw_object(window, block_size);
            
            window.display();


            if(render){
                sf::Image screenshot = window.capture();
                
                screenshot.saveToFile("render/" + std::to_string(frame) + ".png");
                printf("Rendered frame %d at simulation time %fs\n", frame, frame*delta);
            }

        }

        std::cout << std::endl;
        std::cout << "Lift: " << t.calculate_lift() << std::endl;
        std::cout << "Drag: " << t.calculate_drag() << std::endl;

        if(auto_delta){
            std::cout << "Delta: " << delta << std::endl;
        }

        std::cout << std::endl;


        if(save_data != ""){

            save_file << time_elapsed << "," << t.calculate_lift() << "," << t.calculate_drag() << std::endl;
        }

        frame += 1;
        time_elapsed += delta;
        //return 0;
    }

    if(render){
		printf("Rendering ...\n");
		system(std::string(("ffmpeg -y -framerate ") + std::to_string((int) (1 / delta / subcomputatoins)) + std::string(" -i render/%d.png render/output.mp4 > /dev/null")).c_str());
		printf("Deleting pngs ...\n");
		system("rm -r render/*.png");
		printf("Done.\n");
	}

}