#ifndef CAD_MODELING_COMMON_H
#define CAD_MODELING_COMMON_H

template <typename T >
class Vec3
{
public:
    T x;
    T y;
    T z;
    inline Vec3<T> operator +(Vec3<T> &toAdd){
        return Vec3<T>{x + toAdd.x, y + toAdd.y, z + toAdd.z};
    }
};

typedef Vec3<double> Vec3d;

#endif //CAD_MODELING_COMMON_H
