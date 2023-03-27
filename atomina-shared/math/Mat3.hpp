#pragma once
#include "pch.hpp"
#include "core/api.hpp"
#include "Vec3.hpp"

namespace ATMA
{

    /**
     * @brief 3 dimensional matrix for math applications
     * @tparam T numeric subtype of the matrix
     */
    template<class T>
    class ATMA_API Mat3
    {
    public:
#ifdef _WINDOWS
        template<class T>
#endif
        inline T &operator()(const unsigned int &l_row, const unsigned int &l_col)
        {
            return m_values[l_row][l_col];
        }

        T m_values[3][3]{
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0}
        };
    };

    /**
     * addition of matrices
     * @tparam numeric subtype of the matrices
     * @param a first matrix
     * @param b second martix
     * @returns resulting composite matrix
     */
    template<class T>
    inline Mat3<T> operator+(Mat3<T> a, Mat3<T> b)
    {

        return {
            {{a.m_values[0][0] + b.m_values[0][0],
a.m_values[0][1] + b.m_values[0][1],
a.m_values[0][2] + b.m_values[0][2]},
             {a.m_values[1][0] + b.m_values[1][0],
             a.m_values[1][1] + b.m_values[1][1],
             a.m_values[1][2] + b.m_values[1][2]},
             {a.m_values[2][0] + b.m_values[2][0],
             a.m_values[2][1] + b.m_values[2][1],
             a.m_values[2][2] + b.m_values[2][2]}}
        };
    }

    /**
     * subtraction of matrices
     * @tparam numeric subtype of the matrices
     * @param a first matrix
     * @param b second matrix
     * @returns resulting composite matrix
     */
    template<class T>
    inline Mat3<T> operator-(Mat3<T> a, Mat3<T> b)
    {
        return {
            {{a.m_values[0][0] - b.m_values[0][0],
a.m_values[0][1] - b.m_values[0][1],
a.m_values[0][2] - b.m_values[0][2]},
             {a.m_values[1][0] - b.m_values[1][0],
             a.m_values[1][1] - b.m_values[1][1],
             a.m_values[1][2] - b.m_values[1][2]},
             {a.m_values[2][0] - b.m_values[2][0],
             a.m_values[2][1] - b.m_values[2][1],
             a.m_values[2][2] - b.m_values[2][2]}}
        };
    }

    /**
     * scalar multiplication
     * @tparam numeric subtype of the vector
     * @param a matrix
     * @param b scalar
     * @returns resulting magnified matrix
     */
    template<class T>
    inline Mat3<T> operator*(Mat3<T> a, T b)
    {
        return {
            {{a.m_values[0][0] * b, a.m_values[0][1] * b, a.m_values[0][2] * b},
             {a.m_values[1][0] * b, a.m_values[1][1] * b, a.m_values[1][2] * b},
             {a.m_values[2][0] * b, a.m_values[2][1] * b, a.m_values[2][2] * b}}
        };
    }

    /**
     * matrix multiplication
     * @tparam numeric subtype of the matrix
     * @param a first matrix
     * @param b second matrix
     * @returns resulting matrix
     */
    template<class T>
    Mat3<T> operator*(Mat3<T> a, Mat3<T> b)
    {
        return {
            {{(a.m_values[0][0] * b.m_values[0][0] + a.m_values[0][1] * b.m_values[1][0]
+ a.m_values[0][2] * b.m_values[2][0]),
(a.m_values[0][0] * b.m_values[0][1] + a.m_values[0][1] * b.m_values[1][1]
+ a.m_values[0][2] * b.m_values[2][1]),
(a.m_values[0][0] * b.m_values[0][2] + a.m_values[0][1] * b.m_values[1][2]
+ a.m_values[0][2] * b.m_values[2][2])}, //  row 1

 {(a.m_values[1][0] * b.m_values[0][0] + a.m_values[1][1] * b.m_values[1][0]
 + a.m_values[1][2] * b.m_values[2][0]),
 (a.m_values[1][0] * b.m_values[0][1] + a.m_values[1][1] * b.m_values[1][1]
 + a.m_values[1][2] * b.m_values[2][1]),
 (a.m_values[1][0] * b.m_values[0][2] + a.m_values[1][1] * b.m_values[1][2]
 + a.m_values[1][2] * b.m_values[2][2])}, //  row 2

 {(a.m_values[2][0] * b.m_values[0][0] + a.m_values[2][1] * b.m_values[1][0]
 + a.m_values[2][2] * b.m_values[2][0]),
 (a.m_values[2][0] * b.m_values[0][1] + a.m_values[2][1] * b.m_values[1][1]
 + a.m_values[2][2] * b.m_values[2][1]),
 (a.m_values[2][0] * b.m_values[0][2] + a.m_values[2][1] * b.m_values[1][2]
 + a.m_values[2][2] * b.m_values[2][2])}}  // row 3
        };
    }

    /**
     * matrix multiplication
     * @tparam numeric subtype of the matrix
     * @param a first matrix
     * @param b second matrix
     * @returns resulting matrix
     */
    template<class T>
    Vec3<T> operator*(Mat3<T> a, Vec3<T> b)
    {
        return {
            (a.m_values[0][0] * b.x + a.m_values[0][1] * b.y + a.m_values[0][2] * b.z),
            (a.m_values[1][0] * b.x + a.m_values[1][1] * b.y + a.m_values[1][2] * b.z),
            (a.m_values[2][0] * b.x + a.m_values[2][1] * b.y + a.m_values[2][2] * b.z)};
    }

    /**
     * matrix equivalency check
     * @tparam numeric subtype of the matrix
     * @param a first matrix
     * @param b second matrix
     * @returns if equivalent
     */
    template<class T>
    bool operator==(Mat3<T> a, Mat3<T> b)
    {
        return std::fabs(a.m_values[0][0] - b.m_values[0][0]) <= std::numeric_limits<T>::epsilon()
            && std::fabs(a.m_values[0][1] - b.m_values[0][1]) <= std::numeric_limits<T>::epsilon()
            && std::fabs(a.m_values[0][2] - b.m_values[0][2]) <= std::numeric_limits<T>::epsilon()
            && std::fabs(a.m_values[1][0] - b.m_values[1][0]) <= std::numeric_limits<T>::epsilon()
            && std::fabs(a.m_values[1][1] - b.m_values[1][1]) <= std::numeric_limits<T>::epsilon()
            && std::fabs(a.m_values[1][2] - b.m_values[1][2]) <= std::numeric_limits<T>::epsilon()
            && std::fabs(a.m_values[2][0] - b.m_values[2][0]) <= std::numeric_limits<T>::epsilon()
            && std::fabs(a.m_values[2][1] - b.m_values[2][1]) <= std::numeric_limits<T>::epsilon()
            && std::fabs(a.m_values[2][2] - b.m_values[2][2]) <= std::numeric_limits<T>::epsilon();
    }
}