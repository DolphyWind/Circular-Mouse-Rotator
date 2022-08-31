#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include "functions.h"
#include "data.h"
using namespace std;

sf::Color limeColor = sf::Color(50, 205, 50);

int main()
{
	sf::RenderWindow window(sf::VideoMode(300, 600), "Circular Mouse Rotator", sf::Style::Close);
	window.setFramerateLimit(60);

	sf::Font font;
	font.loadFromMemory(&font_data[0], font_data.size());
	const int textureSize = 300;

	sf::Image screenshot;
	sf::Texture texture;
	sf::Sprite screen;
	float radius = 100;

	sf::Text prewiewText("Preview", font);
	prewiewText.setCharacterSize(30);
	screen.setPosition(0, prewiewText.getGlobalBounds().height * 2.25);
	prewiewText.setPosition((textureSize - prewiewText.getGlobalBounds().width) / 2, 0.25 * prewiewText.getGlobalBounds().height * 30 / prewiewText.getCharacterSize());

	sf::Text previewUnavalibleText("Preview is unavalible :(", font);
	previewUnavalibleText.setCharacterSize(25);
	previewUnavalibleText.setPosition((textureSize - previewUnavalibleText.getGlobalBounds().width) / 2, screen.getPosition().y + (textureSize / 2 - (previewUnavalibleText.getGlobalBounds().height * 30 / previewUnavalibleText.getCharacterSize())));

	sf::Vector2i ms, msw;
	RECT desktopSize;
	GetWindowRect(GetDesktopWindow(), &desktopSize);
	bool preview = true;

	sf::Text radiusConstantText("Radius", font);
	radiusConstantText.setCharacterSize(20);
	radiusConstantText.setPosition((window.getSize().x - radiusConstantText.getGlobalBounds().width) / 2, screen.getPosition().y + textureSize + 10);

	sf::RectangleShape minusButton(sf::Vector2f(80, 45));
	minusButton.setFillColor(limeColor);
	minusButton.setPosition((window.getSize().x - minusButton.getSize().x) / 8, radiusConstantText.getPosition().y + radiusConstantText.getGlobalBounds().height + 20);

	sf::RectangleShape plusButton(sf::Vector2f(80, 45));
	plusButton.setFillColor(limeColor);
	plusButton.setPosition(7 * (window.getSize().x - plusButton.getSize().x) / 8, minusButton.getPosition().y);

	sf::RectangleShape startButton(sf::Vector2f(plusButton.getPosition().x + plusButton.getSize().x - minusButton.getPosition().x, 45));
	startButton.setFillColor(limeColor);
	startButton.setPosition(minusButton.getPosition().x, minusButton.getPosition().y + minusButton.getSize().y + 20);

	sf::Text minusText("-", font);
	minusText.setCharacterSize(30);
	minusText.setPosition(minusButton.getPosition().x + (minusButton.getSize().x - minusText.getGlobalBounds().width) / 2, minusButton.getPosition().y + (minusButton.getSize().y / 2 - 10 * (minusText.getGlobalBounds().height * 30 / minusText.getCharacterSize())));

	sf::Text plusText("+", font);
	plusText.setCharacterSize(30);
	plusText.setPosition(plusButton.getPosition().x + (plusButton.getSize().x - plusText.getGlobalBounds().width) / 2, plusButton.getPosition().y + (plusButton.getSize().y / 2 - 1.5 * (plusText.getGlobalBounds().height * 30 / plusText.getCharacterSize())));

	sf::Text radiusText(toString(radius), font);
	radiusText.setPosition((window.getSize().x - radiusText.getGlobalBounds().width) / 2, minusText.getPosition().y);

	sf::Text startText("Start / Stop (Pause Key)", font);
	startText.setCharacterSize(20);
	startText.setPosition((window.getSize().x - startText.getGlobalBounds().width) / 2, startButton.getPosition().y + startButton.getSize().y / 2 - (startText.getGlobalBounds().height) / 1.5);

	sf::Text aboutText(L"Made by Yunus Emre Aydýn", font);
	aboutText.setCharacterSize(12);
	aboutText.setPosition((window.getSize().x - aboutText.getGlobalBounds().width) - 10, window.getSize().y - aboutText.getGlobalBounds().height * 3.5);
	
	sf::Text tipText("For better performance, minimize this window.", font);
	tipText.setCharacterSize(12);
	tipText.setPosition((window.getSize().x - tipText.getGlobalBounds().width) - 10, window.getSize().y - tipText.getGlobalBounds().height * 2);

	sf::RectangleShape mouseShape(sf::Vector2f(1, 1));
	mouseShape.setOrigin(0.5, 0.5);

	sf::Vector2i lastPosition;

	bool lPress = true;
	bool pausePress = true;
	bool startPress = true;
	bool started = false;
	sf::Clock lPClock;
	sf::Time lPTime;

	float min_radius = 20;
	float max_radius = (desktopSize.right < desktopSize.bottom ? desktopSize.right : desktopSize.bottom) / 2;

	float rotation;
	float angle = 0;

	while (window.isOpen())
	{
		sf::Event e;
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed) window.close();
		}

		if (lPress) lPClock.restart();
		lPTime = lPClock.getElapsedTime();
		if (!lPress) if (lPTime.asSeconds() > 0.3) lPress = true;

		sf::Vector2i mousePos = (started ? lastPosition : ms);

		if (started)
		{
			float angle_rad = angle * M_PI / 180;
			sf::Mouse::setPosition(sf::Vector2i(mousePos.x + cos(angle_rad) * radius, mousePos.y - sin(angle_rad) * radius));
			angle += 1;
		}

		//startPress = !(sf::Keyboard::isKeyPressed(sf::Keyboard::Pause) || (sf::Mouse::isButtonPressed(sf::Mouse::Left) && mouseShape.getGlobalBounds().intersects(startButton.getGlobalBounds())));

		ms = sf::Mouse::getPosition();
		msw = sf::Mouse::getPosition(window);
		mouseShape.setPosition(msw.x, msw.y);

		float delta = ceil(pow(lPTime.asSeconds(), 1.5));
		if (delta == 0) delta = 1;

		if (window.hasFocus())
		{
			screenshot = sf::Image();
			texture = sf::Texture();
			screen = sf::Sprite();
			screen.setPosition(0, (prewiewText.getGlobalBounds().height) * 2.25);
			HBITMAP hScreenshot = CaptureScreen();
			SFMLLoadHBitmapAsImage(hScreenshot, &screenshot);
			DeleteObject(hScreenshot);

			if (!started)
			{
				if (ms.x < desktopSize.left || ms.y < desktopSize.top || ms.x > desktopSize.right || ms.y > desktopSize.bottom)
					preview = false;
				else preview = true;
			}

			if (preview)
			{

				for (double deg = 0; deg < 360; deg += 0.1)
				{
					double rad = deg * M_PI / 180;
					float i_min_max = radius / 45;
					for (float i = -i_min_max; i <= i_min_max; i++)
					{
						float x = mousePos.x + cos(rad) * (radius + i);
						float y = mousePos.y - sin(rad) * (radius + i);
						if (x < desktopSize.left) x = desktopSize.left;
						if (y < desktopSize.top) y = desktopSize.top;
						if (x > desktopSize.right - 1) x = desktopSize.right - 1;
						if (y > desktopSize.bottom - 1) y = desktopSize.bottom - 1;
						screenshot.setPixel(x, y, sf::Color::Red);
					}

				}
				float width = 2 * radius;
				float height = 2 * radius;
				float tempX = desktopSize.right - 1 - mousePos.x;
				if (tempX > width) tempX = width;
				float tempY = desktopSize.bottom - 1 - mousePos.y;
				if (tempY > height) tempY = height;

				texture.loadFromImage(screenshot, sf::IntRect(mousePos.x - radius - (width - tempX), mousePos.y - radius - (height - tempY), width, height));
				screen.setTexture(texture);
				sf::Vector2f scale = sf::Vector2f(textureSize / width, textureSize / height);
				screen.setScale(scale.x, scale.y);
			}

			if (!started)
			{
				if (mouseShape.getGlobalBounds().intersects(minusButton.getGlobalBounds()))
				{
					minusButton.setFillColor(sf::Color(limeColor.r, limeColor.g, limeColor.b, 205));
					if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && lPress)
					{
						radius -= delta;
						lPress = false;
					}
				}
				else
				{
					minusButton.setFillColor(sf::Color(limeColor.r, limeColor.g, limeColor.b, 255));
				}
				if (mouseShape.getGlobalBounds().intersects(plusButton.getGlobalBounds()))
				{
					plusButton.setFillColor(sf::Color(limeColor.r, limeColor.g, limeColor.b, 205));
					if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && lPress)
					{
						radius += delta;
						lPress = false;
					}
				}
				else
				{
					plusButton.setFillColor(sf::Color(limeColor.r, limeColor.g, limeColor.b, 255));
				}
			}
			if (mouseShape.getGlobalBounds().intersects(startButton.getGlobalBounds()))
			{
				startButton.setFillColor(sf::Color(limeColor.r, limeColor.g, limeColor.b, 205));
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if (startPress)
					{
						lastPosition = ms;
						started = !started;
						if (!started)
						{
							angle = 0;
							sf::Mouse::setPosition(mousePos);
						}
					}
					startPress = false;
				}
				else startPress = true;
			}
			else
			{
				startButton.setFillColor(sf::Color(limeColor.r, limeColor.g, limeColor.b, 255));
			}
			if (!started)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) && lPress)
				{
					radius -= delta;
					lPress = false;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add) && lPress)
				{
					radius += delta;
					lPress = false;
				}
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Pause))
		{
			if (pausePress)
			{
				lastPosition = ms;
				started = !started;
				if (!started)
				{
					angle = 0;
					sf::Mouse::setPosition(mousePos);
				}
				pausePress = false;
			}
		}
		else pausePress = true;

		if (radius < min_radius) radius = min_radius;
		if (radius > max_radius) radius = max_radius;

		radius = (int)radius;
		radiusText.setString(toString(radius));
		radiusText.setPosition((window.getSize().x - radiusText.getGlobalBounds().width) / 2, minusText.getPosition().y);

		window.clear();

		if (window.hasFocus())
		{
			window.draw(prewiewText);
			window.draw(previewUnavalibleText);
			if (preview) window.draw(screen);
			window.draw(radiusConstantText);
			window.draw(minusButton);
			window.draw(plusButton);
			window.draw(minusText);
			window.draw(plusText);
			window.draw(radiusText);
			window.draw(startButton);
			window.draw(startText);
			window.draw(aboutText);
			window.draw(tipText);
		}

		window.display();
	}
	return 0;
}