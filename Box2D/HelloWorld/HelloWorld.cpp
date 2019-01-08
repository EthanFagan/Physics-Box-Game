#include <SFML\Graphics.hpp>
#include <Box2D\Box2D.h>

#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 
/** We need this to easily convert between pixel and real-world coordinates*/
static const float SCALE = 30.f;

/** Create the base for the boxes to land */
void CreateGround(b2World& World, float X, float Y);

/** Create the boxes */
void CreateBox(b2World& World, int MouseX, int MouseY);

int main()
{
	/** Prepare the window */
	sf::RenderWindow Window(sf::VideoMode(1200, 600, 32), "Box2D and SFML");
	Window.setFramerateLimit(60);

	/** Prepare the world */
	b2Vec2 Gravity(0.f, 9.8f);
	b2World World(Gravity);
	CreateGround(World, 400.f, 500.f);

	/** Prepare the background */
	sf::Texture backGroundtexture;
	backGroundtexture.loadFromFile("background.png");
	sf::Sprite backGroundSprite;
	backGroundSprite.setTexture(backGroundtexture);

	/** Timer for block spawning */
	int m_blockSpawnTimer = 0;

	/** Prepare textures */
	sf::Texture GroundTexture;
	sf::Texture BoxTexture;
	GroundTexture.loadFromFile("ground.png");
	BoxTexture.loadFromFile("box.png");

	int m_score = 0;

	/**setup for targets **/
	sf::Texture m_targetTexture;
	m_targetTexture.loadFromFile("range.png");
	sf::Sprite m_targets[5];
	sf::Vector2f m_targetPositions[5];

	for (int i = 0; i < 5; i++)
	{
		m_targets[i].setTexture(m_targetTexture);
		m_targetPositions[i] = {sf::Vector2f(1000, 100*i)};

		m_targets[i].setPosition(m_targetPositions[i]);
	}


	/** Prepare the player */
	sf::Texture m_playerTexture;
	sf::Sprite m_playerSprite;
	sf::Vector2f m_playerPosition{ sf::Vector2f(100,520) };
	int playerX = 0;
	int playerAnimationTimer = 0;
	m_playerTexture.loadFromFile("Player.png");
	m_playerSprite.setTexture(m_playerTexture);
	m_playerSprite.setTextureRect(sf::IntRect(playerX, 96, 32, 32));
	m_playerSprite.setOrigin(32, 32);
	m_playerSprite.setScale(2.0f, 2.0f);
	m_playerSprite.setPosition(m_playerPosition);

	int m_shotTimer = 0;

	while (Window.isOpen())
	{
		if (m_shotTimer > 0)
		{
			m_shotTimer--;
		}
		else
		{
			m_shotTimer = 0;
		}


		if (m_blockSpawnTimer > 0)
		{
			m_blockSpawnTimer--;
		}
		else
		{
			m_blockSpawnTimer = 0;
		}
		if (playerAnimationTimer == 8)
		{
			if (playerX < 192)
			{
				playerX += 32;
			}
			else
			{
				playerX = 0;
			}
			m_playerSprite.setTextureRect(sf::IntRect(playerX, 96, 32, 32));
			playerAnimationTimer = 0;
		}
		else
		{
			playerAnimationTimer++;
		}
		if (m_blockSpawnTimer == 0)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				CreateBox(World, 200, -600);
				m_blockSpawnTimer = 60;
			}
		}
		
		if (m_shotTimer == 0)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				for (b2Body* BodyIterator = World.GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
				{
					if (BodyIterator->GetType() == b2_dynamicBody)
					{
						BodyIterator->ApplyForce(b2Vec2{ 175.0f, 0.0f },b2Vec2{ 10.0f, 10.0f }, 1);
					}
				}
				m_shotTimer = 60;
			}
		}
		
		World.Step(1 / 60.f, 8, 3);

		Window.clear(sf::Color::White);
		int BodyCount = 0;
		Window.draw(backGroundSprite);
		for (b2Body* BodyIterator = World.GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
		{
			if (BodyIterator->GetType() == b2_dynamicBody)
			{
				sf::Sprite Sprite;
				Sprite.setTexture(BoxTexture);
				Sprite.setOrigin(16.f, 16.f);
			/*	Sprite.setColor(sf::Color::Black);*/
				Sprite.setPosition(SCALE * BodyIterator->GetPosition().x, SCALE * BodyIterator->GetPosition().y);
				Sprite.setRotation(BodyIterator->GetAngle() * 180 / b2_pi);
				Window.draw(Sprite);
				++BodyCount;
			}
			else
			{
				sf::Sprite GroundSprite;
				GroundSprite.setTexture(GroundTexture);
				GroundSprite.setOrigin(400.f, 8.f);
				GroundSprite.setPosition(BodyIterator->GetPosition().x * SCALE, BodyIterator->GetPosition().y * SCALE);
				GroundSprite.setRotation(180 / b2_pi * BodyIterator->GetAngle());
				Window.draw(GroundSprite);
			}
		}
		Window.draw(m_playerSprite);
		for (int i = 0; i < 5; i++)
		{
			Window.draw(m_targets[i]);
		}
		Window.display();
	}

	return 0;
}

void CreateBox(b2World& World, int MouseX, int MouseY)
{
	b2BodyDef BodyDef;
	BodyDef.position = b2Vec2(MouseX / SCALE, MouseY / SCALE);
	BodyDef.type = b2_dynamicBody;
	b2Body* Body = World.CreateBody(&BodyDef);

	b2PolygonShape Shape;
	Shape.SetAsBox((32.f / 2) / SCALE, (32.f / 2) / SCALE);
	b2FixtureDef FixtureDef;
	FixtureDef.density = 1.f;
	FixtureDef.friction = 0.7f;
	FixtureDef.shape = &Shape;
	Body->CreateFixture(&FixtureDef);
}

void CreateGround(b2World& World, float X, float Y)
{
	b2BodyDef BodyDef;
	BodyDef.position = b2Vec2(X / SCALE, Y / SCALE);
	BodyDef.type = b2_staticBody;
	b2Body* Body = World.CreateBody(&BodyDef);

	b2PolygonShape Shape;
	Shape.SetAsBox((800.f / 2) / SCALE, (16.f / 2) / SCALE);
	b2FixtureDef FixtureDef;
	FixtureDef.density = 0.f;
	FixtureDef.shape = &Shape;
	Body->CreateFixture(&FixtureDef);
}