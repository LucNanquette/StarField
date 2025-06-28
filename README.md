# StarField

StarField is a high-performance graphical simulation that displays thousands of moving stars, creating a mesmerizing starfield effect. Built with [SFML 3.0](https://www.sfml-dev.org/), it leverages hardware acceleration and efficient rendering using `sf::VertexArray` to animate up to 100,000 stars in real-time.

## Features

- **Smooth Starfield Animation**: Simulates stars moving in 3D space, giving the illusion of flying through space.
- **Highly Optimized**: Uses vertex arrays for efficient GPU-accelerated rendering, allowing for a very high number of stars without performance loss.
- **FPS Display**: Optionally displays frames per second for performance monitoring.
- **Fullscreen Mode**: Launches in fullscreen and adapts to your desktop resolution.

## How it Works

- Stars are randomly generated in 3D space and projected onto the 2D screen.
- As time progresses, stars move closer to the viewer, simulating depth and motion.
- When a star passes the viewer, it is recycled to the far plane, creating an endless effect.
- Each star is drawn as a textured square (built using two triangle primitives) and rendered with a single draw call for maximum efficiency.

## Requirements

- [SFML 3.0](https://www.sfml-dev.org/) (Simple and Fast Multimedia Library)
- C++20 compatible compiler

## Getting Started

1. **Clone the repository**:
   ```bash
   git clone https://github.com/LucNanquette/StarField.git
   cd StarField
   ```

2. **Install SFML 3.0** and ensure it is available for your build system.

3. **Build the project** (example using g++ and SFML installed system-wide):
   ```bash
   g++ src/main.cpp -o starfield -lsfml-graphics -lsfml-window -lsfml-system
   ```

4. **Run the simulation**:
   ```bash
   ./starfield
   ```

## Controls

- `ESC`: Exit the application.

## Credits

- Uses [SFML](https://www.sfml-dev.org/) for graphics and window management.

---

Feel free to fork and experiment with the code to create your own visual effects!

PS: This description was generated using CoPilot 
