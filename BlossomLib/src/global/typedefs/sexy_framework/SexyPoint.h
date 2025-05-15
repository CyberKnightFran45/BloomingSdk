#pragma once

#include <iostream>

class SexyPoint 
{
private:

int mX, mY;

public:

SexyPoint() : mX(0), mY(0) {}
SexyPoint(int x, int y) : mX(x), mY(y) {}

int getX() const { return mX; }
void setX(int x) { mX = x; }

int getY() const { return mY; }
void setY(int y) { mY = y; }

void addX() { mX++; }
void addY() { mY++; }
};