// PDE2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseLU>
#include <string>
#include <array>
#include <vector>
#include <cmath>
#include "bitmap_image.hpp"
#include <direct.h>

using namespace Eigen;
using namespace std;

static float env_starting_temperature = 0.0f;
static float env_ending_temperature = 0.0f;
static float time = 0.0f;
float border_conditions(float x, float y, float t) {
    return env_starting_temperature+(t/time)*(env_ending_temperature-env_starting_temperature);
}

static float starting_temperature = 0;
float starting_conditions(float x, float y) {
    return starting_temperature;
}

int get_index(int x, int y, int size_x) {
    int index = x + y * size_x;
    return index;
}

struct Point {
    int x;
    int y;
    Point(int _x, int _y) {
        x = _x;
        y = _y;
    }
};

class Impact {
public:

    Impact(string filename) {
        image = bitmap_image(filename);

        cooler_temperature = -100;
        heater_temperature = 750;
    }

    float func(float x, float y, float t) {
        if (is_cooler(x,y)) {
            return cooler_temperature;
        }
        if (is_heater(x, y)) {
            return heater_temperature;
        }
        return 0;
    }

    ~Impact() {
        image.clear();
    }

    float cooler_temperature;
    float heater_temperature;

private:
    bitmap_image image;

    bool is_cooler(int x, int y) {
        if (get_blue(x, y) >= 200 && (get_red(x, y) + get_green(x, y)) <= 100)
            return true;

        return false;
    }

    bool is_heater(int x, int y) {
        if (get_red(x, y) >= 200 && (get_blue(x, y) + get_green(x, y)) <= 100)
            return true;

        return false;
    }

    int get_red(int x, int y) {
        rgb_t colour;

        image.get_pixel(x, y, colour);

        return colour.red;
    }

    int get_green(int x, int y) {
        rgb_t colour;

        image.get_pixel(x, y, colour);

        return colour.green;
    }

    int get_blue(int x, int y) {
        rgb_t colour;

        image.get_pixel(x, y, colour);

        return colour.blue;
    }

};

class Shape {

public:

    Shape(string filename) {
        image = bitmap_image(filename);
        points = vector<Point*>();

        collect_shape_points();
    }

    Point* get_point(int index) {
        if (index >= 0 && index < points.size())
            return points[index];

        return nullptr;
    }

    unsigned int find_index_after(int x, int y, int start_id) {
        int id = -1;
        for (int i = start_id + 1; i < points.size(); i++) {
            Point* point = points[i];
            if (point->x == x && point->y == y) {
                id = i;
                break;
            }
        }
        return id;
    }

    unsigned int find_index_before(int x, int y, int end_id) {
        int id = -1;
        for (int i = end_id - 1; i >= 0; i--) {
            Point* point = points[i];
            if (point->x == x && point->y == y) {
                id = i;
                break;
            }
        }
        return id;
    }

    unsigned int get_points_num() {
        return points.size();
    }

    const unsigned int get_width() {
        return image.width();
    }

    const unsigned int get_height() {
        return image.height();
    }

    bool is_border(int x, int y) {
        if (is_black(x, y)) {
            if (is_white(x - 1, y) || is_white(x + 1, y) || is_white(x, y - 1) || is_white(x, y + 1))
                return true;
        }

        return false;
    }

    bool is_black(int x, int y) {
        if (x < 0 || x >= get_width() || y < 0 || y >= get_height())
            return false;

        if (get_red(x, y) == 0 && get_blue(x, y) == 0 && get_green(x, y) == 0)
            return true;

        return false;
    }

    bool is_white(int x, int y) {
        if (x < 0 || x >= get_width() || y < 0 || y >= get_height())
            return true;

        if (get_red(x, y) == 255 && get_blue(x, y) == 255 && get_green(x, y) == 255)
            return true;

        return false;
    }

    int get_red(int x, int y) {
        rgb_t colour;

        image.get_pixel(x, y, colour);

        return colour.red;
    }

    int get_green(int x, int y) {
        rgb_t colour;

        image.get_pixel(x, y, colour);

        return colour.green;
    }

    int get_blue(int x, int y) {
        rgb_t colour;

        image.get_pixel(x, y, colour);

        return colour.blue;
    }

    ~Shape() {
        image.clear();
    }

    vector<Point*> points;

private:
    void collect_shape_points() {
        for (int y = 0; y < get_height(); y++) {
            for (int x = 0; x < get_width(); x++) {
                if (is_black(x, y)) {
                    points.push_back(new Point(x, y));
                }
            }
        }
    }

    bitmap_image image;
};

class Cube {
public :
    Cube(int size_x, int size_y, int size_z, float step_size_x, float step_size_y, float step_size_t) 
        : size_x_(size_x) , size_y_(size_y), size_z_(size_z), step_size_x_(step_size_x), step_size_y_(step_size_y), step_size_t_(step_size_t) {
        calculated_layer = new float[size_x*size_y];
        previous_layer = new float[size_x*size_y];
        current_z = 1;
        simulation_dir = "/";
    }

    ~Cube() {
        delete[] calculated_layer;
        delete[] previous_layer;
    }

    float& at(const int x, const int y, const int z) {
        if (z == current_z) {
            return calculated_layer[x + y * size_x_];
        }
        else if (z == current_z - 1) {
            return previous_layer[x + y * size_x_];
        }
        return calculated_layer[0];
    }

    void new_layer() {
        if (current_z + 1 < size_z_) {

            current_z++;
            
            // copy calculated layer to previous layer
            for (int x = 0; x < size_x_; x++) {
                for (int y = 0; y < size_y_; y++) {
                    previous_layer[x + y * size_x_] = calculated_layer[x + y * size_x_];
                    if (shape->is_white(x, y)) {
                        calculated_layer[x + y * size_x_] = border_conditions(x*step_size_x_, y*step_size_y_, current_z*step_size_t_);
                    }
                    else {
                        calculated_layer[x + y * size_x_] = 0;
                    }
                }
            }
        }
    }

    void save_layer(VectorXf& layer, int layer_size, const int z) {
        for (int i = 0; i < layer_size; i++) {
            Point* point = shape->get_point(i);
            int x = point->x;
            int y = point->y;

            at(x, y, z) = layer(i);
        }
    }

    void save_bmp_layer(int z, int number) {

        int w = size_x_;
        int h = size_y_;

        bitmap_image image(w, h);

        // set background to orange
        image.set_all_channels(0, 0, 0);


        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                if (shape->is_black(i, j)) {
                    int x = i; int y = (h - 1) - j;

                    float value = at(i, j, z);

                    int r = 127;
                    int g = 127;
                    int b = 127;

                    if (value > 0) {
                        r = 127 + ((value) / temperature_sensivity) * 128;
                    }
                    else {
                        b = 127 + ((-value) / temperature_sensivity) * 128;
                    }


                    if (r > 255) r = 255;
                    if (g > 255) g = 255;
                    if (b > 255) b = 255;

                    image.set_pixel(i, j, r, g, b);
                }
            }
        }

        string file_name = simulation_dir+"img_" + to_string(number) + ".bmp";
        image.save_image(file_name);
    }

    int size_x_;
    int size_y_;
    int size_z_;

    Shape* shape;

    float temperature_sensivity;

    float* calculated_layer;
    float* previous_layer;

    int current_z;

    float step_size_x_;
    float step_size_y_;
    float step_size_t_;

    string simulation_dir;
};

/**
* Arguments ангельский
*/

// 1 : path to area.bmp 24 BIT BMP
// 2 : path to impact.bmp 24 BIT BMP
// 3 : thermal diffusivity (float)
// 4 : cooler temperature
// 5 : heater temperature 
// 6 : duration of experiment
// 7 : starting temperature 
// 8 : environment temperature beginning 
// 9 : environment temperature ending
// 10 : temperature sesivity 
// 11 : real width
// 12 : real height
// 13 : dir to sace


int main(int argc, char** argv)
{
    string area_path = argv[1];
    string impact_path = argv[2];
    float thermal_diffusivity = atof(argv[3]);
    float cooler_temp = atof(argv[4]);
    float heater_temp = atof(argv[5]);
    float duration = atof(argv[6]);
    float starting_temp = atof(argv[7]);
    float env_starting_temp = atof(argv[8]);
    float env_ending_temp = atof(argv[9]);
    float temp_sensivity = atof(argv[10]);
    float real_width = atof(argv[11]);
    float real_height = atof(argv[12]);
    string simulation_path_to_save = argv[13];

    _mkdir(simulation_path_to_save.c_str());

    starting_temperature = starting_temp;
    time = duration;
    env_starting_temperature = env_starting_temp;
    env_ending_temperature = env_ending_temp;

    Shape* shape = new Shape(area_path);

    cout << "Shape loaded" << endl;

    Impact* impact = new Impact(impact_path);
    impact->cooler_temperature = cooler_temp;
    impact->heater_temperature = heater_temp;

    cout << "Impact loaded" << endl;

    float size_x = real_width; // длина пластинки 1.28
    float size_y = real_height; // ширина пластинки
    float time_period = duration; // временной период

    float lambda = thermal_diffusivity; // параметр

    int steps_x = shape->get_width(); // количество шагов по x
    int steps_y = shape->get_height(); // количество шагов по y

    float step_size_x = size_x / (float)steps_x; // размер шага по x
    float step_size_y = size_y / (float)steps_y; // размер шага по y
    float step_size_t = 0.01; // размер шага по времени

    int steps_t = time_period/step_size_t;
    int quality = 100;
    int mod_condition = steps_t / quality;

    int matrix_size = shape->get_points_num();
    SparseMatrix<float, RowMajor> a(matrix_size, matrix_size); // матрица коэф
    a.reserve(VectorXf::Constant(matrix_size, 10));
    VectorXf b(matrix_size);

    cout << "Matrix A init" << endl;

    Cube* cube = new Cube(steps_x, steps_y, steps_t, step_size_x, step_size_y, step_size_t);
    cube->simulation_dir = simulation_path_to_save;

    cout << "Cube init" << endl;

    cube->shape = shape;
    cube->temperature_sensivity = temp_sensivity;
    //начальные условия
    for (int x = 0; x < cube->size_x_; x++) {
        for (int y = 0; y < cube->size_y_; y++) {
            if (shape->is_black(x, y)) {
                cube->at(x, y, 0) = starting_conditions(x*step_size_x, y*step_size_y);
            }
        }
    }

    cout << "Starting conditions init" << endl;

    // пограничные условия
    for (int x = 0; x < cube->size_x_; x++) {
        for (int y = 0; y < cube->size_y_; y++) {
            for (int t = 0; t < cube->size_z_; t++) { 
                if (shape->is_white(x, y)) {
                    cube->at(x, y, t) = border_conditions(x*step_size_x, y*step_size_y, t*step_size_t);
                }
            }
        }
    }

    cout << "Border conditions init" << endl;

    float A = 1 / step_size_t + 2 * lambda*(1 / (step_size_x*step_size_x) + 1 / (step_size_y*step_size_y));
    float B = lambda / (step_size_x*step_size_x);
    float C = lambda / (step_size_y*step_size_y);

    for (int index = 0; index < matrix_size; index++) {

        Point* point = shape->get_point(index);
        int x = point->x;
        int y = point->y;

        float F = cube->at(x, y, 0) / step_size_t + impact->func(x, y, 0 * step_size_t);

        // A
        a.insert(index, index) = A;

        // B (1)
        if (shape->is_white(x - 1, y)) {
            F -= (-B)*(cube->at(x - 1, y, 0));
        }
        else {
            a.insert(index, index-1) = -B;
        }

        // B (2)
        if (shape->is_white(x + 1, y)) {
            F -= (-B)*(cube->at(x + 1, y, 0));
        }
        else {
            a.insert(index, index+1) = -B;
        }

        // C (1)
        if (shape->is_white(x, y - 1)) {
            F -= (-C)*(cube->at(x, y - 1, 0));
        }
        else {
            int upper_index = shape->find_index_before(x, y - 1, index);
            a.insert(index, upper_index) = -C;
        }

        // C (2)
        if (shape->is_white(x, y + 1)) {
            F -= (-C)*(cube->at(x, y + 1, 0));
        }
        else {
            int bottom_index = shape->find_index_after(x, y + 1, index);
            a.insert(index, bottom_index) = -C;
        }

        b(index) = F;
    }

    VectorXf x(matrix_size);
    SimplicialLLT  <SparseMatrix<float>> solver;
    a.makeCompressed();
    
    solver.compute(a);
    x = solver.solve(b);

    cube->save_layer(x, matrix_size, 1);
    cube->save_bmp_layer(0,0);
    cube->new_layer();
 
    cout << "Solved First Layer!!" << endl;

    int bmp_number = 0;
    
    // Solve rest layers
    for (int t = 2; t < steps_t; t++) {
        
        for (int index = 0; index < matrix_size; index++) {

            Point* point = shape->get_point(index);
            int x = point->x;
            int y = point->y;

            //5 in a ; 1 in b

            float F = cube->at(x, y, t - 1) / step_size_t + impact->func(x, y, (t - 1) * step_size_t);

            if (shape->is_white(x - 1, y)) {
                F -= (-B)*(cube->at(x - 1, y, t - 1));
            }
            if (shape->is_white(x + 1, y)) {
                F -= (-B)*(cube->at(x + 1, y, t - 1));
            }

            if (shape->is_white(x, y - 1)) {
                F -= (-C)*(cube->at(x, y - 1, t - 1));
            }
            if (shape->is_white(x, y + 1)) {
                F -= (-C)*(cube->at(x, y + 1, t - 1));
            }

            b(index) = F;
        }

        x = solver.solve(b);

        cube->save_layer(x, matrix_size, t);
        if (t % mod_condition == 0) {
            bmp_number++;
            cout << bmp_number << "%" << endl;
            cube->save_bmp_layer(t, bmp_number);
        }
        cube->new_layer();
        //cout << "Saved " << t << " layer!" << endl;
    }

    return 0;
}


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
