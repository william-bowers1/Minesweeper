#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
using namespace std;
random_device rd;
mt19937 gen(rd());


struct Board {
    int numCols;
    int numRows;
    int numMines;

    int Width;
    int Height;
    int tileCount;
    int tilesToReveal;

    struct Tile {
        bool isMine = false;
        bool isRevealed = false;
        bool isFlagged = false;
        bool visited = false;
        bool beenCounted = false;
        int adjacentMines = 0;
        sf::Sprite sprite;
    };

    vector<vector<Tile>> Tiles;

    Board(int _numCols, int _numRows, int _numMines) {
        this->numCols = _numCols;
        this->numRows = _numRows;
        this->numMines = _numMines;
        this->Width = _numCols * 32;
        this->Height = (_numRows * 32) + 100;
        this->tileCount = _numCols * _numRows;
        this->tilesToReveal = (_numCols * _numRows) - _numMines;
        Tiles.resize(numRows, vector<Tile>(numCols));
        
    }


    
    

};

void addNumbers(Board& board) {
    for (int row = 0; row < board.numRows; row++) {
        for (int col = 0; col < board.numCols; col++) {
            if (board.Tiles[row][col].isMine) {
                // Increment the adjacentMines property of all neighboring tiles
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        if (i == 0 && j == 0) continue; // Skip the current tile
                        int r_index = row + i;
                        int c_index = col + j;
                        if (r_index >= 0 && r_index < board.numRows && c_index >= 0 && c_index < board.numCols) {
                            board.Tiles[r_index][c_index].adjacentMines++;
                        }
                    }
                }
            }
        }
    }
}

void recursiveIsRevealed(Board& Board, int row, int col) {
    if (!Board.Tiles[row][col].visited && !Board.Tiles[row][col].isMine && !Board.Tiles[row][col].isFlagged) {
        Board.Tiles[row][col].isRevealed = true;
        Board.Tiles[row][col].visited = true;
        if (Board.Tiles[row][col].adjacentMines == 0) {
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0) continue;
                    int r_index = row + i;
                    int c_index = col + j;
                    if (r_index >= 0 && r_index < Board.numRows && c_index >= 0 && c_index < Board.numCols) {
                        recursiveIsRevealed(Board, r_index, c_index);
                    }
                }
            }
        }
    }


}

void setText(sf::Text& text, float x, float y) {
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    text.setPosition(sf::Vector2f(x, y));
}
int Random(int min, int max) {
    uniform_int_distribution<int> dist(min, max);
    return dist(rd);
}

void restartGame(Board& board) {
    for (int row = 0; row < board.numRows; row++) {
        for (int col = 0; col < board.numCols; col++) {
            board.Tiles[row][col].isMine = false;
            board.Tiles[row][col].isRevealed = false;
            board.Tiles[row][col].isFlagged = false;
            board.Tiles[row][col].visited = false;
            board.Tiles[row][col].adjacentMines = 0;
            board.Tiles[row][col].beenCounted = false;

        }
    }

    board.tilesToReveal = board.tileCount - board.numMines;


    int minesCount = 0;
    while (minesCount < board.numMines) {
        int row = Random(0, board.numRows - 1);
        int col = Random(0, board.numCols - 1);
        if (!board.Tiles[row][col].isMine) {
            board.Tiles[row][col].isMine = true;
            minesCount++;
        }
    }
}

int main()
{
    int cols = 0;
    int rows = 0;
    int mines = 0;
    ifstream configFile("files/board_config.cfg");
    if (configFile.is_open()) {
        string line;
        getline(configFile, line);
        cols = stoi(line);
        getline(configFile, line);
        rows = stoi(line);
        getline(configFile, line);
        mines = stoi(line);
        configFile.close();

    }
    else {
        cout << "Error reading file" << endl;
    }

    Board board(cols, rows, mines);

    sf::RenderWindow welcomeWindow(sf::VideoMode(board.Width, board.Height), "Welcome Window");


    sf::Font font; // loading font
    font.loadFromFile("files/font.ttf");


    // welcome text
    sf::Text welcomeText("WELCOME TO MINESWEEPER!", font, 24);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    welcomeText.setFillColor(sf::Color::White);
    setText(welcomeText, welcomeWindow.getSize().x / 2.0f, welcomeWindow.getSize().y / 2.0f - 150);

    // input prompt text
    sf::Text inputPromptText("Enter your name:", font, 20);
    inputPromptText.setStyle(sf::Text::Bold);
    inputPromptText.setFillColor(sf::Color::White);
    setText(inputPromptText, welcomeWindow.getSize().x / 2.0f, welcomeWindow.getSize().y / 2.0f - 75);

    // input text
    sf::Text inputText("", font, 18);
    inputText.setStyle(sf::Text::Bold);
    inputText.setFillColor(sf::Color::Yellow);
    setText(inputText, welcomeWindow.getSize().x / 2.0f, welcomeWindow.getSize().y / 2.0f - 45);

    // cursor text
    sf::Text cursorText("|", font, 18);
    cursorText.setStyle(sf::Text::Bold);
    cursorText.setFillColor(sf::Color::Yellow);
    setText(cursorText, welcomeWindow.getSize().x / 2.0f, welcomeWindow.getSize().y / 2.0f - 45);

    welcomeWindow.clear(sf::Color::Blue);
    welcomeWindow.draw(welcomeText);
    welcomeWindow.draw(inputPromptText);
    welcomeWindow.display();


    sf::Texture mineTexture;
    mineTexture.loadFromFile("files/images/mine.png");
    sf::Texture hiddenTexture;
    hiddenTexture.loadFromFile("files/images/tile_hidden.png");
    sf::Texture revealedTexture;
    revealedTexture.loadFromFile("files/images/tile_revealed.png");
    sf::Texture happyTexture;
    happyTexture.loadFromFile("files/images/face_happy.png");
    sf::Texture winTexture;
    winTexture.loadFromFile("files/images/face_win.png");
    sf::Texture loseTexture;
    loseTexture.loadFromFile("files/images/face_lose.png");
    sf::Texture digitsTexture;
    digitsTexture.loadFromFile("files/images/digits.png");
    sf::Texture debugTexture;
    debugTexture.loadFromFile("files/images/debug.png");
    sf::Texture pauseTexture;
    pauseTexture.loadFromFile("files/images/pause.png");
    sf::Texture playTexture;
    playTexture.loadFromFile("files/images/play.png");
    sf::Texture leaderboardTexture;
    leaderboardTexture.loadFromFile("files/images/leaderboard.png");
    sf::Texture flagTexture;
    flagTexture.loadFromFile("files/images/flag.png");


    sf::Text mainText("LEADERBOARD", font, 20);
    mainText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    mainText.setFillColor(sf::Color::White);

    

    bool newRecord = false;
    int playerRank = -1;

        // Create a sprite for the digits image
    sf::Sprite digitsSprite;
    digitsSprite.setTexture(digitsTexture);

    sf::Sprite digits[10];
    for (int i = 0; i < 10; i++) {
        digits[i].setTexture(digitsTexture);
        digits[i].setTextureRect(sf::IntRect(i * 21, 0, 21, 32));
    }

    int minutes = 0;
    int seconds = 0;
    sf::Clock clock;

    string username = "";

    // main loop
    while (welcomeWindow.isOpen())
    {
        // handle events
        sf::Event event;
        while (welcomeWindow.pollEvent(event))
        {

            if (event.type == sf::Event::Closed) {
                welcomeWindow.close();
                return 0;
            }

            // handle text input
            if (event.type == sf::Event::TextEntered) {
                char inputChar = static_cast<char>(event.text.unicode);
                // validates input is a letter and name is less than 10 chars
                if (isalpha(inputChar) && inputText.getString().getSize() < 10)
                {
                    inputText.setString(inputText.getString() + inputChar);

                }
                // handle backspace
                if (event.text.unicode == '\b' && inputText.getString().getSize() > 0)
                {
                    inputText.setString(inputText.getString().substring(0, inputText.getString().getSize() - 1));
                    setText(inputText, welcomeWindow.getSize().x / 2.0f, welcomeWindow.getSize().y / 2.0f - 45);
                }
                // capitalize first letter and make other letters lowercase
                std::string name = inputText.getString();
                if (!name.empty())
                {
                    name[0] = toupper(name[0]);
                    for (int i = 1; i < name.length(); i++)
                    {
                        name[i] = tolower(name[i]);
                    }
                }
                inputText.setString(name);


                // handle enter key
                if (event.text.unicode == 13 && inputText.getString().getSize() > 0)
                {
                    username = inputText.getString();
                    welcomeWindow.close();
                    break;
                }
            }
            welcomeWindow.clear(sf::Color::Blue);
            welcomeWindow.draw(welcomeText);
            welcomeWindow.draw(inputPromptText);

            setText(inputText, welcomeWindow.getSize().x / 2.0f, welcomeWindow.getSize().y / 2.0f - 45);
            welcomeWindow.draw(inputText);

            setText(cursorText, inputText.getGlobalBounds().left + inputText.getGlobalBounds().width, welcomeWindow.getSize().y / 2.0f - 45);
            welcomeWindow.draw(cursorText);
            welcomeWindow.display();
        }


    }







    




    

    float counterStartX = 33.0;
    float counterStartY = (32 * (board.numRows + 0.5f) + 16);
    
    for (int row = 0; row < board.numRows; row++) {
        for (int col = 0; col < board.numCols; col++) {
            board.Tiles[row][col].sprite.setTexture(hiddenTexture);
            board.Tiles[row][col].sprite.setPosition(sf::Vector2f(col * 32, row * 32));
        }
    }

    int minesCount = 0;
    while (minesCount < board.numMines) {
        int row = Random(0, board.numRows - 1);
        int col = Random(0, board.numCols - 1);
        if (!board.Tiles[row][col].isMine) {
            board.Tiles[row][col].isMine = true;
            minesCount++;
        }
    }

    sf::Sprite happyButton;
    happyButton.setTexture(happyTexture);
    happyButton.setPosition(((board.numCols / 2.0) * 32) - 32, 32 * (board.numRows + 0.5f));
    
    sf::Sprite debugButton;
    debugButton.setTexture(debugTexture);
    debugButton.setPosition((board.numCols * 32) - 304, 32 * (board.numRows + 0.5f));

    sf::Sprite pauseButton;
    pauseButton.setTexture(pauseTexture);
    pauseButton.setPosition((board.numCols * 32) - 240, 32 * (board.numRows + 0.5f));

    sf::Sprite leaderboardButton;
    leaderboardButton.setTexture(leaderboardTexture);
    leaderboardButton.setPosition((board.numCols * 32) - 176, 32 * (board.numRows + 0.5f));

    sf::Sprite mineSprite;
    mineSprite.setTexture(mineTexture);

    sf::Sprite flagSprite;
    flagSprite.setTexture(flagTexture);

    sf::Sprite tempRevealed;
    tempRevealed.setTexture(revealedTexture);

    sf::Sprite gameOverMines;
    gameOverMines.setTexture(mineTexture);

    sf::Sprite gameWonFlags;
    gameWonFlags.setTexture(flagTexture);


    

    sf::RenderWindow gameWindow(sf::VideoMode(board.Width, board.Height), "Game Window");
    addNumbers(board);

    bool isPaused = false;
    bool gameOver = false;
    bool debugMode = false;
    bool userLost = false;
    bool userWon = false;
    int revealedCount = 0;

    int flagCount = board.numMines;

    
    ifstream file("files/leaderboard.txt");
    vector<string> leaderboard;
    string line;
    while (getline(file, line)) {
        leaderboard.push_back(line);
    }
    file.close();

    
    while (gameWindow.isOpen()) {
        if (!isPaused && !gameOver) {
            sf::Time elapsed = clock.getElapsedTime();
            if (elapsed.asSeconds() >= 1.0f) {
                seconds++;
                if (seconds >= 60) {
                    minutes++;
                    seconds = 0;
                }
                clock.restart();
            }
        }

        
        gameWindow.clear(sf::Color::White);
        for (int row = 0; row < board.numRows; row++) {
            for (int col = 0; col < board.numCols; col++) {
                gameWindow.draw(board.Tiles[row][col].sprite);
                if (board.Tiles[row][col].isFlagged) {
                    flagSprite.setPosition(board.Tiles[row][col].sprite.getPosition());
                    gameWindow.draw(flagSprite);
                }
                if (board.Tiles[row][col].isMine && board.Tiles[row][col].isRevealed) {
                    // Draw the mine texture on top of the tile sprite
                    sf::Sprite mineSprite(mineTexture);
                    mineSprite.setPosition(board.Tiles[row][col].sprite.getPosition());
                    gameWindow.draw(mineSprite);
                }
                else if (board.Tiles[row][col].isRevealed && board.Tiles[row][col].adjacentMines != 0) {
                    board.Tiles[row][col].sprite.setTexture(revealedTexture);
                    string num = to_string(board.Tiles[row][col].adjacentMines);
                    sf::Texture numTexture;
                    numTexture.loadFromFile("files/images/number_" + num + ".png");
                    sf::Sprite numSprite;
                    numSprite.setTexture(numTexture);
                    numSprite.setPosition(board.Tiles[row][col].sprite.getPosition());
                    gameWindow.draw(numSprite);
                }
                else if (board.Tiles[row][col].isRevealed) {
                    recursiveIsRevealed(board, row, col);
                    board.Tiles[row][col].sprite.setTexture(revealedTexture);
                }
            }
        }

        int minute1 = minutes / 10;
        int minute2 = minutes % 10;
        sf::Vector2f minutePos(((gameWindow.getSize().x) - 97), 32 * (board.numRows + 0.5f) + 16);
        digits[minute1].setPosition(minutePos);
        gameWindow.draw(digits[minute1]);
        digits[minute2].setPosition(minutePos + sf::Vector2f(21, 0));
        gameWindow.draw(digits[minute2]);

        // Draw seconds
        int second1 = seconds / 10;
        int second2 = seconds % 10;
        sf::Vector2f secondPos(((gameWindow.getSize().x) - 54), 32 * (board.numRows + 0.5f) + 16);
        digits[second1].setPosition(secondPos);
        gameWindow.draw(digits[second1]);
        digits[second2].setPosition(secondPos + sf::Vector2f(21, 0));
        gameWindow.draw(digits[second2]);

        gameWindow.draw(happyButton);
        gameWindow.draw(debugButton);
        gameWindow.draw(pauseButton);
        gameWindow.draw(leaderboardButton);

        if (flagCount < 0) {
            digitsSprite.setTextureRect(sf::IntRect(210, 0, 21, 32));
            digitsSprite.setPosition(12, counterStartY);
            gameWindow.draw(digitsSprite);
        }

        std::string flagCountStr = std::to_string(abs(flagCount));

        for (char digit : flagCountStr) {
            // cout << counterStartX << " " << counterStartY << endl;
            int digitInt = digit - '0';
            sf::IntRect digitRect(digitInt * 21, 0, 21, 32);
            digitsSprite.setTextureRect(digitRect);
            digitsSprite.setPosition(counterStartX, counterStartY);
            gameWindow.draw(digitsSprite);
            counterStartX += 21;
        }

        counterStartX = 33;
        
        sf::Event event;
        while (gameWindow.pollEvent(event))
        {

            if (event.type == sf::Event::Closed) {
                gameWindow.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                for (int row = 0; row < board.numRows; row++) {
                    for (int col = 0; col < board.numCols; col++) {
                        sf::FloatRect tileBounds = board.Tiles[row][col].sprite.getGlobalBounds();
                        if (tileBounds.contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y) && !isPaused && !gameOver) { // clicked on tile

                            if (event.mouseButton.button == sf::Mouse::Right && !board.Tiles[row][col].isRevealed) {
                                if (!board.Tiles[row][col].isFlagged) {
                                    flagCount--;
                                    // cout << flagCount << endl;
                                    board.Tiles[row][col].isFlagged = true;
                                    }
                                else {
                                    flagCount++;
                                    // cout << flagCount << endl;
                                    board.Tiles[row][col].isFlagged = false;
                                }
                            }
                            else if (!board.Tiles[row][col].isFlagged && !board.Tiles[row][col].isRevealed) {
                                board.Tiles[row][col].sprite.setTexture(revealedTexture);
                                board.Tiles[row][col].isRevealed = true;
                                // cout << board.tilesToReveal << "normal" << endl;
                            }
                        }
                        if (tileBounds.contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y) && board.Tiles[row][col].isMine && !isPaused
                            && !board.Tiles[row][col].isFlagged && event.mouseButton.button != sf::Mouse::Right) {
                            // GAME OVER
                            userLost = true;
                            gameOver = true;
                            happyButton.setTexture(loseTexture);
                            

                        }
                    }
                }

                sf::FloatRect debugBounds = debugButton.getGlobalBounds();
                if (debugBounds.contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y) && !isPaused && !gameOver) {
                    for (int row = 0; row < board.numRows; row++) {
                        for (int col = 0; col < board.numCols; col++) {
                            if (board.Tiles[row][col].isMine) {
                                board.Tiles[row][col].isRevealed = true;
                                debugMode = true;
                                sf::Sprite mineSprite(mineTexture);
                                mineSprite.setPosition(board.Tiles[row][col].sprite.getPosition());
                                gameWindow.draw(mineSprite);
                                
                            }
                        }
                    }
                }
                
                sf::FloatRect restartBounds = happyButton.getGlobalBounds();
                if (restartBounds.contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y)) {
                    restartGame(board);
                    for (int row = 0; row < board.numRows; row++) {
                        for (int col = 0; col < board.numCols; col++) {
                            board.Tiles[row][col].sprite.setTexture(hiddenTexture);
                        }
                    }
                    addNumbers(board);
                    happyButton.setTexture(happyTexture);
                    gameOver = false;
                    userLost = false;
                    userWon = false;
                    clock.restart();
                    minutes = 0;
                    seconds = 0;
                    flagCount = board.numMines;
                    board.tilesToReveal = board.tileCount - board.numMines;

                    isPaused = false;
                    bool debugMode = false;
                    int revealedCount = 0;

                    int flagCount = board.numMines;
                }

                // FIX
                sf::FloatRect pauseBounds = pauseButton.getGlobalBounds();
                if (pauseBounds.contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y) && !gameOver) {
                    pauseButton.setTexture(playTexture);
                    while (gameWindow.pollEvent(event)) {
                        if (restartBounds.contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y)) {
                            restartGame(board);
                            for (int row = 0; row < board.numRows; row++) {
                                for (int col = 0; col < board.numCols; col++) {
                                    board.Tiles[row][col].sprite.setTexture(hiddenTexture);
                                }
                            }
                            addNumbers(board);
                            happyButton.setTexture(happyTexture);
                            gameOver = false;
                            userLost = false;
                            userWon = false;
                            clock.restart();
                            minutes = 0;
                            seconds = 0;
                            flagCount = board.numMines;
                            board.tilesToReveal = board.tileCount - board.numMines;
                            bool debugMode = false;
                            int revealedCount = 0;
                        }
                    }
                        if (pauseBounds.contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y)) {
                            isPaused = !isPaused;
                            if (isPaused) {
                                pauseButton.setTexture(playTexture);
                            }
                            else {
                                pauseButton.setTexture(pauseTexture);
                            }
                        }
                    
                }


                //LEADERBOARD
                sf::FloatRect leaderboardBounds = leaderboardButton.getGlobalBounds();
                if (leaderboardBounds.contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y)) {
                    
                    sf::RenderWindow leaderboardWindow(sf::VideoMode(board.Width / 2.0, board.Height / 2.0), "Leaderboard Window");

                    ifstream file("files/leaderboard.txt");
                    vector<string> leaderboard;
                    string line;
                    while (getline(file, line)) {
                        leaderboard.push_back(line);
                    }
                    file.close();
                    
                    setText(mainText, leaderboardWindow.getSize().x / 2.0f, leaderboardWindow.getSize().y / 2.0f - 120);
                    
                    ostringstream leaderboardText;
                    for (int i = 0; i < 5; i++) {
                        istringstream ss(leaderboard[i]);
                        string time;
                        string name;
                        getline(ss, time, ',');
                        getline(ss, name);
                        leaderboardText << i + 1 << ".\t" << time << "\t" << name;
                        if (i == playerRank) {
                            leaderboardText << "*";
                        }
                        leaderboardText << "\n\n";
                    }

                    string leaderString = leaderboardText.str();
                    sf::Text leaderText(leaderString, font, 18);
                    leaderText.setFillColor(sf::Color::White);
                    leaderText.setStyle(sf::Text::Bold);
                    setText(leaderText, leaderboardWindow.getSize().x / 2.0f, leaderboardWindow.getSize().y / 2.0f + 20);

                    while (leaderboardWindow.isOpen()) {
                        for (int row = 0; row < board.numRows; row++) {
                            for (int col = 0; col < board.numCols; col++) {
                                tempRevealed.setPosition(board.Tiles[row][col].sprite.getPosition());
                                gameWindow.draw(tempRevealed);
                            }
                        }
                        gameWindow.display();
                        leaderboardWindow.clear(sf::Color::Blue);
                        leaderboardWindow.draw(mainText);
                        leaderboardWindow.draw(leaderText);
                        leaderboardWindow.display();
                        while (leaderboardWindow.pollEvent(event))
                        {

                            if (event.type == sf::Event::Closed) {
                                
                                leaderboardWindow.close();
                            }
                            leaderboardWindow.clear(sf::Color::Blue);
                            leaderboardWindow.draw(mainText);
                            leaderboardWindow.draw(leaderText);
                            leaderboardWindow.display();
                        }
                    }
                }
            }
        }

        

        if (isPaused) {
            for (int row = 0; row < board.numRows; row++) {
                for (int col = 0; col < board.numCols; col++) {
                    tempRevealed.setPosition(board.Tiles[row][col].sprite.getPosition());
                    gameWindow.draw(tempRevealed);
                }
            }
        }

        if (gameOver && userLost) {
            for (int row = 0; row < board.numRows; row++) {
                for (int col = 0; col < board.numCols; col++) {
                    if (board.Tiles[row][col].isMine) {
                        gameOverMines.setPosition(board.Tiles[row][col].sprite.getPosition());
                        gameWindow.draw(gameOverMines);
                    }
                }
            }
            debugMode = false;
            revealedCount = 0;
        }

        if (gameOver && userWon) {
            for (int row = 0; row < board.numRows; row++) {
                for (int col = 0; col < board.numCols; col++) {
                    if (board.Tiles[row][col].isMine) {
                        gameWonFlags.setPosition(board.Tiles[row][col].sprite.getPosition());
                        gameWindow.draw(gameWonFlags);
                        flagCount = 0;
                    }
                }
            }
        }

        for (int row = 0; row < board.numRows; row++) {
            for (int col = 0; col < board.numCols; col++) {
                if (board.Tiles[row][col].isRevealed && !board.Tiles[row][col].beenCounted) {
                    revealedCount++;
                    // cout << revealedCount << ", " << board.tilesToReveal << endl;
                    board.Tiles[row][col].beenCounted = true;

                }
            }
        }


        if (!debugMode && board.tilesToReveal == revealedCount) {
            // cout << "game won!" << endl;
            gameOver = true;
            happyButton.setTexture(winTexture);
            debugMode = false;
            revealedCount = 0;
            userWon = true;

        }
        else if (debugMode && board.tilesToReveal == revealedCount - board.numMines) {
            // cout << "game won!" << endl;
            gameOver = true;
            happyButton.setTexture(winTexture);
            debugMode = false;
            revealedCount = 0;
            userWon = true;
        }

        
        if (userWon) {
            bool userWritten = false;
            ofstream outFile("files/leaderboard.txt");
            for (int i = 0; i < 5; i++) {
                istringstream lineStream(leaderboard[i]);
                string minutesString;
                string secondsString;
                string name;
                getline(lineStream, minutesString, ':');
                getline(lineStream, secondsString, ',');
                getline(lineStream, name);

                
                int totalSeconds = (stoi(minutesString) * 60) + stoi(secondsString);

                int userSeconds = (minutes * 60) + seconds;
                if (!userWritten && userSeconds < totalSeconds) {
                    string _userString = to_string(minute1) + to_string(minute2) + ":" + to_string(second1) + to_string(second2) + "," + username;
                    outFile << _userString << endl;
                    userWritten = true;
                    playerRank = i;
                }
                
                outFile << leaderboard[i] << endl;


            }
            
            outFile.close();
            
            ifstream file("files/leaderboard.txt");
            vector<string> leaderboard;
            string line;
            while (getline(file, line)) {
                leaderboard.push_back(line);
            }
            file.close();

        }
        gameWindow.display();

    }



    

    

    return 0;
}




