#include "SDL.h" // Windows
#include "SDL_ttf.h" // Windows



#include <iostream>
#include <time.h> // Random seed
#include <vector>
#include <sstream>

using namespace std;

class Figure {
public:
    int row;
    int column;

    // Придает фигуре случайную форму и цвет
    void reset() {
        // Установленное положение
        row = 21;
        column = 3;
        // Выберите случайную форму
        type = rand() % figures.size();
        // Выберите цвет
        colour = type;
        rotation = 0;
    }

    // Вернуть тип фигуры и поворот
    vector<int> state() {
        return figures[type][rotation];
    }

    // Возвращает значения rgb рисунка
    vector<int>getColour() {
        return colours[colour];
    }


    //Повернуть фигуру к предыдущему повороту в массиве
    void rotateRight() {
        rotation--;
        if (rotation < 0) {
            rotation = figures[type].size() - 1;
        }
    }

    // Повернуть фигуру к следующему повороту в массиве
    void rotateLeft() {
        rotation = (rotation + 1) % figures[type].size();
    }

    void resetRotation() {
        rotation = 0;
    }

private:
    int type;
    int rotation;
    int colour;

    /*Использование векторов для легкого возврата и получения размера массивов
    Вектор всех фигур и их поворотов в матрице
    * 0  1  2  3
    * 4  5  6  7
    * 8  9  10 11
    * 12 13 14 15
    */
    vector<vector<vector<int>>> figures =
    {
        {{4, 5, 6, 7}, {1, 5, 9, 13}}, // I
        {{4, 5, 9, 10}, {2, 6, 5, 9}}, // Z
        {{6, 7, 9, 10}, {1, 5, 6, 10}}, // S
        {{1, 2, 5, 9}, {0, 4, 5, 6}, {1, 5, 9, 8}, {4, 5, 6, 10}}, // J
        {{1, 2, 6, 10}, {5, 6, 7, 9}, {1, 5, 9, 10}, {3, 5, 6, 7}}, // L
        {{1, 4, 5, 6}, {1, 4, 5, 9}, {4, 5, 6, 9}, {1, 5, 6, 9}}, // T
        {{1, 2, 5, 6}} // O
    };

    vector<vector<int>> colours =
    {
        {33, 235, 225}, // Cyan
        {235, 33, 33}, // Red
        {27, 196, 47}, // Green
        {30, 67, 214}, // blue
        {224, 139, 34}, // Orange
        {164, 34, 224}, //Purple
        {255, 192, 203} //Yellow
    };

};


class Tetris {
public:
    int error = 1;


    // Конструктор создает экран и настраивает игру
    Tetris(const char* title, int xpos, int ypos, int width, int height, int gridSize) : shape() {
        // Настройка SDL
        // Инициализировать библиотеку SDL2
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            cout << "Error Initializing SDL: " << SDL_GetError() << endl;
            return;
        }
        cout << "SDL Initialized\n";

        // Инициализировать библиотеку шрифтов
        if (TTF_Init() == -1) {
            cout << "Error Initializing SDL_TTF\n";
            return;
        }
        cout << "SDL_TTF Initialized\n";

        infoFont = getFont("ALGER", 35);
        if (!infoFont) {
            cout << "Error opening font ALGER1.ttf";
            return;
        }

        titleFont = getFont("COLONNA", 150);
        if (!titleFont) {
            cout << "Error opening font ALGER2.ttf";
            return;
        }

        textFont = getFont("ALGER", 25);
        if (!textFont) {
            cout << "Error opening font ALGER3.ttf";
            return;
        }



        // Создайте окно для рисования 
        GRIDSIZE = gridSize;
        SCREEN_WIDTH = width;
        SCREEN_HEIGHT = height;
        GRID_WIDTH = GRIDSIZE * 10;
        GRID_HEIGHT = GRIDSIZE * 20;
        OFFSET = (width / 2) - (GRIDSIZE * 5); // Xpos of grid start
        window = SDL_CreateWindow(title, xpos, ypos, width, height, 0);
        if (!window) {
            cout << "Error Initializing Window: " << SDL_GetError() << endl;
            return;
        }
        cout << "Window Created\n";

        // Создайте средство визуализации для рисования в окне
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (!renderer) {
            cout << "Error Initializing Renderer: " << SDL_GetError() << endl;
            return;
        }
        cout << "Renderer Created\n";
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        error = 0; // Ошибки нет
        // Настройка игры
        reset();
    }

    // Проверьте вводимые пользователем данные
    void handleEvents() {
        // Проверьте наличие какого-то действия
        SDL_Event event;
        SDL_PollEvent(&event);

        switch (event.type) {
            // Нажата кнопка выхода
        case SDL_QUIT:
            state = "quit";
            break;

            // Клавиша была нажата
        case SDL_KEYDOWN:
            if (state == "start") {
                switch (event.key.keysym.sym) {
                case SDLK_SPACE: // Начать игру
                    state = "play";
                    break;

                case SDLK_h: // Открыть экран справки
                    state = "help";
                    break;

                case SDLK_ESCAPE: // Выйти из игры
                    state = "quit";
                    break;

                default:
                    break;
                }
            }

            else if (state == "help") {
                switch (event.key.keysym.sym) {
                case SDLK_h: // Вернитесь к началу
                    state = "start";
                    break;

                case SDLK_ESCAPE: // Выйти из игры
                    state = "quit";
                    break;

                default:
                    break;
                }
            }

            else if (state == "play") {
                switch (event.key.keysym.sym) {
                case SDLK_LEFT: // Двигаться влево
                    moveLeft();
                    break;
                case SDLK_RIGHT: // Двигайся вправо
                    moveRight();
                    break;
                case SDLK_x: //Повернуть влево
                    rotateLeft();
                    break;
                case SDLK_UP: // Повернуть направо
                    rotateRight();
                    break;
                case SDLK_DOWN: // Переместитесь вниз на 1
                    softDrop();
                    break;
                case SDLK_SPACE: // Опускайся до упора вниз
                    hardDrop();
                    break;
                case SDLK_c: // Удерживающая деталь в "кеше"
                    hold();
                    break;

                case SDLK_ESCAPE: // Выйти из игры
                    state = "quit";
                    break;

                default:
                    break;
                }
            }

            else if (state == "finish") {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE: // Выйти из игры
                    state = "quit";
                    break;

                case SDLK_r: // Перезапустить игру
                    reset();
                    break;

                default:
                    break;
                }
            }

        default:
            break;
        }
    }

    // Обновить объект фигуры
    void update() {
        if (state == "play") {
            level = (linesCleared / 10) + 1;
            float frameDiff = 16.6666667; //Исходное время между кадрами
            float delay; // время между каждым сбросом (в зависимости от уровня)
            // Установите скорость игры на основе текущего уровня
            if (level >= 0 && level <= 8) {
                delay = 800.0f - (5.f * level * frameDiff); // Ranged between 48-8 frames
            }
            else if (level == 9) {
                delay = frameDiff * 6;
            }
            else if (level >= 10 && level <= 12) {
                delay = frameDiff * 5;
            }
            else if (level >= 13 && level <= 15) {
                delay = frameDiff * 4;
            }
            else if (level >= 16 && level <= 18) {
                delay = frameDiff * 3;
            }
            else if (level >= 19 && level <= 28) {
                delay = frameDiff * 2;
            }
            else {
                delay = frameDiff;
            }

            // Разница во времени с момента последнего снижения фигуры
            Uint32 endMS = SDL_GetTicks();
            float elapsedMS = endMS - startMS;

            if (elapsedMS >= delay) {
                // Двигаться вниз
                shape.row--;
                // Если фигура ударится обо что-нибудь
                if (intersects()) {
                    shape.row++;
                    freeze();
                    breakLines();
                }
                startMS = SDL_GetTicks();
            }
        }

    }

    // Отрисовка следующего фрагмента
    void render() {
        // Set background to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Color red = { 255, 0, 51, 255 };
        SDL_Color titul = { 153, 153, 255, 255 };

        SDL_Color grey = { 255, 255, 255, 100 };

        // Меню
        if (state == "start") {

         
            // Title
            renderText(SCREEN_WIDTH / 2, 50, "TETRIS", titleFont, titul, true);

            //
            renderText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 400, "SPACE TO START", infoFont, grey, true);
            renderText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 200, "H - MENU", textFont, grey, true);
        }

        // Элементы управления/справка
        if (state == "help") {
            // Title
            renderText(SCREEN_WIDTH / 2, 25, "CONTROLS", infoFont, white, true);

            // Управления
            renderText(50, 100, "ESC - Close", textFont, white, false);
            renderText(50, 150, "C - Hold", textFont, white, false);
            renderText(50, 200, "SPACE - Hard Drop", textFont, white, false);
            renderText(50, 250, "X - Rotate Left", textFont, white, false);
            renderText(50, 300, "UP - Rotate Right", textFont, white, false);
            renderText(50, 350, "LEFT - Move Left", textFont, white, false);
            renderText(50, 400, "RIGHT - Move Right", textFont, white, false);
            renderText(50, 450, "DOWN - Soft Drop", textFont, white, false);

            // Как играть
            string HowToPlay = "The goal of Tetris is to try and score as many points as possible by stacking figures in horizontal rows.\nThe game will gradually speed up with the more lines you clear.";
            renderText(SCREEN_WIDTH / 2 - 50, 100, HowToPlay, textFont, white, false, 400);

            //Баллы
            string ScoreValues = "SCORE VALUES\n\nSoft Drop     - 1 X Distance \nHard Drop    - 2 X Distance \nSingle Line  - 100 \nDouble Line - 300 \nTriple Line   - 500 \nTetris (4)      - 800";
            renderText(SCREEN_WIDTH / 2 - 50, 275, ScoreValues, textFont, white, false);

            //Вернуться в меню
            renderText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, "Press H to return to the menu", textFont, grey, true);
        }

        // игра
        else if (state == "play" || state == "finish") {
            // Grid
            vector<vector<int>> coords;

            // Временно добавьте фигуру в сетку для рендеринга
            vector<int> colour = shape.getColour();
            for (auto num : shape.state()) {
                int row = shape.row - (num / 4);
                int column = (num - (num / 4) * 4) + shape.column;
                field[row][column][0] = colour[0];
                field[row][column][1] = colour[1];
                field[row][column][2] = colour[2];
                coords.push_back({ row, column });
            }

            // Нарисуйте сетку
            for (int row = 0; row < 20; row++) {
                for (int column = 0; column < 10; column++) {
                    SDL_Rect rect;
                    rect.x = column * GRIDSIZE + OFFSET;
                    rect.y = SCREEN_HEIGHT - (row * GRIDSIZE) - GRIDSIZE;
                    rect.w = GRIDSIZE;
                    rect.h = GRIDSIZE;

                    // Цвет квадрата
                    SDL_SetRenderDrawColor(renderer, field[row][column][0], field[row][column][1], field[row][column][2], 255);
                    SDL_RenderFillRect(renderer, &rect);
                    // Граница квадрата
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                    SDL_RenderDrawRect(renderer, &rect);

                }
            }

            // Удалить фигуру с поля
            for (auto coord : coords) {
                field[coord[0]][coord[1]][0] = 0;
                field[coord[0]][coord[1]][1] = 0;
                field[coord[0]][coord[1]][2] = 0;
            }
            coords.clear();


            int gridTop = SCREEN_HEIGHT - GRID_HEIGHT;

            //Оценка/уровень/линии - - -
            // Установите размеры границы
            SDL_Rect infoBorder;
            infoBorder.x = OFFSET - 200;
            infoBorder.y = SCREEN_HEIGHT - GRID_HEIGHT / 3 - GRID_HEIGHT / 7;
            infoBorder.w = 175;
            infoBorder.h = SCREEN_HEIGHT - infoBorder.y;

            // Позиции внутри границы
            int xCenter = infoBorder.x + infoBorder.w / 2;
            int yTop = infoBorder.y + 25;
            int yCenter = SCREEN_HEIGHT - infoBorder.h / 2 - 25;
            int yBottom = SCREEN_HEIGHT - 75;

            // Нарисуйте границу
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &infoBorder);

            // Информация о розыгрыше
            renderText(xCenter, yTop, "SCORE", infoFont, white, true);
            renderText(xCenter, yTop + 35, to_string(score), infoFont, white, true);

            renderText(xCenter, yCenter, "LEVEL", infoFont, white, true);
            renderText(xCenter, yCenter + 35, to_string((int)level), infoFont, white, true);

            renderText(xCenter, yBottom, "LINES", infoFont, white, true);
            renderText(xCenter, yBottom + 35, to_string(linesCleared), infoFont, white, true);


            // Следующие фрагменты - - - - - - - - - - - - -
            // Установите размеры границы
            SDL_Rect nextBorder;
            nextBorder.x = OFFSET + GRID_WIDTH + 25; // Right of grid
            nextBorder.y = gridTop + 50;
            nextBorder.w = 4 * (GRIDSIZE + 5) + 30; // Width of nextField grid
            nextBorder.h = 14 * (GRIDSIZE + 5); // Height of nextField grid

            // текст 
            renderText(nextBorder.x + nextBorder.w / 2, gridTop + 10, "NEXT", infoFont, red, true);

            // Нарисуйте границу
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &nextBorder);


            // Четкая сетка
            int nextField[14][4][3];

            for (int row = 0; row < 14; row++) {
                for (int column = 0; column < 4; column++) {
                    nextField[row][column][0] = 0;
                    nextField[row][column][1] = 0;
                    nextField[row][column][2] = 0;
                }
            }

            //Add pieces to grid
            int space = 0;
            for (auto shape : nextShapes) {
                colour = shape.getColour();
                for (auto num : shape.state()) {

                    int row = (num / 4) + space;
                    int column = num - (num / 4) * 4;
                    nextField[row][column][0] = colour[0];
                    nextField[row][column][1] = colour[1];
                    nextField[row][column][2] = colour[2];
                }
                space += 5;
            }

            // Нарисуйте сетку
            for (int row = 0; row < 14; row++) {
                for (int column = 0; column < 4; column++) {
                    SDL_Rect nextRect;
                    nextRect.x = column * (GRIDSIZE + 5) + nextBorder.x + 15;
                    nextRect.y = nextBorder.y + (row * GRIDSIZE) + 15;
                    nextRect.w = GRIDSIZE + 5; // Большая площадь
                    nextRect.h = GRIDSIZE + 5;

                    // Цвет квадрата
                    SDL_SetRenderDrawColor(renderer, nextField[row][column][0], nextField[row][column][1], nextField[row][column][2], 255);
                    SDL_RenderFillRect(renderer, &nextRect);

                    // Граница квадрата из него 
                    if (nextField[row][column][0] != 0 || nextField[row][column][1] != 0 || nextField[row][column][2] != 0) {
                        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                        SDL_RenderDrawRect(renderer, &nextRect);
                    }

                }
            }


            // хранить - - - - - - - - - - - - -
            SDL_Rect holdBorder;
            holdBorder.x = OFFSET - 200;
            holdBorder.y = gridTop + 50;
            holdBorder.w = 175;
            holdBorder.h = 175;

            // Установление параметра для окошка с удержанием фигуры
            renderText(holdBorder.x + holdBorder.w / 2, gridTop + 10, "HOLDIK", infoFont, white, true);

            // Надпись текста
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &holdBorder);

            // Прозрачная сетка
            int holdField[4][4][3] =
            {
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
            };

            // Добавить фрагмент в сетку
            if (holdShape.row != -1) {
                colour = holdShape.getColour();
                for (auto num : holdShape.state()) {
                    int row = (num / 4);
                    int column = num - (num / 4) * 4;
                    holdField[row][column][0] = colour[0];
                    holdField[row][column][1] = colour[1];
                    holdField[row][column][2] = colour[2];
                }
            }

            // Нарисуйте сетку
            for (int row = 3; row > -1; row--) {
                for (int column = 0; column < 4; column++) {
                    SDL_Rect holdRect;
                    holdRect.x = column * (GRIDSIZE + 5) + holdBorder.x + 15;
                    holdRect.y = (holdBorder.y + holdBorder.h - 15) - (row * (GRIDSIZE + 5)) - (GRIDSIZE + 5);
                    holdRect.w = GRIDSIZE + 5;  // Larger square
                    holdRect.h = GRIDSIZE + 5;

                    // Colour of square
                    SDL_SetRenderDrawColor(renderer, holdField[row][column][0], holdField[row][column][1], holdField[row][column][2], 255);
                    SDL_RenderFillRect(renderer, &holdRect);

                    // Border of square of it has a piece
                    if (holdField[row][column][0] != 0 || holdField[row][column][1] != 0 || holdField[row][column][2] != 0) {
                        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                        SDL_RenderDrawRect(renderer, &holdRect);
                    }
                }
            }

        }

        //// Конечный экран
        //if (state == "finish") {
        //    renderText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 125, "GAME OVER", titleFont, white, true);
        //    renderText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 25, "Press R to play again or ESC to close", textFont, white, true);
        //}

        // Показать новый кадр
      SDL_RenderPresent(renderer);
    }

    // Очистите память после завершения программы
    void clean() {
        // Очистить память, используемую SDL
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        TTF_Quit();
        cout << "Game Cleaned\n";
    }

    //Возвращает текущее состояние игры
    string getState() {
        return state;
    }

private:
    // Экран
    int SCREEN_WIDTH; // Window
    int SCREEN_HEIGHT; // Window y
    int GRIDSIZE; // Размер каждого квадрата сетки
    int OFFSET; // X положение левой стороны сетки
    int GRID_HEIGHT;
    int GRID_WIDTH;

    // Game
    int score;
    float level;
    int linesCleared;
    bool held; // если игрок нажал на удержание

    // Запуск анимации
    bool down;
    Uint8 light;

    // Шрифты
    TTF_Font* infoFont;
    TTF_Font* titleFont;
    TTF_Font* textFont;

    // Таймер сброса рисунка
    Uint32 startMS;

    // SDL2
    SDL_Window* window;
    SDL_Renderer* renderer;

    // Рисунки
    Figure shape;
    Figure holdShape;
    vector<Figure> nextShapes;

    string state; // начать/воспроизвести/закончить/выйти
    int field[22][10][3]; // игровая сетка 10x20, хранящая цвета

    // сброс игры
    void reset() {
        score = 0;
        level = 1;
        linesCleared = 0;
        state = "start";

        down = true;
        light = 255;

        // Сбросить текущую форму
        shape.reset();

        // Используется для обозначения пустой формы удержания
        holdShape.row = -1;
        // Получите следующие 3 фигуры
        nextShapes.clear();
        for (int i = 0; i < 3; i++) {
            Figure nextShape;
            nextShape.reset();
            nextShapes.push_back(nextShape);
        }

        // четкая сетка
        for (int row = 0; row < 22; row++) {
            for (int column = 0; column < 10; column++) {
                field[row][column][0] = 0; //r
                field[row][column][1] = 0; //g
                field[row][column][2] = 0; //b
            }
        }
        startMS = SDL_GetTicks(); // Время начала кадра
    }

    // Получите новую фигуру и начните с самого начала
    void new_figure() {
        shape = nextShapes[0];
        nextShapes[0] = nextShapes[1];
        nextShapes[1] = nextShapes[2];
        nextShapes[2].reset();
        if (intersects()) {
            state = "finish";
        }
    }

    void rotateRight() {
        shape.rotateRight();
        if (intersects()) {
            shape.rotateLeft();
        }
    }

    void rotateLeft() {
        shape.rotateLeft();
        if (intersects()) {
            shape.rotateRight();
        }
    }

    // Двигайтесь вниз, пока фигура не достигнет
    void hardDrop() {
        int distance = 0;
        while (!intersects()) {
            shape.row--;
            distance++;
        }
        shape.row++;
        distance--;
        score += distance * 2;
        freeze();
        breakLines();
    }

    // Переместитесь на один ряд вниз
    void softDrop() {
        shape.row--;
        score++;
        if (intersects()) {
            shape.row++;
            score--;
        }

    }

    void moveRight() {
        shape.column++;
        if (intersects()) {
            shape.column--;
        }
    }

    void moveLeft() {
        shape.column--;
        if (intersects()) {
            shape.column++;
        }
    }

    // Переключатель для удержания
    void hold() {
        if (!held) {
            if (holdShape.row != -1) {
                holdShape.row = 19;
                holdShape.column = 3;
                shape.resetRotation();
                swap(holdShape, shape);
            }
            else {
                holdShape = shape;
                new_figure();
            }
            held = true;
        }
    }

    // Проверьте, не касается ли деталь чего либо
    bool intersects() {

        vector<int> proximity; // Вектор всех близлежащих частей
        proximity.clear();
        //Выполните итерацию по 4 строкам
        for (int row = shape.row; row > shape.row - 4; row--) {
            // 
            for (int column = shape.column; column < shape.column + 4; column++) {

                // При касании другой части
                if (field[row][column][0] != 0 || field[row][column][1] != 0 || field[row][column][2] != 0) {
                    // Преобразуйте 2d-сетку в число в диапазоне 0-15
                    int val = ((shape.row - row) * 4) + (column - shape.column);
                    proximity.push_back(val); //Добавить по возможности 
                }
                // Касание края экрана
                else if (row < 0 || column > 9 || column < 0) {
                    int val = ((shape.row - row) * 4) + (column - shape.column);
                    proximity.push_back(val); //Добавить по возможности
                }
            }
        }

        // Проверка на соответствие
        if (proximity.size() > 0) {
            for (auto num : shape.state()) {
                // Если позиция также находится в непосредственной близости, они сталкиваются
                if (find(proximity.begin(), proximity.end(), num) != proximity.end()) {
                    return true;
                }
            }
        }
        return false;
    }

    // Очистите полные строки и переместите все, что выше, вниз
    void breakLines() {
        int linesBroken = 0;
        for (int row = 0; row < 20; row++) {
            bool full = true;
            for (int column = 0; column < 10; column++) {
                // Если квадрат пуст, проверьте следующую строку
                if (field[row][column][0] == 0 && field[row][column][1] == 0 && field[row][column][2] == 0) {
                    full = false;
                    break;
                }
            }
            // Если строка заполнена, переместите все, что находится выше, вниз
            if (full) {
                linesBroken++;
                linesCleared++;
                for (int rRow = row + 1; rRow < 20; rRow++) {
                    for (int rColumn = 0; rColumn < 10; rColumn++) {
                        field[rRow - 1][rColumn][0] = field[rRow][rColumn][0];
                        field[rRow - 1][rColumn][1] = field[rRow][rColumn][1];
                        field[rRow - 1][rColumn][2] = field[rRow][rColumn][2];
                    }
                }
                row--;
            }

        }

        // Рассчитать балл
        if (linesBroken == 1) {
            score += 100 * level;
        }
        else if (linesBroken == 2) {
            score += 300 * level;
        }
        else if (linesBroken == 3) {
            score += 500 * level;
        }
        else if (linesBroken == 4) {
            score += 800 * level;
        }
    }

    // Зафиксируйте фигуру на месте на сетке
    void freeze() {
        vector<int> colour = shape.getColour();
        for (auto num : shape.state()) {
            int row = shape.row - (num / 4);
            int column = (num - (num / 4) * 4) + shape.column;
            field[row][column][0] = colour[0];
            field[row][column][1] = colour[1];
            field[row][column][2] = colour[2];
        }
        // Создайте новую фигуру
        new_figure();
        // установка параметра для ожидания
        held = false;
    }

    // функция для отображения текста на экране
    void renderText(int xpos, int ypos, string text, TTF_Font* font, SDL_Color& colour, bool center, int wrap = 1000) {
        SDL_Rect position;
        position.x = xpos;
        position.y = ypos;

        // Создание текстуры
        SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), colour, wrap);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // установка ширины и высоты
        SDL_QueryTexture(texture, nullptr, nullptr, &position.w, &position.h);

        if (center) {
            position.x -= position.w / 2;
        }

        // отображение текстуры
        SDL_RenderCopy(renderer, texture, nullptr, &position);

        SDL_DestroyTexture(texture);
    }

    // Загрузка шрифтов
    TTF_Font* getFont(string fontName, int size) {
        fontName += ".ttf";
        string fontPath = "C:/Windows/Fonts/" + fontName;
        TTF_Font* userFont;
        userFont = TTF_OpenFont(fontPath.c_str(), size);
        if (!userFont) {
            userFont = TTF_OpenFont(fontName.c_str(), size);
        }
        return userFont;
    }

};


// MAIN - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int main(int argc, char* argv[])
{
    const int FPS = 100; // Как часто будет обновлять экран
    const float TICKS_PER_FRAME = 1000 / FPS; // Подсчитывание количества миллисекунд для каждого фрейма (частота смены 

    Tetris tetris("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 30); // Инициализация параметров окна: Имя, screenX, screenY, width, height, gridsize
    // Вылавливание ошибок сборки SLD2
    if (tetris.error != 0) {
        return -1;
    }

    // Gameloop
    while (tetris.getState() != "quit") {
        Uint64 start = SDL_GetPerformanceCounter(); // Время начала кадра

        // Frame
        tetris.handleEvents(); //Получить входные данные
        tetris.update(); // вычисление физики
        tetris.render(); // Рисовка новой рамки

        Uint64 end = SDL_GetPerformanceCounter(); // Время окончания кадра

        // Разница во времени в милисек(отклик)
        float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

        // время, чтобы карды обновлялись с постоянной скоростью
        float delay = floor(TICKS_PER_FRAME - elapsedMS);
        if (delay > 0) {
            SDL_Delay(delay);
        }
    }

    // Очищение используемой памяти SDL2
    tetris.clean();

    return 0;
}