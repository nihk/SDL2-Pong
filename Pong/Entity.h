#pragma once

#include <SDL.h>
#include "Messageable.h"

class Entity 
	: public Messageable {
public:
	Entity();
	Entity(int x, int y, int w, int h);
	Entity(SDL_Rect rect);
	Entity(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	Entity(int x, int y, int w, int h, SDL_Color color);
	Entity(SDL_Rect rect, SDL_Color color);
	Entity(SDL_Rect rect, SDL_Color color, float xVelocity, float yVelocity);

	void draw(SDL_Renderer* renderer) const;

	int getWidth() const { return mRect.w; }
	int getHeight() const { return mRect.h; }

	void setWidth(int w) { mRect.w = w; }
	void setHeight(int h) { mRect.h = h; }

	int getLeft() const { return mRect.x; }
	int getTop() const { return mRect.y; }
	int getRight() const { return mRect.x + mRect.w; }
	int getBottom() const { return mRect.y + mRect.h; }

	SDL_Point getCenter() const;
	SDL_Rect getRect() const { return mRect; }

	float getVelocityX() const { return mVelocityX; }
	float getVelocityY() const { return mVelocityY; }

	SDL_Color getColor() const { return mColor; }

	void setLeft(int x) { mRect.x = x; }
	void setRight(int x) { mRect.x = x - mRect.w; }
	void setTop(int y) { mRect.y = y; }
	void setBottom(int y) { mRect.y = y - mRect.h; }

	void setCenter(int x, int y);
	void setCenter(SDL_Point point);
	void setRect(int x, int y, int w, int h) { setRect(SDL_Rect{ x, y, w, h }); }
	void setRect(SDL_Rect rect) { mRect = rect; }

	void setVelocityX(float vx) { mVelocityX = vx; }
	void setVelocityY(float vy) { mVelocityY = vy; }
	void setVelocity(float vx, float vy) { setVelocityX(vx); setVelocityY(vy); }

	void setColor(SDL_Color color) { mColor = color; }
	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { mColor.r = r; mColor.g = g; mColor.b = b; mColor.a = a; }

	SDL_bool containsPoint(SDL_Point point) const;
	SDL_bool containsRect(SDL_Rect rect) const;

	void move(int dx, int dy);
	virtual void update(float delta);
	bool message(const MessageId messageId, const void* data = nullptr, void* reply = nullptr) override;

protected:
	SDL_Rect mRect;

private:
	SDL_Color mColor;
	float mVelocityX;
	float mVelocityY;
};

