#include <pch.h>

#include "MyMath.h"

namespace GuGu{
    namespace math{
       float4x4 orthoProjD3DStyle(float left, float right, float bottom, float top, float zNear, float zFar)
       {
           float xScale = 1.0f / (right - left);
           float yScale = 1.0f / (top - bottom);
           float zScale = 1.0f / (zFar - zNear);
           return float4x4(
                   2.0f * xScale, 0, 0, 0,
                   0, 2.0f * yScale, 0, 0,
                   0, 0, zScale, 0,
                   -(left + right) * xScale, -(bottom + top) * yScale, -zNear * zScale, 1);
       }
      
       float4x4 orthoProjOGLStyle(float left, float right, float bottom, float top, float zNear, float zFar)
       {
           float xScale = 1.0f / (right - left);
           float yScale = 1.0f / (top - bottom);
           float zScale = 1.0f / (zFar - zNear);
           return float4x4(
                   2.0f * xScale, 0, 0, 0,
                   0, 2.0f * yScale, 0, 0,
                   0, 0, -2.0f * zScale, 0,
                   -(left + right) * xScale, -(bottom + top) * yScale, -(zNear + zFar) * zScale, 1);
       }
      
       float4x4 perspProjD3DStyle(float left, float right, float bottom, float top, float zNear, float zFar)
       {
           float xScale = 1.0f / (right - left);
           float yScale = 1.0f / (top - bottom);
           float zScale = 1.0f / (zFar - zNear);
           return float4x4(
                   2.0f * xScale, 0, 0, 0,
                   0, 2.0f * yScale, 0, 0,
                   -(left + right) * xScale, -(bottom + top) * yScale, zFar * zScale, 1,
                   0, 0, -zNear * zFar * zScale, 0);
       }
      
       float4x4 perspProjOGLStyle(float left, float right, float bottom, float top, float zNear, float zFar)
       {
           float xScale = 1.0f / (right - left);
           float yScale = 1.0f / (top - bottom);
           float zScale = 1.0f / (zFar - zNear);
           return float4x4(
                   2.0f * zNear * xScale, 0, 0, 0,
                   0, 2.0f * zNear * yScale, 0, 0,
                   (left + right) * xScale, (bottom + top) * yScale, -(zNear + zFar) * zScale, -1,
                   0, 0, -2.0f * zNear * zFar * zScale, 0);
       }
      
       float4x4 perspProjD3DStyleReverse(float left, float right, float bottom, float top, float zNear)
       {
           float xScale = 1.0f / (right - left);
           float yScale = 1.0f / (top - bottom);
      
           return float4x4(
                   2.0f * xScale, 0, 0, 0,
                   0, 2.0f * yScale, 0, 0,
                   -(left + right) * xScale, -(bottom + top) * yScale, 0, 1,
                   0, 0, zNear, 0);
       }
      
       float4x4 perspProjD3DStyle(float verticalFOV, float aspect, float zNear, float zFar)
       {
           //观察矩阵的we是1，xe，ye在[l, r], [b, t]之间，而ze在[n, f]之间
		   //这个投影矩阵，是把[n, f]转换成[0, 1], [l, r]，[b, t]转成[-1, 1]
		   //注意，矩阵这里是列主序，并且C++这里是左乘，向量左乘矩阵，会按列相乘再相加
		   //在shader里面，也是左乘，shader会将向量和每一行左乘，但是shader会将矩阵的列转换成行
           float yScale = 1.0f / tanf(0.5f * verticalFOV);
           float xScale = yScale / aspect;
           float zScale = 1.0f / (zFar - zNear);
           return float4x4(
                   xScale, 0, 0, 0,
                   0, yScale, 0, 0,
                   0, 0, zFar * zScale, 1,
                   0, 0, -zNear * zFar * zScale, 0);
       }
      
       float4x4 perspProjOGLStyle(float verticalFOV, float aspect, float zNear, float zFar)
       {
           float yScale = 1.0f / tanf(0.5f * verticalFOV);
           float xScale = yScale / aspect;
           float zScale = 1.0f / (zFar - zNear);
           return float4x4(
                   xScale, 0, 0, 0,
                   0, yScale, 0, 0,
                   0, 0, -(zNear + zFar) * zScale, -1,
                   0, 0, -2.0f * zNear * zFar * zScale, 0);
       }
      
       float4x4 perspProjD3DStyleReverse(float verticalFOV, float aspect, float zNear)
       {
           float yScale = 1.0f / tanf(0.5f * verticalFOV);
           float xScale = yScale / aspect;
      
           return float4x4(
                   xScale, 0, 0, 0,
                   0, yScale, 0, 0,
                   0, 0, 0, 1,
                   0, 0, zNear, 0);
       }
       float4x4 viewportMatrix(float width, float height)
       {
           float4x4 viewportM(
               width / 2.0f, 0.0f, 0.0f, width / 2.0f,
               0.0f, -height / 2.0f, 0.0f, height / 2.0f,
               0, 0, 1, 0,
               0, 0, 0, 1
           );
           return viewportM;
       }
    }
}