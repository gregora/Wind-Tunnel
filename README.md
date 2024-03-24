# Wind Tunnel
A wind tunnel simulation using Navier-Stokes equations.

## Usage

### Dependencies
* SFML

### Compilation
Run `make` in the root directory.

### Running
Run `./main.out` in the root directory. The program supports the following command line arguments:
* `-object <object name>` - the object to simulate. Only use name of the file in `/objects/` folder
* `-energy` - shows the total energy of the system at each time step
* `-time <time>` - sets simulation time to `<time>` seconds
* `-velocities` - render velocity field together with smoke
* `-pressure` - wheter to render pressure or not. If pressure is not rendered, smoke is
* `-threads <threads>` - number of CPU threads to use
* `-render` - renders the system to a video file in the `render` directory as `output.mp4`
* `-auto` - automatically sets simulation delta as calculated via `max_delta()` function
