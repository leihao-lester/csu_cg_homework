#include "../core/shape.h"

#include "../core/primitive.h"

namespace csupbr {

    bool Sphere::intersect(const Ray &ray, Intersection &isct) const {
        // 转换到局部坐标
        Ray oRay = (*w2o)(ray);
        // 请使用 'oRay' 而不是 'ray'
        // 请计算以下的值
        float hit_t = 0.f;                      // 射线t
        Point3f hit_point;                      // 相交点
        // 法向量
        Normal geometryNormal = {0, 0, 1.f};
        Normal shadingNormal = {0, 0, 1.f};
        Point2f uv;                             // 纹理坐标
        // ...

        // 射线与球相交的方程为t^2*d*d+2*t*d*origin+origin-r*r=0
        float a = dot(oRay.direct);
        float b = 2.0f * dot(oRay.origin, oRay.direct);
        float c = dot(oRay.origin) - radius * radius;
        float determination = b * b - 4.0f * a * c;
        // Δ<0,无交点
        if(determination < 0.0f){
            return false;
        }
        float sqrtd = std::sqrt(determination);
        // 获得较小根
        float root = (-b - sqrtd) / (2.0f * a);
        if(root < oRay.mint || root > oRay.maxt){
            root = (-b + sqrtd) / (2.0f * a);
            if(root < oRay.mint || root > oRay.maxt){
                return false;
            }
        }
        hit_t = root;
        hit_point = oRay.to(root);
        // 法线方向与光线从外部还是从内部与球体相交有关
        geometryNormal = dot(hit_point,oRay.direct) < 0.0f ? hit_point / radius : -hit_point / radius;
        shadingNormal = hit_point / radius;
        uv.u = atan2f(hit_point.z,hit_point.x) / 2.0f / Pi + 0.5f;
        uv.v = asinf(std::clamp(hit_point.y,-1.0f,1.f)) / Pi;

        // ***以下代码不要动***
        // 转换到世界坐标
        isct.pHit = (*o2w)(hit_point);
        isct.geoN = normalize((*o2w)(geometryNormal));
        isct.shN = normalize((*o2w)(shadingNormal));
        isct.uv = uv;
        isct.geoCoord = CoordinateSystem(isct.geoN);
        isct.shCoord = CoordinateSystem(isct.shN);
        ray.maxt = hit_t;

        // 不要忘记返回是否相交
        return true;
        // return false;
    }

    bool Sphere::intersectP(const Ray &ray, bool blend_test) const {
        
        // 只需计算是否相交即可 (blend_test不用管)
        // ...


        // 不要忘记返回是否相交
        // return false;

        Ray oRay = (*w2o)(ray);
        // 射线与球相交的方程为t^2*d*d+2*t*d*origin+origin-r*r=0
        float a = dot(oRay.direct);
        float b = 2.0f * dot(oRay.origin, oRay.direct);
        float c = dot(oRay.origin) - radius * radius;
        float determination = b * b - 4.0f * a * c;
        // Δ<0,无交点
        if(determination < 0.0f){
            return false;
        }
        float sqrtd = std::sqrt(determination);
        // 获得较小根
        float root = (-b - sqrtd) / (2.0f * a);
        if(root < oRay.mint || root > oRay.maxt){
            root = (-b + sqrtd) / (2.0f * a);
            if(root < oRay.mint || root > oRay.maxt){
                return false;
            }
        }
        return true;
    }
    
    BBox3f Sphere::getBBox() const {
        return (*o2w)(BBox3f({-radius, -radius, -radius}, {radius, radius, radius}));
    }

}