#include "primitive.h"
#include "bsdf.h"

namespace csupbr {

    ///////////////////////////
    /// LambertianReflection
    ///////////////////////////
    
    void LambertianReflection::sample(BSDFRecord &bsdfRec, const Point2f &sample, Float &pdf) const {
        // 在这写代码
        // ...

        // 均匀半球采样
        Vec3f uniformSampling = uniformHemisphereSampling(sample);
        // bsdfRec.wi = bsdfRec.isct.shCoord.toWorld(uniformSampling);


        // 余弦半球采样
        Vec3f cosSampling = cosineHemisphereSampling(sample);
        bsdfRec.wi = bsdfRec.isct.shCoord.toWorld(cosSampling);

        bsdfRec.wi.normalize();
        pdf = this->pdf(bsdfRec);
    }

    Spectrum LambertianReflection::evaluate(BSDFRecord &bsdfRec, Float &pdf) const {
        pdf = this->pdf(bsdfRec);
        return {};
    }

    Float LambertianReflection::pdf(const BSDFRecord &bsdfRec) const {
        return InvPi2;
    }

    ///////////////////////////
    /// SpecularReflection
    ///////////////////////////

    void SpecularReflection::sample(BSDFRecord &bsdfRec, const Point2f &sample, Float &pdf) const {
        // 在这写代码
        // ...

        //reflect求反射向量，利用roughness进行模糊
        Vec3f reflected = random_in_unit_ball() * roughness + reflect(-bsdfRec.wo, bsdfRec.isct.shN);
        bsdfRec.wi = reflected;
        
        bsdfRec.wi.normalize();
        pdf = 1.f;
    }

    Spectrum SpecularReflection::evaluate(BSDFRecord &bsdfRec, Float &pdf) const {
        pdf = this->pdf(bsdfRec);
        return {};
    }

    Float SpecularReflection::pdf(const BSDFRecord &bsdfRec) const {
        return .0f;
    }

    ///////////////////////////
    /// SpecularTransmission
    ///////////////////////////

    void SpecularTransmission::sample(BSDFRecord &bsdfRec, const Point2f &sample, Float &pdf) const {
        // 在这写代码
        // ...

        bsdfRec.wo.normalize();
        float refract_index_item = refract_rate;
        Vec3f normal = bsdfRec.isct.shN;
        if(dot(bsdfRec.isct.shN, -bsdfRec.wo)<0.f){
            refract_index_item = 1 / refract_index_item;
        } else{
            normal = -normal;
        }
        float cos_theta = fmin(fabs(dot(bsdfRec.wo, normal)), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        bool can_not_refract = (refract_index_item * sin_theta > 1.0);
        float rand_f = rand() / (RAND_MAX + 1.0f);
        if(schlick(cos_theta, refract_index_item) > rand_f || can_not_refract){
            bsdfRec.wi = reflect(-bsdfRec.wo, normal);
        } else{
            refract(-bsdfRec.wo, bsdfRec.wi, normal, refract_index_item);
        }
        
        bsdfRec.wi.normalize();
        pdf = 1.f;
    }

    Spectrum SpecularTransmission::evaluate(BSDFRecord &bsdfRec, Float &pdf) const {
        pdf = this->pdf(bsdfRec);
        return {};
    }

    Float SpecularTransmission::pdf(const BSDFRecord &bsdfRec) const {
        return .0f;
    }

}
