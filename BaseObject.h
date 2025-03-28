#ifndef BASE_OBJECT_H_
#define BASE_OBJECT_H_

#include "CommonFuntion.h"

class BaseObject {
public:
    SDL_Texture* p_object_;             //luu tru hinh anh
    SDL_Rect rect_;                           //luu vi tri va kich thuoc hinh anh

    BaseObject();
    ~BaseObject();
    void SetRect(const int& x, const int& y) {
        rect_.x = x;
        rect_.y = y;
    }

    bool loadImg(const char* path, SDL_Renderer* renderer);
    void Render(SDL_Renderer* renderer);
    void Free();
};

#endif // BASE_OBJECT_H_
