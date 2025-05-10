#include <random>
#include <SFML/Graphics.hpp>
#include "config.h"

// Define a star
struct Star
{
	sf::Vector2f position;
	float z = 1.0f;
};

// Create a vector of random stars
static std::vector<Star> GenerateStars(uint32_t count, float scale, sf::Vector2f screen_size)
{
	std::vector<Star> stars;
	stars.reserve(count);

	// Initialize a random number generator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	// Define a restricted zone to prevent stars from just disappearing in front of view
	sf::Vector2f const viewport_size = screen_size * cf::near;
	sf::FloatRect const restricted_zone = {-viewport_size * 0.5f, viewport_size * 0.5f};

	// Create randomly distributed stars on the screen
	for (size_t i {count}; i--;)
	{
		float const x = (dis(gen) - 0.5f) * screen_size.x * scale;
		float const y = (dis(gen) - 0.5f) * screen_size.y * scale;
		float const z = (cf::far - cf::near) * dis(gen) + cf::near;

		// Discard stars that fall into the restricted zone
		if (restricted_zone.contains({x, y}))
		{
			++i;
			continue;
		}

		// Else add it in the vector
		stars.push_back({{x, y}, z});
	}

	// Depth ordering
	std::sort(stars.begin(), stars.end(), [](Star const &s_1, Star const &s_2) {
		return s_1.z > s_2.z;
	});

	return stars;
}

static void UpdateGeometry(uint32_t idx, Star const &s, sf::VertexArray &va)
{
	float const scale = 1.0f / s.z;
	float const depth_ratio = (s.z - cf::near) / (cf::far - cf::near);
	float const color_ratio = 1.0f - depth_ratio;
	auto const c = static_cast<uint8_t>(color_ratio * 255.0f);

	sf::Vector2f const p = s.position * scale;
	float const r = cf::size * scale;
	uint32_t const i = 6 * idx;

	va[static_cast<size_t>(i) + 0].position = {p.x - r, p.y - r};
	va[static_cast<size_t>(i) + 1].position = {p.x + r, p.y - r};
	va[static_cast<size_t>(i) + 2].position = {p.x - r, p.y + r};
	va[static_cast<size_t>(i) + 3].position = {p.x + r, p.y - r};
	va[static_cast<size_t>(i) + 4].position = {p.x + r, p.y + r};
	va[static_cast<size_t>(i) + 5].position = {p.x - r, p.y + r};

	sf::Color const color {c, c, c};
	va[static_cast<size_t>(i) + 0].color = color;
	va[static_cast<size_t>(i) + 1].color = color;
	va[static_cast<size_t>(i) + 2].color = color;
	va[static_cast<size_t>(i) + 3].color = color;
	va[static_cast<size_t>(i) + 4].color = color;
	va[static_cast<size_t>(i) + 5].color = color;
}

int main()
{
	auto window = sf::RenderWindow {sf::VideoMode::getDesktopMode(), "Starfield", sf::State::Fullscreen};
	window.setVerticalSyncEnabled(false);
	window.setMouseCursorVisible(false);

	// Get screen size
	sf::Vector2f const screen_size = static_cast<sf::Vector2f>(window.getSize());

	// Load the star texture and generate a mipmap
	sf::Texture texture;
	if (!texture.loadFromFile("res/star.png"))	throw std::runtime_error("Failed to load texture");
	if (!texture.generateMipmap()) throw std::runtime_error("Failed to generate mipmap");
	texture.setSmooth(true);

	// Load font, used to display FPS
	sf::Font font;
	if (!font.openFromFile("res/Typewriter.ttf")) throw std::runtime_error("Error loading font");

	// Create the stars and reserve a vertex array for them, each star being represented by 2 triangle primitives forming a square, thus 6 vertices per star
	std::vector<Star> stars = GenerateStars(cf::star_total, cf::far, screen_size);
	sf::VertexArray va {sf::PrimitiveType::Triangles, 6 * cf::star_total};

	// Prefill texture coordinates for each star
	auto const texture_size_f = static_cast<sf::Vector2f>(texture.getSize());
	for (uint32_t idx {cf::star_total}; idx--;)
	{
		uint32_t const i = 6 * idx;
		va[static_cast<size_t>(i) + 0].texCoords = {0.0f, 0.0f};
		va[static_cast<size_t>(i) + 1].texCoords = {texture_size_f.x, 0.0f};
		va[static_cast<size_t>(i) + 2].texCoords = {0.0f, texture_size_f.y};
		va[static_cast<size_t>(i) + 3].texCoords = {texture_size_f.x, 0.0f};
		va[static_cast<size_t>(i) + 4].texCoords = {texture_size_f.x, texture_size_f.y};
		va[static_cast<size_t>(i) + 5].texCoords = {0.0f, texture_size_f.y};
	}

	// Setup everything needed for FPS counter
	sf::Text fpsCounter {font, "", 16}; // String used to display the current FPS
	fpsCounter.setFillColor(sf::Color::Yellow);
	fpsCounter.setPosition(sf::Vector2f(10, 10));
	float frameCount {0.0f}; // Counter for frames used in calculation for FPS
	float sT {0.0f};         // Sum of frame times used in calculation for FPS
	float dT {0.0f};	  	   // Frame time

	// Initialize the clock before getting into the main loop
	sf::Clock dClock;
	dClock.start();

	// Main loop
	uint32_t first = 0;
	while (window.isOpen())
	{
		// Eventhandler
		while (std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>()) window.close();
			if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				switch (keyPressed->code)
				{
					case sf::Keyboard::Key::Escape:
						window.close();
						break;
					default:
						break;
				}
			}
		}

		// Update the stars on their z-axis, aka move the stars towards the viewer
		for (uint32_t i {cf::star_total}; i--;)
		{
			Star &s = stars[i];
			s.z -= cf::speed * dT;
			if (s.z < cf::near)
			{
				s.z = cf::far - (cf::near - s.z);
				first = i;
			}
		}

		// Rendering
		window.clear();

		for (uint32_t i {0}; i < cf::star_total; ++i)
		{
			uint32_t const idx = (i + first) % cf::star_total;
			Star const &s = stars[idx];
			UpdateGeometry(i, s, va);
		}

		sf::RenderStates states;
		states.transform.translate(screen_size * 0.5f);
		states.texture = &texture;

		window.draw(va, states);
		if(cf::show_fps) window.draw(fpsCounter);

		window.display();

		// Get frame time
		dT = dClock.restart().asSeconds();

		if(cf::show_fps)
		{
			// Update the FPS counter
			sT += dT;
			frameCount++;

			if (sT > 0.5f) // Update FPS every 500 ms
			{
				float averageFrameTime = sT / frameCount;
				fpsCounter.setString(std::format("FPS: {}", static_cast<int>(1.0f / averageFrameTime)));
				sT = 0.0f;
				frameCount = 0;
			}
		}
	}
}