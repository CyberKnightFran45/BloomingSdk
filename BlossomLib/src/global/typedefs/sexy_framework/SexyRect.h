#pragma once

#include <iostream>

class SexyRect 
{
private:
    int mX;
    int mY;
    int mWidth;
    int mHeight;

public:

    SexyRect() : mX(0), mY(0), mWidth(0), mHeight(0) {}
		
    SexyRect(int x, int y, int w, int h) : mX(x), mY(y), mWidth(w), mHeight(h) {}

    int getX() const { return mX; }
    void setX(int x) { mX = x; }

    int getY() const { return mY; }
    void setY(int y) { mY = y; }

    int getWidth() const { return mWidth; }
    void setWidth(int w) { mWidth = w; }

    int getHeight() const { return mHeight; }
    void setHeight(int h) { mHeight = h; }
	
	void addX() { mX++; }
    void addY() { mY++; }
};