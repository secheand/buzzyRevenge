/*
Author: Sebastian F. Echeandia
Class: ECE6122
Last Date Modified: October 26, 2021
Description:
A derivation of “Angry Birds” called “Buzzy’s Revenge” using the SFML API. In this game, the user launches buzzy across the field attempting to hit the evil mascots from other universities, while avoiding the cute woodland creatures.
*/

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <unordered_map>


/* Function Header
int main(int argc, char const** argv)

Main function of the code. This function runs the game Buzzy's Revenge using the SFML API. The functionality of the game includes all 16 points required from the Lab 3 instructions PDF.
*/
int main(int argc, char const** argv)
{
    // Seed for random number generator
    srand (time(NULL));
    
    /////////////////////  Definitions and Initializations Section  ////////////////////
    // Initialize events object
    sf::Event event;
    
    // Initialize all rectangle objects for texts and sprites
    sf::FloatRect textRect;
    sf::FloatRect woodlandCreatureRect[10];
    sf::FloatRect buzzyRect;
    sf::FloatRect beeRect;
    sf::FloatRect buzzyLifeRect;
    
    // Initialize all required textures
    sf::Texture backgroundImageTexture;
    sf::Texture woodCreaturesTextures[10];
    sf::Texture buzzyTexture;
    sf::Texture buzzLifeTexture;
    sf::Texture beeTexture;
    
    // Initialize all required Sprites
    sf::Sprite backgroundSprite;
    sf::Sprite woodCreaturesSprite[10];
    sf::Sprite buzzySprite;
    sf::Sprite buzzLifeSprite[5];
    sf::Sprite beeSprite;
    
    // Initialize all text objects
    sf::Text scoreText;
    sf::Text titleText;
    sf::Text instructionText[4];
    sf::Text authorText;
    sf::Text livesText;
    sf::Text powerText;
    sf::Text gameOverText;
    
    // Define font object
    sf::Font font;
    
    // Initialize clock and clock for bee
    sf::Clock clock;
    sf::Clock clock_bee;
    
    // Initialize time and bee time variables
    sf::Time t;
    sf::Time t_bee;
    
    // Initialize boolean variables
    bool acceptInput = false; // True if the game accepts user controls
    bool paused = true; // True if the game is paused
    bool powerBarHold = false; // True if user is holding the space bar
    bool levelRestart = true; // True if a it is necessary to recreate the level
    bool colission = false; // True if there has been a colission
    bool gameOver = false; // True if Buzzy ran out of lives
    
    // Initialize variables to simulate buzzy's motion
    double y = 540; // Buzzy's vertical position
    double x = 50; // Buzzy's horizontal position
    double v = 0; // Buzzy's velocity (magnitude)
    double vy = 0; // Buzzy's velocity (y component)
    double vx = 0; // Buzzy's velocity (x component)
    double ay = 0.1; // Buzzy's acceleration (only on y component)
    double buzzyAngle = 0; // Buzzy's angle of orientation
    
    // Declare variables used to track colission detection points fo Buzzy
    double buzzyHalfWidth; // Half the length of Buzzy
    double buzzyHalfHeight; // Half the height of Buzzy
    double buzzyInternalHypotenuse; // Distance from Buzzy's center to Buzzy's top right corner
    double buzzyInternalAngle; // Angle of Buzzy's internal hypotenuse with respect to the x axis
    std::pair<double, double> buzzyTopRight; // Tracks the coordinates of Buzzy's top right corner
    std::pair<double, double> buzzyMidRight; // Tracks the coordinates of Buzzy's midpoint on the right edge
    std::pair<double, double> buzzyBottomRight; // Tracks the coordinates of Buzzy's bottom right corner
    
    // Declare variables for power bar
    double maxPower = 500; // Indicates the max power achivable by Buzzy
    double powerPerSecond = 100; // How much power increases per second holding down the space bar
    double powerLevel = 0; // Current buzzy's power level
    
    // Declare variables used to track state of the game
    int numberLives = 5; // Lives remaining
    int score = 0; // Player's score
    int mascotsHit = 0; // How many mascots have been hit
    int positionNumber[10]; // Used to track the position of each of the creatures within the grid
    
    // Initialize strings
    std::string scorePhrase = "Score: ";
    
    // Initialize vectors
    std::vector<std::pair<int, int>> positionCoordinates; // Coordinates for each creature position in the game
    std::vector<std::string> woodlandCreaturesFileNames; // Contains names for creatures .png files
    std::vector<std::string> instructionPhrases; // Contains phrases for instructions texts at beginning of game
    
    /////////////////////  Setting up sprites Section  ////////////////////
    
    // Create background: load texture, assign it to sprite and set position
    backgroundImageTexture.loadFromFile("assets/background.png");
    backgroundSprite.setTexture(backgroundImageTexture);
    backgroundSprite.setPosition(0, 0);
    
    // Create Buzzy: Load and assign texture to sprite, find center and set it as origin. Scale sprite
    buzzyTexture.loadFromFile("assets/smallbuzzy.png");
    buzzySprite.setTexture(buzzyTexture);
    buzzyRect = buzzySprite.getLocalBounds();
    buzzySprite.setOrigin(buzzyRect.left + buzzyRect.width / 2.0f, buzzyRect.top + buzzyRect.height / 2.0f);
    buzzySprite.scale(1.2, 1.2);
    
    // Create Bee (insect): Load and assign texture to sprite, find center and set it as origin. Scale sprite
    beeTexture.loadFromFile("assets/insect.png");
    beeSprite.setTexture(beeTexture);
    beeRect = beeSprite.getLocalBounds();
    beeSprite.setOrigin(beeRect.left + beeRect.width / 2.0f, beeRect.top + beeRect.height / 2.0f);
    beeSprite.scale(1.2, 1.2);
    
    // Make 5 Buzzy lives: for each life, assign texture to sprite, set origin, scale and position
    buzzLifeTexture.loadFromFile("assets/buzzy life.png");
    for (int i = 0; i < 5; i++)
    {
        buzzLifeSprite[i].setTexture(buzzLifeTexture);
        buzzyLifeRect = buzzLifeSprite[i].getLocalBounds();
        buzzLifeSprite[i].setOrigin(buzzyLifeRect.left + buzzyLifeRect.width / 2.0f, buzzyLifeRect.top + buzzyLifeRect.height / 2.0f);
        buzzLifeSprite[i].scale(0.45, 0.45);
        buzzLifeSprite[i].setPosition(150 + (70 * i), 45);
    }
    
    // Make all woodland creatues: for each creature, assign texture to sprite, set origin, scale and position
    woodlandCreaturesFileNames.assign({"assets/sheep.png", "assets/frog.png", "assets/bunny.png", "assets/dog.png", "assets/pig.png", "assets/chicken.png", "assets/mouse.png", "assets/angry_unicorn.png", "assets/tiger.png", "assets/Georgia_Bulldogs_logo_dog_g.png"});
    for (int i = 0; i < 10; i++)
    {
        woodCreaturesTextures[i].loadFromFile(woodlandCreaturesFileNames[i]);
        woodCreaturesSprite[i].setTexture(woodCreaturesTextures[i]);
        woodlandCreatureRect[i] = woodCreaturesSprite[i].getLocalBounds();
        woodCreaturesSprite[i].setOrigin(woodlandCreatureRect[i].left + woodlandCreatureRect[i].width / 2.0f, woodlandCreatureRect[i].top + woodlandCreatureRect[i].height / 2.0f);
        woodCreaturesSprite[i].scale(1.1, 1.1);
    }
    woodCreaturesSprite[8].scale(0.115, 0.115); // Further scale down tiger sprite because texture is too big
    
    /////////////////////  Setting up texts Section  ////////////////////
    // Upload font
    font.loadFromFile("assets/KOMIKAP_.ttf");
    
    // Make score text
    scorePhrase = "Score: ";
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(1700, 20);
    
    // Make Title text
    titleText.setFont(font);
    titleText.setString("Buzzy's Revenge");
    titleText.setCharacterSize(75);
    titleText.setFillColor(sf::Color::Red);
    textRect = titleText.getLocalBounds();
    titleText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    titleText.setPosition(960, 250);
    
    // Make Author text
    authorText.setFont(font);
    authorText.setString("        Created by:\n Sebastian F. Echeandia");
    authorText.setCharacterSize(30);
    authorText.setFillColor(sf::Color::White);
    textRect = authorText.getLocalBounds();
    authorText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    authorText.setPosition(960, 800);
    
    // Make lives text
    livesText.setFont(font);
    livesText.setString("Lives");
    livesText.setCharacterSize(30);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(20, 30);
    
    // Make Power text
    powerText.setFont(font);
    powerText.setString("Power");
    powerText.setCharacterSize(30);
    powerText.setFillColor(sf::Color::White);
    powerText.setPosition(50, 960);
    
    // Make game over text
    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(75);
    gameOverText.setFillColor(sf::Color::Red);
    textRect = titleText.getLocalBounds();
    gameOverText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    gameOverText.setPosition(960, 250);
    
    // Make Instruction texts
    instructionPhrases.assign({"Press Enter to Restart Game", "Press Esc to Exit", "Press Space to Powerup", "Press up/down arrow to aim"});
    for (int i = 0; i < 4; i++)
    {
        instructionText[i].setFont(font);
        instructionText[i].setString(instructionPhrases[i]);
        instructionText[i].setCharacterSize(60);
        instructionText[i].setFillColor(sf::Color::White);
        textRect = instructionText[i].getLocalBounds();
        instructionText[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        instructionText[i].setPosition(960, 370 + (80 * i));
    }
    
    /////////////////////  Power bar Section  ////////////////////
    // Make power bar
    sf::RectangleShape powerBar;
    powerBar.setSize(sf::Vector2f(400, 50));
    powerBar.setFillColor(sf::Color::Red);
    powerBar.setPosition(200, 950);

    
    /////////////////////  Set up position coordinates for each of the 10 creature spots  ////////////////////
    // Loop over the 10 different spots
    for (int i = 0; i < 10; i++)
    {
        // For positions in the first column
        if(i < 5)
        {
            positionCoordinates.push_back(std::make_pair(1570, 370 + 120 * i));
        }
        // For positions in the second column
        else
        {
            positionCoordinates.push_back(std::make_pair(1720, 370 + 120 * (i - 5)));
        }
    }
    
    /////////////////////  Compute Buzzy's internal geometry to use for colission tracking  ////////////////////
    // To check if buzzy has colided, we will track three points: the top right and bottom right corners as well as the midpoint in the rightmost edge of buzzy
    // Compute half width and height
    buzzyHalfWidth = (buzzySprite.getTexture()->getSize().x * buzzySprite.getScale().x) / 2;
    buzzyHalfHeight = (buzzySprite.getTexture()->getSize().y * buzzySprite.getScale().y) / 2;
    
    // Compute Buzzy's internal angle and hypotenuse
    buzzyInternalAngle = atan(buzzyHalfHeight / buzzyHalfWidth) * 180.0 / 3.14159265;
    buzzyInternalHypotenuse = sqrt(pow(buzzyHalfHeight, 2) + pow(buzzyHalfWidth, 2));
    
    // Establish initial coordinates for the three tracking points
    buzzyTopRight = std::make_pair(50 + buzzyHalfWidth, 540 - buzzyHalfHeight);
    buzzyMidRight = std::make_pair(50 + buzzyHalfWidth, 540);
    buzzyBottomRight = std::make_pair(50 + buzzyHalfWidth, 540 + buzzyHalfHeight);
    
    /////////////////////  Game  ////////////////////
    // Open window
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Buzzy's Revenge");

    // Start the game loop
    while (window.isOpen())
    {
        
        // Check if the user closed the window
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
         
        // Check if the user pressed scape to close window
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
        }
        
        // Check if user pressed return to begin game
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
        {
            paused = false;
            powerBar.setSize(sf::Vector2f(10, 50));
            acceptInput = true;
            
            clock_bee.restart();
        }
        
        // If user is allowed to rotate or power buzzy
        if(acceptInput)
        {
            // If user pressed the space bar start charging the power bar
            if (!powerBarHold && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                clock.restart();
                powerBarHold = true;
                powerBar.setSize(sf::Vector2f(300, 50));
            }
            // If user releases the space bar, stop charging the power bar and release buzzy
            else if (powerBarHold && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                powerBarHold = false;
                acceptInput = false;
                
                v = powerLevel * 25 / maxPower;

                vy = v * sin(buzzyAngle * 3.14159265 / 180.0);
                vx = v * cos(buzzyAngle * 3.14159265 / 180.0);
                
                clock.restart();
            }
            
            // If up key is pressed, rotate buzzy counterclockwise
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                buzzyAngle-=1;
                buzzySprite.setRotation(buzzyAngle);
            }
            // If down key is pressed, rotate buzzy clockwise
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                buzzyAngle+=1;
                buzzySprite.setRotation(buzzyAngle);
            }
             
        }
        
        // If the space bar is being held, increase the length of power bar
        if (powerBarHold)
        {
            t = clock.getElapsedTime();
            powerLevel = 10 + powerPerSecond * t.asSeconds();
            powerBar.setSize(sf::Vector2f(powerLevel, 50));
            
            // If power exceeds max power reset it to equal max power
            if (powerLevel > maxPower)
            {
                powerBar.setSize(sf::Vector2f(maxPower, 50));
                powerLevel = maxPower;
            }
        }
        
        // If the game is not paused, simulate the bee
        if(!paused)
        {
            // If the bee left the screen, reset the position of the bee on the right side
            if(beeSprite.getPosition().x < 0)
            {
                beeSprite.setPosition(1900, 150);
            }
            
            // compute position of the bee based on elapsed time
            // To randomize the vertical path we use a random number generator. If the number is less than 50 the bee moves down, else if moves up
            t_bee = clock_bee.getElapsedTime();
            if ((rand() % 100) < 50)
            {
                beeSprite.setPosition(beeSprite.getPosition().x - t_bee.asSeconds() * 150, beeSprite.getPosition().y + 200 * t_bee.asSeconds());
            }
            else
            {
                beeSprite.setPosition(beeSprite.getPosition().x - t_bee.asSeconds() * 150, beeSprite.getPosition().y - 200 * t_bee.asSeconds());
            }
            clock_bee.restart();
            beeRect = beeSprite.getGlobalBounds(); // update bee rect with new position
        }
        
        // This condition indicates that buzzy has launched. Compute buzzy's parabolic motion
        if(!paused && !acceptInput)
        {
            // Equations of motion for buzzy
            t = clock.getElapsedTime();
            y += vy * t.asSeconds() + 0.5 * ay * t.asSeconds() * t.asSeconds();
            vy += ay * t.asSeconds();
            x += vx * t.asSeconds();
            
            // Compute buzzy's angle to be tangent to path
            buzzyAngle = atan(vy / vx) * 180.0 / 3.14159265;
            
            // Check if arctangent reached the asymptote. limit to 90 if it did. Multiply by vy/abs(vy) to get proper sign for the angle
            if (abs(buzzyAngle) > 90)
            {
                buzzyAngle = 90 * vy / abs(vy);
            }
            
            // Update buzzy's position and angle
            buzzySprite.setPosition(x, y);
            buzzySprite.setRotation(buzzyAngle);
            
            // Update collision detection points for buzzy
            buzzyMidRight.first = buzzyHalfWidth * cos(buzzyAngle * 3.14159265 / 180.0) + x;
            buzzyMidRight.second = buzzyHalfWidth * sin(buzzyAngle * 3.14159265 / 180.0) + y;
            buzzyTopRight.first = buzzyInternalHypotenuse * cos((buzzyAngle - buzzyInternalAngle) * 3.14159265 / 180.0) + x;
            buzzyTopRight.second = buzzyInternalHypotenuse * sin((buzzyAngle - buzzyInternalAngle) * 3.14159265 / 180.0) + y;
            buzzyBottomRight.first = buzzyInternalHypotenuse * cos((buzzyAngle + buzzyInternalAngle) * 3.14159265 / 180.0) + x;
            buzzyBottomRight.second = buzzyInternalHypotenuse * sin((buzzyAngle + buzzyInternalAngle) * 3.14159265 / 180.0) + y;
            
        }
        
        // When the level has to restart
        if (levelRestart)
        {
            // This vector tracks which positions are already taken
            std::vector<bool> positionTaken(10, false);
            
            // Put Buzzy on initial location at 0 angle.
            buzzySprite.setPosition(50, 540);
            buzzySprite.setRotation(0);
            y = 540;
            x = 50;
            buzzyAngle = 0;
            
            // Restart bee position
            beeSprite.setPosition(1620, 150);
            
            // Pick position for tiger. Any random position will do, then assign it to the tiger sprite
            positionNumber[8] = rand() % 10;
            woodCreaturesSprite[8].setPosition(positionCoordinates.at(positionNumber[8]).first, positionCoordinates.at(positionNumber[8]).second);
            positionTaken[positionNumber[8]] = true;
            woodlandCreatureRect[8] = woodCreaturesSprite[8].getGlobalBounds();
            
            // Now pick position for bulldog making sure it's in different column
            // If tiger was in first column pick a position in second column
            if (positionNumber[8] < 5)
            {
                do
                {
                    positionNumber[9] = rand() % 10;
                }
                while (positionNumber[9] < 5);
            }
            // If tiger was in second column pick a position in first column
            else
            {
                do
                {
                    positionNumber[9] = rand() % 10;
                }
                while (positionNumber[9] > 5);
            }
            
            // Assign the position to the bulldog sprite and update its rectangle position
            woodCreaturesSprite[9].setPosition(positionCoordinates.at(positionNumber[9]).first, positionCoordinates.at(positionNumber[9]).second);
            woodlandCreatureRect[9] = woodCreaturesSprite[9].getGlobalBounds();
            
            positionTaken[positionNumber[9]] = true;
            
            // loop for the remaining woodland creatures
            for (int i = 0; i < 8; i++)
            {
                // Pick an open position
                do
                {
                    positionNumber[i] = rand() % 10;
                }
                while (positionTaken[positionNumber[i]]);
                
                // Asign the position to sprite, set corresponding coordinates and update its rectangle's position
                woodCreaturesSprite[i].setPosition(positionCoordinates.at(positionNumber[i]).first, positionCoordinates.at(positionNumber[i]).second);
                positionTaken[positionNumber[i]] = true;
                woodlandCreatureRect[i] = woodCreaturesSprite[i].getGlobalBounds();
            }
            
            levelRestart = false;
        }
        
        // Check if Buzzy is off screan
        if (x < 0 || x > 1920 || y < 0 || y > 1080)
        {
            // Restart Buzzy's position to the initial position
            buzzySprite.setPosition(50, 540);
            buzzySprite.setRotation(0);
            y = 540;
            x = 50;
            buzzyAngle = 0;
            
            // handle state of game variables
            numberLives --;
            acceptInput = true;
        }
        
        // Check for collission with woodland creature
        for (int i = 0; i < 7; i++)
        {
            if ((woodlandCreatureRect[i].contains(buzzyTopRight.first, buzzyTopRight.second) ||
                woodlandCreatureRect[i].contains(buzzyMidRight.first, buzzyMidRight.second) ||
                woodlandCreatureRect[i].contains(buzzyBottomRight.first, buzzyBottomRight.second)) && !colission)
            {
                numberLives --;
                colission = true;
            }
        }
        
        // Check for colission with mascot
        for (int i = 8; i < 10; i++)
        {
            if ((woodlandCreatureRect[i].contains(buzzyTopRight.first, buzzyTopRight.second) ||
                woodlandCreatureRect[i].contains(buzzyMidRight.first, buzzyMidRight.second) ||
                woodlandCreatureRect[i].contains(buzzyBottomRight.first, buzzyBottomRight.second)) && !colission)
            {
                score += 25;
                colission = true;
                mascotsHit ++;
                
                // Move off screen all creatures in the same column as the mascot hit
                for (int j = 0; j < 10; j++)
                {
                    if(positionNumber[i] < 5 && positionNumber[j] < 5)
                    {
                        woodCreaturesSprite[j].setPosition(3000, 3000);
                        woodlandCreatureRect[j] = woodCreaturesSprite[i].getGlobalBounds();
                    }
                    else if(positionNumber[i] >= 5 && positionNumber[j] >= 5)
                    {
                        woodCreaturesSprite[j].setPosition(3000, 3000);
                        woodlandCreatureRect[j] = woodCreaturesSprite[i].getGlobalBounds();
                    }
                }
            }
        }
        
        // Check colission with unicorn
        if ((woodlandCreatureRect[7].contains(buzzyTopRight.first, buzzyTopRight.second) ||
            woodlandCreatureRect[7].contains(buzzyMidRight.first, buzzyMidRight.second) ||
            woodlandCreatureRect[7].contains(buzzyBottomRight.first, buzzyBottomRight.second)) && !colission)
        {
            // Only increase lives if less than 5
            if(numberLives < 5)
            {
                numberLives ++;
            }
            colission = true;
            
            // Remove unicorn from screen
            woodCreaturesSprite[7].setPosition(3000, 3000);
            woodlandCreatureRect[7] = woodCreaturesSprite[7].getGlobalBounds();
            
            // Use the position counter to cycle through all positions above the unicorn
            int positionCounter = positionNumber[7] -1;
            
            // If the unicorn was in the first column, enter this loop
            while (positionCounter >= 0 && positionCounter < 5 && positionNumber[7] != 5)
            {
                // Search for the creature that matches the position counter
                for (int i = 0; i < 10; i++)
                {
                    if (positionNumber[i] == positionCounter)
                    {
                        // Move down creature by increasing its position number and update its position
                        positionNumber[i] ++;
                        woodCreaturesSprite[i].setPosition(positionCoordinates.at(positionNumber[i]).first, positionCoordinates.at(positionNumber[i]).second);
                        woodlandCreatureRect[i] = woodCreaturesSprite[i].getGlobalBounds();
                        
                        // Decrease counter by 1 to find the next creature that has to drop down
                        positionCounter --;
                    }
                }
            }
            // If the unicorn was in the second column, enter this loop
            while (positionCounter >= 5)
            {
                // Search for the creature that matches the position counter
                for (int i = 0; i < 10; i++)
                {
                    if (positionNumber[i] == positionCounter)
                    {
                        // Move down creature by increasing its position number and update its position
                        positionNumber[i] ++;
                        woodCreaturesSprite[i].setPosition(positionCoordinates.at(positionNumber[i]).first, positionCoordinates.at(positionNumber[i]).second);
                        woodlandCreatureRect[i] = woodCreaturesSprite[i].getGlobalBounds();
                        
                        // Decrease counter by 1 to find the next creature that has to drop down
                        positionCounter --;
                    }
                }
            }
        }
        
        // Check colission with Bee
        if ((beeRect.contains(buzzyTopRight.first, buzzyTopRight.second) ||
            beeRect.contains(buzzyMidRight.first, buzzyMidRight.second) ||
            beeRect.contains(buzzyBottomRight.first, buzzyBottomRight.second)) && !colission)
        {
            score += 75;
            colission = true;
            
            // Remove bee from screen
            beeSprite.setPosition(100000, 3000);
        }
        
        // If a colission occurred
        if (colission)
        {
            // Reset Buzzy to initial position
            buzzySprite.setPosition(50, 540);
            buzzySprite.setRotation(0);
            y = 540;
            x = 50;
            buzzyAngle = 0;
            
            // Reset colission tracking coordinates
            buzzyTopRight = std::make_pair(50 + buzzyHalfWidth, 540 - buzzyHalfHeight);
            buzzyMidRight = std::make_pair(50 + buzzyHalfWidth, 540);
            buzzyBottomRight = std::make_pair(50 + buzzyHalfWidth, 540 + buzzyHalfHeight);
            
            // handle state of game variables
            acceptInput = true;
            colission = false;
        }
        
        // If both mascots where hit, restart level
        if (mascotsHit == 2)
        {
            levelRestart = true;
            mascotsHit = 0;
        }
        
        // Check if game is over
        if (numberLives == 0)
        {
            gameOver = true;
            acceptInput = false;
            
            // Reset Buzzy to initial position
            buzzySprite.setPosition(50, 540);
            buzzySprite.setRotation(0);
        }
        
        // Clear screen
        window.clear();

        // Draw background, buzzy and bee sprites
        window.draw(backgroundSprite);
        window.draw(buzzySprite);
        window.draw(beeSprite);
        
        // Draw all woodland creatures
        for (int i = 0; i < 10; i++)
        {
            window.draw(woodCreaturesSprite[i]);
        }
        
        // Draw buzzy's lives
        for (int i = 0; i < numberLives; i++)
        {
            window.draw(buzzLifeSprite[i]);
        }
        
        // If paused, draw instructions and title
        if(paused)
        {
            // Draw the texts
            window.draw(titleText);
            window.draw(authorText);
            for (int i = 0; i < 4; i++)
            {
                window.draw(instructionText[i]);
            }
        }
        
        // If the game is over, display game over text
        if (gameOver)
        {
            window.draw(gameOverText);
        }
    
        // Update score string
        scorePhrase = "Score: ";
        scoreText.setString(scorePhrase += std::to_string(score));
        
        // Draw score, lives and power texts
        window.draw(scoreText);
        window.draw(livesText);
        window.draw(powerText);
        
        // Draw power bar
        window.draw(powerBar);

        // Update the window
        window.display();
    }

    return 0;
}
