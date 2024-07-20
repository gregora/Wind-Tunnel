#include <SFML/Graphics.hpp>
#include "stdio.h"
#include <string.h>
#include <string>
#include "math.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include "misc.h"
#include "Tunnel.h"
#include "Video.h"


int main(int args, char** argv){

    float WIDTH = 400;
    float HEIGHT = 80;

    float WINDOW_WIDTH = 1000;
    float WINDOW_HEIGHT = 200;

    float scale = WINDOW_WIDTH / WIDTH;

    float simulation_time = 20;
    float delta = 0.001;
    uint threads = 1;
    std::string object = "sphere";

    float object_scale = HEIGHT * 0.3 / 1000;
    if(object == "wing"){
        object_scale = HEIGHT * 1 / 1000;
    }else if(object == "sphere"){
        object_scale = HEIGHT * 0.2 / 1000;
    }

    int subcomputations = 10;

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
            subcomputations = atof(argv[i + 1]);
        }

        if(strcmp(argv[i], "-save") == 0){
            save_data = std::string(argv[i + 1]);
            save_file.open(save_data, std::ios::out | std::ios::trunc);
        }

    }

    Tunnel t("objects/" + object + ".png", WIDTH, HEIGHT, 50.0 / HEIGHT, object_scale, threads, 20, 50);
    t.debug_performance = true;
    t.threads = threads;
    t.gs_iters_diffuse = 100;
    t.gs_iters_incompressibility = 100;
    if (auto_delta && render){
            printf("ERROR: Rendering with auto delta is not possible!\n");
            return 1;
    }

    if (!auto_delta){
        t.show_warnings = true;
    }
    
    Video* video = NULL;

    if(render){
        video = new Video(WINDOW_WIDTH, WINDOW_HEIGHT, 1 / delta / subcomputations, "render/output.mp4");
    }

    sf::Image icon;
    icon.loadFromFile("icon.png");

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT + 100), "Wind Tunnel Simulation");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    sf::RenderTexture tunnel_texture;
    tunnel_texture.create(WINDOW_WIDTH, WINDOW_HEIGHT);

    sf::Font font;
    font.loadFromFile("fonts/Prototype.ttf");

    sf::Text lift_text;
    lift_text.setFont(font);
    lift_text.setCharacterSize(15);
    lift_text.setFillColor(sf::Color::White);
    lift_text.setPosition(10, WINDOW_HEIGHT + 10);

    sf::Text drag_text;
    drag_text.setFont(font);
    drag_text.setCharacterSize(15);
    drag_text.setFillColor(sf::Color::White);
    drag_text.setPosition(10, WINDOW_HEIGHT + 40);

    sf::Text delta_text;
    delta_text.setFont(font);
    delta_text.setCharacterSize(15);
    delta_text.setFillColor(sf::Color::White);
    delta_text.setPosition(10, WINDOW_HEIGHT + 70);

    sf::Text time_text;
    time_text.setFont(font);
    time_text.setCharacterSize(20);
    time_text.setFillColor(sf::Color::White);
    time_text.setPosition(WINDOW_WIDTH - 200, WINDOW_HEIGHT + 10);


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

        float lift = t.calculate_lift();
        float drag = t.calculate_drag();

        if(frame % subcomputations == 0){
            //clear with gray
            window.clear(sf::Color(20, 20, 20));


            t.drawParticles(tunnel_texture, scale, render_energy, render_velocities, render_pressure);
            t.draw_object(tunnel_texture, scale);
            
            sf::Sprite tunnel_sprite(tunnel_texture.getTexture());
            window.draw(tunnel_sprite);

            lift_text.setString("Lift: " + std::to_string((int) lift));
            drag_text.setString("Drag: " + std::to_string((int) drag));
            delta_text.setString("Delta: " + std::to_string(delta));
            time_text.setString("Time: " + std::to_string(time_elapsed) + "s");

            window.draw(lift_text);
            window.draw(drag_text);
            window.draw(delta_text);
            window.draw(time_text);

            window.display();


            if(render){
                //save only tunnel_texture as video

                sf::Image img = tunnel_texture.getTexture().copyToImage();
                uint8_t* pixels = new uint8_t[img.getSize().x * img.getSize().y * 4];
                for(int i = 0; i < img.getSize().x; i++){
                    for(int j = 0; j < img.getSize().y; j++){
                        sf::Color color = img.getPixel(i, j);
                        pixels[(i + j * img.getSize().x) * 4] = color.r;
                        pixels[(i + j * img.getSize().x) * 4 + 1] = color.g;
                        pixels[(i + j * img.getSize().x) * 4 + 2] = color.b;
                        pixels[(i + j * img.getSize().x) * 4 + 3] = 255;
                    }
                }

                video->write(pixels);
            }

        }

        if(save_data != ""){

            save_file << time_elapsed << "," << lift << "," << drag << std::endl;
        }

        frame += 1;
        time_elapsed += delta;
        //return 0;
    }

    if(render){
        video->release();
    }

}