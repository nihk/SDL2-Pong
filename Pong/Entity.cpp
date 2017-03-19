#include "Entity.h"
#include "Utils.h"

Entity::Entity()
	: Entity(0, 0, 0, 0) {
}

Entity::Entity(int x, int y, int w, int h)
	: Entity(SDL_Rect{ x, y, w, h }) {
}

Entity::Entity(SDL_Rect rect)
	: Entity(rect, Colors::MATTE_BLACK) {
}

Entity::Entity(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	: Entity(SDL_Rect{ x, y, w, h }, SDL_Color{ r, g, b, a }) {
}

Entity::Entity(int x, int y, int w, int h, SDL_Color color)
	: Entity(SDL_Rect{ x, y, w, h }, color) {
}

Entity::Entity(SDL_Rect rect, SDL_Color color)
	: Entity(rect, color, 0.0f, 0.0f) {
}

Entity::Entity(SDL_Rect rect, SDL_Color color, float xVelocity, float yVelocity)
	: mRect(rect)
	, mColor(color)
	, mVelocityX(xVelocity)
	, mVelocityY(yVelocity) {
}

void Entity::draw(SDL_Renderer* renderer) const {
	SDL_SetRenderDrawColor(renderer, mColor.r, mColor.g, mColor.b, mColor.a);
	SDL_RenderFillRect(renderer, &mRect);
}

SDL_Point Entity::getCenter() const {
	SDL_Point p;
	p.x = getLeft() + getWidth() / 2;
	p.y = getTop() + getHeight() / 2;
	return p;
}

void Entity::setCenter(int x, int y) {
	setCenter(SDL_Point{ x, y });
}

void Entity::setCenter(SDL_Point point) {
	setLeft(point.x - getWidth() / 2);
	setTop(point.y - getHeight() / 2);
}

SDL_bool Entity::containsPoint(SDL_Point point) const {
	return SDL_PointInRect(&point, &mRect);
}

SDL_bool Entity::containsRect(SDL_Rect rect) const {
	return SDL_HasIntersection(&mRect, &rect);
}

void Entity::move(int dx, int dy) {
	mRect.x += dx;
	mRect.y += dy;
}

void Entity::update(float delta) {
	int dx = static_cast<int>(round(mVelocityX * delta));
	int dy = static_cast<int>(round(mVelocityY * delta));
	
	move(dx, dy);
}

bool Entity::message(const MessageId messageId, void* data) {
	return false;
}
