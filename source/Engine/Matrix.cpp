/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Matrix.hpp"
#include <cmath>

namespace Math
{
    TransformationMatrix::TransformationMatrix()
    {
        Reset();
    }

    TransformationMatrix TransformationMatrix::operator*(TransformationMatrix m) const
    {
        TransformationMatrix result;

        result.matrix[0][0] = matrix[0][0] * m[0][0] + matrix[0][1] * m[1][0] + matrix[0][2] * m[2][0];
        result.matrix[0][1] = matrix[0][0] * m[0][1] + matrix[0][1] * m[1][1] + matrix[0][2] * m[2][1];
        result.matrix[0][2] = matrix[0][0] * m[0][2] + matrix[0][1] * m[1][2] + matrix[0][2] * m[2][2];
        result.matrix[1][0] = matrix[1][0] * m[0][0] + matrix[1][1] * m[1][0] + matrix[1][2] * m[2][0];
        result.matrix[1][1] = matrix[1][0] * m[0][1] + matrix[1][1] * m[1][1] + matrix[1][2] * m[2][1];
        result.matrix[1][2] = matrix[1][0] * m[0][2] + matrix[1][1] * m[1][2] + matrix[1][2] * m[2][2];
        result.matrix[2][0] = matrix[2][0] * m[0][0] + matrix[2][1] * m[1][0] + matrix[2][2] * m[2][0];
        result.matrix[2][1] = matrix[2][0] * m[0][1] + matrix[2][1] * m[1][1] + matrix[2][2] * m[2][1];
        result.matrix[2][2] = matrix[2][0] * m[0][2] + matrix[2][1] * m[1][2] + matrix[2][2] * m[2][2];

        return result;
    }

    TransformationMatrix& TransformationMatrix::operator*=(TransformationMatrix m)
    {
        (*this) = (*this) * m;
        return (*this);
    }

    vec2 TransformationMatrix::operator*(vec2 v) const
    {
        Math::vec2 result;
        result.x = matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2];
        result.y = matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2];
        return result;
    }

    void TransformationMatrix::Reset()
    {
        matrix[0][0] = 1;
        matrix[1][1] = 1;
        matrix[2][2] = 1;
        matrix[0][1] = 0;
        matrix[1][0] = 0;
        matrix[0][2] = 0;
        matrix[2][0] = 0;
        matrix[1][2] = 0;
        matrix[2][1] = 0;
    }

    TranslationMatrix::TranslationMatrix(ivec2 translate)
    {
        matrix[0][0] = 1;
        matrix[1][1] = 1;
        matrix[2][2] = 1;
        matrix[0][1] = 0;
        matrix[1][0] = 0;
        matrix[0][2] = translate.x;
        matrix[2][0] = 0;
        matrix[1][2] = translate.y;
        matrix[2][1] = 0;
    }

    TranslationMatrix::TranslationMatrix(vec2 translate)
    {
        matrix[0][0] = 1;
        matrix[1][1] = 1;
        matrix[2][2] = 1;
        matrix[0][1] = 0;
        matrix[1][0] = 0;
        matrix[0][2] = translate.x;
        matrix[2][0] = 0;
        matrix[1][2] = translate.y;
        matrix[2][1] = 0;
    }

    RotationMatrix::RotationMatrix(double theta)
    {
        double cosine = cos(theta);
        double sine   = sin(theta);

        matrix[0][0] = cosine;
        matrix[0][1] = -sine;
        matrix[0][2] = 0;
        matrix[1][0] = sine;
        matrix[1][1] = cosine;
        matrix[1][2] = 0;
        matrix[2][0] = 0;
        matrix[2][1] = 0;
        matrix[2][2] = 1;
    }

    ScaleMatrix::ScaleMatrix(double scale)
    {
        matrix[0][0] = scale;
        matrix[0][1] = 0;
        matrix[0][2] = 0;
        matrix[1][0] = 0;
        matrix[1][1] = scale;
        matrix[1][2] = 0;
        matrix[2][0] = 0;
        matrix[2][1] = 0;
        matrix[2][2] = 1;
    }

    ScaleMatrix::ScaleMatrix(vec2 scale)
    {
        matrix[0][0] = scale.x;
        matrix[0][1] = 0;
        matrix[0][2] = 0;
        matrix[1][0] = 0;
        matrix[1][1] = scale.y;
        matrix[1][2] = 0;
        matrix[2][0] = 0;
        matrix[2][1] = 0;
        matrix[2][2] = 1;
    }
}
