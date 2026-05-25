#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cmath>
#include <omp.h>

using namespace std;
using namespace std::chrono;

const int WIDTH = 4096;
const int HEIGHT = 4096;
const int MAX_ITER = 2000;
const int KERNEL_SIZE = 15;
const int COLORS = 256;

struct Pixel {
    unsigned char r, g, b;
};

void savePPM(const string& filename, const vector<Pixel>& image) {
    ofstream file(filename, ios::binary);
    file << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";

    for (const auto& p : image) {
        file.write((char*)&p, 3);
    }
}

void generateMandelbrot(vector<Pixel>& image) {
    #pragma omp parallel for schedule(dynamic, 1)
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

vector<vector<float>> createGaussianKernel() {
    vector<vector<float>> kernel(KERNEL_SIZE, vector<float>(KERNEL_SIZE));

    float sigma = 5.0f;
    float sum = 0.0f;
    int half = KERNEL_SIZE / 2;

    for (int y = -half; y <= half; y++) {
        for (int x = -half; x <= half; x++) {
            float value = exp(-(x * x + y * y) / (2 * sigma * sigma));
            kernel[y + half][x + half] = value;
            sum += value;
        }
    }

    for (int y = 0; y < KERNEL_SIZE; y++) {
        for (int x = 0; x < KERNEL_SIZE; x++) {
            kernel[y][x] /= sum;
        }
    }

    return kernel;
}

void applyGaussianBlur(const vector<Pixel>& input, vector<Pixel>& output) {
    auto kernel = createGaussianKernel();
    int half = KERNEL_SIZE / 2;

    #pragma omp parallel for schedule(static)
    for (int y = half; y < HEIGHT - half; y++) {
        for (int x = half; x < WIDTH - half; x++) {
            float r = 0.0f;
            float g = 0.0f;
            float b = 0.0f;

            for (int ky = -half; ky <= half; ky++) {

                #pragma omp simd reduction(+:r,g,b)
                for (int kx = -half; kx <= half; kx++) {
                    Pixel p = input[(y + ky) * WIDTH + (x + kx)];
                    float weight = kernel[ky + half][kx + half];

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
// Histograma con atomic
// ------------------------------------------------------------
void histogramAtomic(const vector<Pixel>& image, vector<long long>& hist) {
    #pragma omp parallel for
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        int gray = image[i].r;

        #pragma omp atomic
        hist[gray]++;
    }
}

// ------------------------------------------------------------
// Histograma con critical
// ------------------------------------------------------------
void histogramCritical(const vector<Pixel>& image, vector<long long>& hist) {
    #pragma omp parallel for
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        int gray = image[i].r;

        #pragma omp critical
        {
            hist[gray]++;
        }
    }
}

// ------------------------------------------------------------
// Histograma con variables locales por hilo
// ------------------------------------------------------------
void histogramLocal(const vector<Pixel>& image, vector<long long>& hist) {
    int numThreads = omp_get_max_threads();

    vector<vector<long long>> localHists(
        numThreads,
        vector<long long>(COLORS, 0)
    );

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();

        #pragma omp for
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            int gray = image[i].r;
            localHists[tid][gray]++;
        }
    }

    for (int t = 0; t < numThreads; t++) {
        for (int c = 0; c < COLORS; c++) {
            hist[c] += localHists[t][c];
        }
    }
}

void resetHistogram(vector<long long>& hist) {
    for (int i = 0; i < COLORS; i++) {
        hist[i] = 0;
    }
}

int main() {
    vector<Pixel> image(WIDTH * HEIGHT);
    vector<Pixel> blurred(WIDTH * HEIGHT);
    vector<long long> hist(COLORS, 0);

    cout << "Hilos OpenMP: " << omp_get_max_threads() << endl;

    auto start1 = high_resolution_clock::now();
    generateMandelbrot(image);
    auto end1 = high_resolution_clock::now();

    double mandelbrotTime = duration<double>(end1 - start1).count();

    auto start2 = high_resolution_clock::now();
    applyGaussianBlur(image, blurred);
    auto end2 = high_resolution_clock::now();

    double blurTime = duration<double>(end2 - start2).count();

    resetHistogram(hist);

    auto start3 = high_resolution_clock::now();
    histogramAtomic(blurred, hist);
    auto end3 = high_resolution_clock::now();

    double atomicTime = duration<double>(end3 - start3).count();

    resetHistogram(hist);

    auto start4 = high_resolution_clock::now();
    histogramCritical(blurred, hist);
    auto end4 = high_resolution_clock::now();

    double criticalTime = duration<double>(end4 - start4).count();

    resetHistogram(hist);

    auto start5 = high_resolution_clock::now();
    histogramLocal(blurred, hist);
    auto end5 = high_resolution_clock::now();

    double localTime = duration<double>(end5 - start5).count();

    cout << "Tiempo Mandelbrot: " << mandelbrotTime << " segundos\n";
    cout << "Tiempo Convolucion: " << blurTime << " segundos\n";
    cout << "Histograma atomic: " << atomicTime << " segundos\n";
    cout << "Histograma critical: " << criticalTime << " segundos\n";
    cout << "Histograma local: " << localTime << " segundos\n";

    return 0;
}
