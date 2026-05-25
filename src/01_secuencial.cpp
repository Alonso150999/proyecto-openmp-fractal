#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cmath>

using namespace std;
using namespace std::chrono;

const int WIDTH = 4096;
const int HEIGHT = 4096;

const int MAX_ITER = 2000;

// Kernel Gaussiano simple 15x15
const int KERNEL_SIZE = 15;

struct Pixel {
    unsigned char r, g, b;
};

// ------------------------------------------------------------
// Guardar imagen PPM
// ------------------------------------------------------------
void savePPM(const string& filename, const vector<Pixel>& image) {
    ofstream file(filename, ios::binary);

    file << "P6\n";
    file << WIDTH << " " << HEIGHT << "\n255\n";

    for (const auto& p : image) {
        file.write((char*)&p, 3);
    }

    file.close();
}

// ------------------------------------------------------------
// Generar Mandelbrot
// ------------------------------------------------------------
void generateMandelbrot(vector<Pixel>& image) {

    for (int y = 0; y < HEIGHT; y++) {

        for (int x = 0; x < WIDTH; x++) {

            double zx = 0.0;
            double zy = 0.0;

            double cx = (x - WIDTH / 2.0) * 4.0 / WIDTH;
            double cy = (y - HEIGHT / 2.0) * 4.0 / HEIGHT;

            int iter = 0;

            while (zx * zx + zy * zy < 4.0 && iter < MAX_ITER) {

                double temp = zx * zx - zy * zy + cx;

                zy = 2.0 * zx * zy + cy;
                zx = temp;

                iter++;
            }

            int color = (255 * iter) / MAX_ITER;

            image[y * WIDTH + x] = {
                (unsigned char)color,
                (unsigned char)(color / 2),
                (unsigned char)(255 - color)
            };
        }
    }
}

// ------------------------------------------------------------
// Crear kernel gaussiano
// ------------------------------------------------------------
vector<vector<float>> createGaussianKernel() {

    vector<vector<float>> kernel(KERNEL_SIZE,
                                 vector<float>(KERNEL_SIZE));

    float sigma = 5.0f;
    float sum = 0.0f;

    int half = KERNEL_SIZE / 2;

    for (int y = -half; y <= half; y++) {

        for (int x = -half; x <= half; x++) {

            float value = exp(-(x * x + y * y)
                         / (2 * sigma * sigma));

            kernel[y + half][x + half] = value;

            sum += value;
        }
    }

    // Normalizar
    for (int y = 0; y < KERNEL_SIZE; y++) {

        for (int x = 0; x < KERNEL_SIZE; x++) {

            kernel[y][x] /= sum;
        }
    }

    return kernel;
}

// ------------------------------------------------------------
// Aplicar convolución gaussiana
// ------------------------------------------------------------
void applyGaussianBlur(const vector<Pixel>& input,
                       vector<Pixel>& output) {

    auto kernel = createGaussianKernel();

    int half = KERNEL_SIZE / 2;

    for (int y = half; y < HEIGHT - half; y++) {

        for (int x = half; x < WIDTH - half; x++) {

            float r = 0;
            float g = 0;
            float b = 0;

            for (int ky = -half; ky <= half; ky++) {

                for (int kx = -half; kx <= half; kx++) {

                    Pixel p =
                        input[(y + ky) * WIDTH + (x + kx)];

                    float weight =
                        kernel[ky + half][kx + half];

                    r += p.r * weight;
                    g += p.g * weight;
                    b += p.b * weight;
                }
            }

            output[y * WIDTH + x] = {
                (unsigned char)r,
                (unsigned char)g,
                (unsigned char)b
            };
        }
    }
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main() {

    vector<Pixel> image(WIDTH * HEIGHT);

    vector<Pixel> blurred(WIDTH * HEIGHT);

    // -------------------------
    // Mandelbrot
    // -------------------------
    auto start1 = high_resolution_clock::now();

    generateMandelbrot(image);

    auto end1 = high_resolution_clock::now();

    double mandelbrotTime =
        duration<double>(end1 - start1).count();

    cout << "Tiempo Mandelbrot: "
         << mandelbrotTime
         << " segundos\n";

    // Guardar fractal original
    savePPM("output/fractal.ppm", image);

    // -------------------------
    // Convolución
    // -------------------------
    auto start2 = high_resolution_clock::now();

    applyGaussianBlur(image, blurred);

    auto end2 = high_resolution_clock::now();

    double blurTime =
        duration<double>(end2 - start2).count();

    cout << "Tiempo Convolucion: "
         << blurTime
         << " segundos\n";

    // Guardar imagen filtrada
    savePPM("output/filtrada.ppm", blurred);

    cout << "Imagenes guardadas en output/\n";

    return 0;
}
