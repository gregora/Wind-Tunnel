# Wind Tunnel
A wind tunnel simulation using Navier-Stokes equations. Project is built on top of my [Navier-Stokes library](https://github.com/gregora/Navier-Stokes).

The project is written in C++ with CUDA used as hardware acceleration for physics computations.

## Showcase

[sphere.webm](https://github.com/user-attachments/assets/977c7521-c4a4-4afa-98c4-17b6c00de87a)


### Sphere

![Spherical object](render/sphere.png)

A simulation of a sphere put in a wind tunnel. Above you can see a screenshot of the simulation and below the resulting graph for lift and drag.

![Graph](data/figure_sphere.png)

The oscilating values for both lift and drag are a result of vortices behind the sphere.

The oscilations seem to have a frequency of 1.25Hz, however I did not investigate this further.

Raw data for this experiment is available in `data/sphere.csv`.

## Usage

### Dependencies
* SFML

### Compilation
Run `make` in the root directory. As this project is written both for CUDA and non-CUDA devices you can compile for both.

Run `make main.out` to build the non-CUDA version that runs on the processor.

Run `make mainCUDA.out` to use `nvcc` compiler to compile for your GPU.

### Running
Run `./main.out` or `./mainCUDA.out` in the root directory. The program supports the following command line arguments:
* `-object <object name>` - the object to simulate. Only use name of the file in `/objects/` folder
* `-energy` - shows the total energy of the system at each time step
* `-time <time>` - sets simulation time to `<time>` seconds
* `-velocities` - render velocity field together with smoke
* `-pressure` - wheter to render pressure or not. If pressure is not rendered, smoke is
* `-threads <threads>` - number of CPU threads to use
* `-render` - renders the system to a video file in the `render` directory as `output.mp4`
* `-auto` - automatically sets simulation delta as calculated via `max_delta()` function
* `-save <file name>` - saves simulation results as a csv file with columns `time, lift, drag`

## CPU vs GPU performance

Performance was tested a system with Ryzen 7 5800X CPU with 8 cores and Nvidia RTX 3060 GPU.

Physics time was calculated to be ~0.15s single and ~0.10s multi core on the CPU.
Physics calculations took ~0.005s on the GPU.

All together, that makes the CUDA version about 20x - 30x faster.
