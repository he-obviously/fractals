#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

struct point {

    double x, y;

    point(double _x = 0, double _y = 0) {
        this->x = _x;
        this->y = _y;
    }

};

struct pixel {

    double x, y;

    pixel(double _x = 0, double _y = 0) {
        this->x = _x;
        this->y = _y;
    }

};

struct complex {

    double Re, Im;

    complex(double _Re = 0, double _Im = 0) {
        this->Re = _Re;
        this->Im = _Im;
    }

    friend complex operator-(const complex &a) {
        return {-a.Re, -a.Im};
    }

    friend complex operator+(const complex &a, const complex &b) {
        return {a.Re + b.Re, a.Im + b.Im};
    }

    friend complex operator-(const complex &a, const complex &b) {
        return a + (-b);
    }

    friend double dot_product(const complex &a, const complex &b) {
        return a.Re * b.Re + a.Im * b.Im;
    }

    friend double cross_product(const complex &a, const complex &b) {
        return a.Re * b.Im - a.Im * b.Re;
    }

    friend complex operator*(const complex &a, const complex &b) {
        return {a.Re * b.Re - a.Im * b.Im, a.Re * b.Im + a.Im * b.Re};
    }

    complex pow(int n) {
        complex ans = {1, 0}, step = {Re, Im};
        while (n > 0) {
            if (n % 2 == 1) {
                ans = ans * step;
            }
            ans = ans * ans;
            n /= 2;
        }
        return ans;
    }

    double abs() {
        return sqrt(Re * Re + Im * Im);
    }

};

const int W = 720, H = 720, MAX_ITERS = 206;
double scale = 200;
pixel centre{W / 2, H / 2};
sf::Uint8* pixels = new sf::Uint8[W * H * 4];

point pixel_to_point(const pixel &a) {
    return {(a.x - centre.x) / scale, (centre.y - a.y) / scale};
}

double get(double x) {
    return (x + 1.0) / 2.0 * 255.0;
}

void make_fractal(complex c) {
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            point p = pixel_to_point({double(i), double(j)});
            complex z = {p.x, p.y};
            int it;
            double r = 2;
            for (it = 0; it < MAX_ITERS; ++it) {
                if (z.abs() >= r) {
                    break;
                }
                z = z.pow(2) + c;
            }
            double len = z.abs();
            int id = 4 * (j * W + i);
            if (it == MAX_ITERS) {
                pixels[id] = pixels[id + 1] = pixels[id + 2] = 0;
                pixels[id + 3] = 255;
            }
            else {
                double m = it - log2(log2(len));
                pixels[id] = int(get(sin(m * 0.2)));
                pixels[id + 1] = int(get(sin(m * 0.3)));
                pixels[id + 2] = int(get(sin(m * 0.4)));
                pixels[id + 3] = 255;
            }
        }
    }
}

int main() {

    sf::RenderWindow window(sf::VideoMode(W, H), "SFML works!");

    sf::Texture texture;
    texture.create(W, H);
    sf::Sprite sprite(texture);

    bool pressed = false;
    pixel mouse_pos;

    sf::ConvexShape win;
    win.setPointCount(4);
    win.setOutlineThickness(3);
    win.setOutlineColor(sf::Color::Magenta);
    win.setFillColor(sf::Color(0, 0, 0, 0));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                pressed = true;
                mouse_pos = {double(sf::Mouse::getPosition(window).x), double(sf::Mouse::getPosition(window).y)};
            }
            else if (event.type == sf::Event::MouseMoved) {
                if (pressed) {
                    pixel current_pos = {double(sf::Mouse::getPosition(window).x), double(sf::Mouse::getPosition(window).y)};
                    win.setPoint(0, sf::Vector2f(mouse_pos.x, mouse_pos.y));
                    win.setPoint(1, sf::Vector2f(current_pos.x, mouse_pos.y));
                    win.setPoint(2, sf::Vector2f(current_pos.x, current_pos.y));
                    win.setPoint(3, sf::Vector2f(mouse_pos.x, current_pos.y));
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                win.setPoint(0, sf::Vector2f(0, 0));
                win.setPoint(1, sf::Vector2f(0, 0));
                win.setPoint(2, sf::Vector2f(0, 0));
                win.setPoint(3, sf::Vector2f(0, 0));
                /*pixel current_pos = {double(sf::Mouse::getPosition(window).x), double(sf::Mouse::getPosition(window).y)};
                double d = (current_pos.x + mouse_pos.x) / 2;
                centre = {mouse_pos.x + d, mouse_pos.y + d};*/
                pressed = false;
            }
        }
        make_fractal({0.377, -0.248});
        texture.update(pixels);
        window.clear();
        window.draw(sprite);
        window.draw(win);
        window.display();
    }

    return 0;
}
