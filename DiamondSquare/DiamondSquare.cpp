#include <iostream>
#include <vector>
#include <random>
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void printHeightMap(std::vector<std::vector<int>> const &heightMap)
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

void diamondStep(std::vector<std::vector<int>>& heightMap, int chunkSize, int half, int roughness)
{
    int heightMapSize = heightMap.size();
    for (int x = 0; x < heightMapSize - 1; x += chunkSize)
    {
        for (int y = 0; y < heightMapSize - 1 ; y += chunkSize)
        {
			heightMap[x + half][y + half] =
				(heightMap[x][y] +
					heightMap[x][y + chunkSize] +
					heightMap[x + chunkSize][y] +
					heightMap[x + chunkSize][y + chunkSize]) /
				4 + roughness;
        }
    }
}

void squareStep(std::vector<std::vector<int>>& heightMap, int chunkSize, int half, int roughness)
{
    int heightMapSize = heightMap.size();
    int totalSize = heightMapSize * heightMapSize;
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

            if (y < chunkSize)
            {
                value += heightMap[x][y + half];
                count++;
            }
            
            if (x < chunkSize)
            {
                value += heightMap[x + half][y];
                count++;
            }

            heightMap[x][y] = value / count + roughness;
        }
    }
}

std::vector<std::vector<int>> diamondSquare(int N)
{
    int heightMapSize = std::pow(2, N) + 1;
    std::vector<std::vector<int>> heightMap(heightMapSize, std::vector<int>(heightMapSize));

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist255(0, 255);
    std::uniform_int_distribution<std::mt19937::result_type> dist50(0, 10);

    heightMap[0][0] = dist255(rng);
    heightMap[0][heightMapSize-1] = dist255(rng);
    heightMap[heightMapSize-1][0] = dist255(rng);
    heightMap[heightMapSize-1][heightMapSize-1] = dist255(rng);

    int chunkSize = heightMapSize - 1;
    int roughnessScale = 8; 

    while (chunkSize > 1)
    {
        int half = chunkSize / 2;
        int roughness = (dist50(rng)) - 5 * roughnessScale;
        diamondStep(heightMap, chunkSize, half, roughness);
        squareStep(heightMap, chunkSize, half, roughness);
        chunkSize /= 2;
        roughnessScale /= 2;
    }
    return heightMap;
}


int main()
{
    const int n = 9;
    const int width = (1 << n) + 1;
    const int height = (1 << n) + 1;
    const int bytes_per_pixel = 3;
    const int image_size = width * height * bytes_per_pixel;

    unsigned char* image = new unsigned char[image_size];
    int index = 0;

    std::vector<std::vector<int>> grid = diamondSquare(n);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image[index++] = (uint8_t)grid[y][x];
            image[index++] = (uint8_t)grid[y][x];
            image[index++] = (uint8_t)grid[y][x];
        }
    }

    std::cout << stbi_write_png("./result16.png", width, height, bytes_per_pixel, image, width * bytes_per_pixel) << std::endl;

    std::cout << "Hello, from png_creator!\n";
}

