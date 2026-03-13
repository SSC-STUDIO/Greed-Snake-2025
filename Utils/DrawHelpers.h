#pragma once
#include <graphics.h>

inline int ToInt(float value) {
    return static_cast<int>(value);
}

inline int ToInt(int value) {
    return value;
}

inline void circle(float x, float y, float r) {
    ::circle(ToInt(x), ToInt(y), ToInt(r));
}

inline void solidcircle(float x, float y, float r) {
    ::solidcircle(ToInt(x), ToInt(y), ToInt(r));
}

inline void fillcircle(float x, float y, float r) {
    ::fillcircle(ToInt(x), ToInt(y), ToInt(r));
}

inline void line(float x1, float y1, float x2, float y2) {
    ::line(ToInt(x1), ToInt(y1), ToInt(x2), ToInt(y2));
}

inline void rectangle(float left, float top, float right, float bottom) {
    ::rectangle(ToInt(left), ToInt(top), ToInt(right), ToInt(bottom));
}

inline void roundrect(float left, float top, float right, float bottom, float ellipsewidth, float ellipseheight) {
    ::roundrect(ToInt(left), ToInt(top), ToInt(right), ToInt(bottom), ToInt(ellipsewidth), ToInt(ellipseheight));
}

inline void solidrectangle(float left, float top, float right, float bottom) {
    ::solidrectangle(ToInt(left), ToInt(top), ToInt(right), ToInt(bottom));
}

inline void solidroundrect(float left, float top, float right, float bottom, float ellipsewidth, float ellipseheight) {
    ::solidroundrect(ToInt(left), ToInt(top), ToInt(right), ToInt(bottom), ToInt(ellipsewidth), ToInt(ellipseheight));
}

inline void outtextxy(float x, float y, LPCTSTR text) {
    ::outtextxy(ToInt(x), ToInt(y), text);
}
