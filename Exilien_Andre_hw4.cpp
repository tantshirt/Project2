/*
Name: Andre Exilien
Date: November 4, 2023
Description: This program simulates terraforming by procedurally generating a land mass using a dirtball dropping algorithm.
Usage: Compile and run the program, follow the console prompts for input. Use the -s <integer> flag to set a custom seed.
*/ 
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <random>
#include "Terraformer.h"
#include <cstdio> 
#include <cstring>
// Main Function
int main(int argc, char* argv[]) {
    int seed = static_cast<int>(time(nullptr));
    if(argc > 1) {
        if (strcmp(argv[1], "-s") == 0) {
            if (argc > 2) {
                seed = atoi(argv[2]);
                std::cout << "seed: " << seed << std::endl;

            } 
        } 
    }
    
    srand(seed);

    std::cout << "Welcome to Andre Exilien's CSE240 the Terraformer!\n";

    // Input for grid dimensions
    int width, height;
    std::cout << "Enter grid width: ";
    std::cin >> width;
    std::cout << "Enter grid height: ";
    std::cin >> height;

    // Input validation (basic)
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid grid dimensions. Please enter positive values." << std::endl;
        return 1;
    }

    // Create and initialize landmass array
    int** landmass = createLandmass(width, height);
    initializeLandmass(landmass, width, height);

    // Input for terraforming parameters
    int waterLine, radius, power, numberOfDirtballs;
    std::cout << "Enter value for waterline (40-200): ";
    std::cin >> waterLine;
    std::cout << "Enter dirtball radius (minimum 2): ";
    std::cin >> radius;
    std::cout << "Enter dirtball power rating (minimum = radius): ";
    std::cin >> power;
    std::cout << "Enter number of dirtballs to drop: ";
    std::cin >> numberOfDirtballs;

    // Input validation (basic)
    if (waterLine < 40 || waterLine > 200 || radius < 2 || power < radius || numberOfDirtballs <= 0) {
        std::cerr << "Invalid input parameters." << std::endl;
        deleteLandmass(landmass, height);
        return 1;
    }

    // Dropping dirtballs
    for (int i = 0; i < numberOfDirtballs; ++i) {
        
        int centerX = frand() * width;
        int centerY = frand() * height;
        dropDirtBall(landmass, width, height, centerX, centerY, radius, power);
    }

    // Print raw landmass to file
    printRawLand(landmass, width, height);

    // Finding max value in the landmass and normalizing
    int maxVal = findMax(landmass, width, height);
    normalizeMap(landmass, width, height, maxVal);   


    // Print normalized landmass to file
    writeNormalizedLandmassToFile(landmass, width, height);

    // Finalize map and output to file
    char** finalMap = finalizeMap(landmass, width, height, waterLine);
    printFinalMap(finalMap, width, height, "final_landmass.txt");

    // Cleanup
    deleteLandmass(landmass, height);
    for (int i = 0; i < height; ++i) {
        delete[] finalMap[i];
    }
    delete[] finalMap;

    std::cout << "Terraforming complete. Check output files for results." << std::endl;
    return 0;
}

double frand(){
    return (double)rand() / ((double)RAND_MAX + 0);
}
// Function definitions
int** createLandmass(int width, int height) {
    int** landmass = new int*[height];
    for (int i = 0; i < height; ++i) {
        landmass[i] = new int[width];
    }
    return landmass;
}

void initializeLandmass(int** landmass, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            landmass[y][x] = 0;
        }
    }
}

void dropDirtBall(int** landmass, int width, int height, int centerX, int centerY, int radius, int power) {
    for (int y = centerY - radius ; y <= centerY + radius; y++) {
        for (int x = centerX - radius; x <= centerX + radius; x++) {
            if (boundsCheck(x, y, width, height)) {
                
                int distanceSquared = (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY);
                
                if (distanceSquared <= radius * radius) {
                int distance = static_cast<int>(sqrt(distanceSquared));
                int impactValue = power - distance; 
                
                if (impactValue > 0) {
                landmass[y][x] += impactValue;
                    }
                }
            }
        }
    }
}


bool boundsCheck(int x, int y, int width, int height) {
    return (x >= 0 && x < width && y >= 0 && y < height);
}

int findMax(int** landmass, int width, int height) {
    int maxVal = landmass[0][0];
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (landmass[y][x] > maxVal) {
                maxVal = landmass[y][x];
            }
        }
    }
    return maxVal;
}

void normalizeMap(int** landmass, int width, int height, int maxVal) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
        landmass[y][x] = static_cast<int>((landmass[y][x] / static_cast<float>(maxVal)) * 255);
        }
    }
}


void printLand(int** landmass, int width, int height, std::ostream& stream) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            stream.width(4);
            stream << landmass[y][x];
            std::cout.width(4);
            std::cout << landmass[y][x];
        }
        stream << std::endl;
        std::cout << std::endl;
    }
}


char** finalizeMap(int** landmass, int width, int height, int waterLine) {
    int landZone = 255 - waterLine;
    char** map = new char*[height];
    for (int y = 0; y < height; ++y) {
        map[y] = new char[width];
        for (int x = 0; x < width; ++x) {
            int value = landmass[y][x];
            // Calculate integer thresholds
            int deepWaterThreshold = waterLine / 2;
            int coastThreshold = waterLine + (landZone * 15) / 100;
            int plainsThreshold = waterLine + (landZone * 40) / 100;
            int forestThreshold = waterLine + (landZone * 80) / 100;
            // Use integer thresholds for comparisons
            if (value < deepWaterThreshold) {
                map[y][x] = '#'; // Deep water
            } else if (value >= deepWaterThreshold && value <= waterLine) {
                map[y][x] = '~'; // Shallow water
            } else if (value > waterLine && value < coastThreshold) {
                map[y][x] = '.'; // Coast/Beach
            } else if (value >= coastThreshold && value < plainsThreshold) {
                map[y][x] = '-'; // Plains/Grass
            } else if (value >= plainsThreshold && value < forestThreshold) {
                map[y][x] = '*'; // Forests
            } else {
                map[y][x] = '^'; // Mountains
            }
        }
    }
    return map;
}


void deleteLandmass(int** landmass, int height) {
    for (int i = 0; i < height; ++i) {
        delete[] landmass[i];
    }
    delete[] landmass;
}

void printRawLand(int** landmass, int width, int height) {
    std::ofstream rawFile("raw_landmass.txt");
    if (!rawFile) {
        std::cerr << "Failed to open raw_landmass.txt for writing." << std::endl;
        return;
    }
    std::cout << "\nRaw Landmass:" << std::endl;
    printLand(landmass, width, height, rawFile);
    rawFile.close();
}

void writeNormalizedLandmassToFile(int** landmass, int width, int height) {
    std::ofstream normalizedFile("normalized_landmass.txt");
    if (!normalizedFile) {
        std::cerr << "Failed to open normalized_landmass.txt for writing." << std::endl;
        return;
    }
    std::cout << "\nNormalized Landmass:" << std::endl;
    printLand(landmass, width, height, normalizedFile);
    normalizedFile.close();
}

void printFinalMap(char** finalMap, int width, int height, const char* filename) {
    std::ofstream finalFile(filename);
    if (!finalFile) {
        std::cerr << "Failed to open " << filename << " for writing." << std::endl;
        return;
    }
    std::cout << "\nFinal Map:" << std::endl;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            finalFile << finalMap[y][x];
            std::cout << finalMap[y][x];
        }
        finalFile << '\n';
        std::cout << std::endl;
    }
    finalFile.close();
}


