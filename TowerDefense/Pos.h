#pragma once
#ifndef Pos_H
#define Pos_H

class Pos
{
public:
	float x, y;
	Pos() { x = .0f; y = .0f; }
	Pos(float fx, float fy) { x = fx; y = fy; }
	friend bool operator < (Pos& lhs, Pos& rhs) { return (lhs.x < rhs.x || lhs.y < rhs.y); }
};

#endif // !Pos_H