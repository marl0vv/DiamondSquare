#include <iostream>
#include <vector>
#include <random>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const uint64_t n = 9;
const uint64_t width = (1 << n) + 1;
const uint64_t height = (1 << n) + 1;
const uint64_t bytes_per_pixel = 3;
const int max_height = 10; //удачное значение 10 и 20
const int min_height = 1;
const uint64_t image_size = width * height * bytes_per_pixel;

void printHeightMap(std::vector<std::vector<int>> const& heightMap)
{
    int heightMapSize = heightMap.size();
    for (int i = 0; i < heightMapSize; ++i)
    {
        for (int j = 0; j < heightMapSize; ++j)
        {
            std::cout << heightMap[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

namespace original
{
    float round(float value)
    {
        return std::round(std::min(std::max(value, 1.f), 255.f));
    }

    float randomValue(float min, float max)
    {
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    }

    void squareStep(std::vector<std::vector<int>>& heightMap, int chunkSize, int half, float roughness)
    {
        int heightMapSize = heightMap.size();
        for (int x = 0; x < heightMapSize - 1; x += chunkSize)
        {
            for (int y = 0; y < heightMapSize - 1; y += chunkSize)
            {
                float value = (float)(heightMap[x][y] +
                    heightMap[x][y + chunkSize] +
                    heightMap[x + chunkSize][y] +
                    heightMap[x + chunkSize][y + chunkSize]) /
                    4.f +
                    (float)roughness * randomValue(-1, 1);
                heightMap[x + half][y + half] = (int)round(value);
            }
        }
    }

    void diamondStep(std::vector<std::vector<int>>& heightMap, int chunkSize, int half, float roughness)
    {
        int heightMapSize = heightMap.size();
        for (int x = 0; x < heightMapSize; x += half)
        {
            for (int y = (x + half) % chunkSize; y < heightMapSize; y += chunkSize)
            {
                int value = 0;
                int count = 0;

                if (x - half >= 0)
                {
                    value += heightMap[x - half][y];
                    count++;
                }

                if (y - half >= 0)
                {
                    value += heightMap[x][y - half];
                    count++;
                }

                if (y + half < chunkSize)
                {
                    value += heightMap[x][y + half];
                    count++;
                }

                if (x + half < chunkSize)
                {
                    value += heightMap[x + half][y];
                    count++;
                }

                heightMap[x][y] = (int)round((float)value / (float)count + roughness * randomValue(-1, 1));
            }
        }
    }

    std::vector<std::vector<int>> diamondSquare(int N)
    {
        int heightMapSize = std::pow(2, N) + 1;
        std::vector<std::vector<int>> heightMap(heightMapSize);

        for (int i = 0; i < heightMapSize; i++)
        {
            heightMap[i] = std::vector<int>(heightMapSize);
        }

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist255(min_height, max_height);

        heightMap[0][0] = dist255(rng);
        heightMap[0][heightMapSize - 1] = dist255(rng);
        heightMap[heightMapSize - 1][0] = dist255(rng);
        heightMap[heightMapSize - 1][heightMapSize - 1] = dist255(rng);

        int chunkSize = heightMapSize - 1;
        float roughness = 8;

        while (chunkSize > 1)
        {
            squareStep(heightMap, chunkSize, chunkSize / 2, roughness);
            diamondStep(heightMap, chunkSize, chunkSize / 2, roughness);
            chunkSize /= 2;
            roughness = std::max(roughness / 2.f, 0.1f);
        }
        // printHeightMap(heightMap);
        return heightMap;
    }

}

int main(int argc, char** argv)
{
    uint8_t* image = new uint8_t[image_size];

    std::vector<std::vector<int>> grid = original::diamondSquare(n);
    srand(time(0));
    for (int y = 0, index = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image[index++] = (uint8_t)(((float)grid[x][y] / (float)max_height) * 255.f);
            image[index++] = (uint8_t)(((float)grid[x][y] / (float)max_height) * 255.f);
            image[index++] = (uint8_t)(((float)grid[x][y] / (float)max_height) * 255.f);
        }
    }

    std::cout << stbi_write_png("./result22.png", width, height, bytes_per_pixel, image, width * bytes_per_pixel) << std::endl;

    delete[] image;
}
