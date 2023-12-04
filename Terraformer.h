/*
Name: Exilien Andre
Date: November 4, 2023
Description: This program simulates terraforming by procedurally generating a land mass using a dirtball dropping algorithm.
Usage: Compile and run the program, follow the console prompts for input. Use the -s <integer> flag to set a custom seed.
*/



#ifndef TERRAFORMER_H
#define TERRAFORMER_H

#include <iostream>
#include <fstream>

int** createLandmass(int width, int height);
void initializeLandmass(int** landmass, int width, int height);
void dropDirtBall(int** landmass, int width, int height, int centerX, int centerY, int radius, int power);
bool boundsCheck(int x, int y, int width, int height);
int findMax(int** landmass, int width, int height);
void normalizeMap(int** landmass, int width, int height, int maxVal);
char** finalizeMap(int** landmass, int width, int height, int waterLine);
void deleteLandmass(int** landmass, int height);
void printRawLand(int** landmass, int width, int height);
void writeNormalizedLandmassToFile(int** landmass, int width, int height);
void printFinalMap(char** finalMap, int width, int height, const char* filename);
void printLand(int** landmass, int width, int height, std::ostream& stream);
double frand();


#endif
