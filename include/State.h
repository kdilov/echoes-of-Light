#pragme once 
#include <SFML/Graphics.hpp>

class State {
public:
	virtual ~State() = default;

	virtual void handleInput(sf::RenderWindow& window) = 0;
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderWindow window) = 0;
};