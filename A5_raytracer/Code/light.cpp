#include"light.h"
#include<sstream>
#include<string>
#include<cmath>
#include<cstdlib>

#define ran() ( double( rand() % 32768 ) / 32768 )

Light::Light() {
    sample = rand();
    next = NULL;
    lightPrimitive = NULL;
}

void Light::Input(std::string var, std::stringstream &fin) {
    if (var == "color=") color.Input(fin);
}

void PointLight::Input(std::string var, std::stringstream &fin) {
    if (var == "O=") O.Input(fin);
    Light::Input(var, fin);
}


double PointLight::CalnShade(Vector3 C, Primitive *primitive_head, int shade_quality) {
    Vector3 V = O - C;
    double dist = V.Module();
    for (Primitive *now = primitive_head; now != NULL; now = now->GetNext()) {
        CollidePrimitive tmp = now->Collide(C, V);
        if (EPS < (dist - tmp.dist)) return 0;
    }

    return 1;
}

void SquareLight::Input(std::string var, std::stringstream &fin) {
    if (var == "O=") O.Input(fin);
    if (var == "Dx=") Dx.Input(fin);
    if (var == "Dy=") Dy.Input(fin);
    Light::Input(var, fin);
}


double SquareLight::CalnShade(Vector3 C, Primitive *primitive_head, int shade_quality) {
	//TODO: NEED TO IMPLEMENT

    // 在矩形面上采样若干个点，判断点到交点之间是否存在物体
    Vector3 xDir = Dx.GetUnitVector(), yDir = Dy.GetUnitVector(); // 横纵坐标轴
    double length = Dx.Module(), width = Dy.Module();

    int shadeCount = 0; // 被遮挡的采样点个数
    int sampleCount = shade_quality;
    for (int i = 0; i < sampleCount; i++) {
        double x = length * (2.0 * ran() - 1.0), y = width * (2.0 * ran() - 1.0);
        Vector3 lightPoint = O + x * xDir + y * yDir;
        Vector3 V = lightPoint - C;
        double dist = V.Module();
        for (Primitive *now = primitive_head; now != NULL; now = now->GetNext()) {
            CollidePrimitive tmp = now->Collide(C, V);
            if (tmp.isCollide && EPS < (dist - tmp.dist)) {
                shadeCount++;
                break;
            }
        }
    }
    double shade = 1.0 - (double) shadeCount / (double) sampleCount;
    return shade;
}

Primitive *SquareLight::CreateLightPrimitive() {
    PlaneAreaLightPrimitive *res = new PlaneAreaLightPrimitive(O, Dx, Dy, color);
    lightPrimitive = res;
    return res;
}


void SphereLight::Input(std::string var, std::stringstream &fin) {
    if (var == "O=") O.Input(fin);
    if (var == "R=") fin >> R;
    Light::Input(var, fin);
}


double SphereLight::CalnShade(Vector3 C, Primitive *primitive_head, int shade_quality) {
    //TODO: NEED TO IMPLEMENT

    // 在与交点到球心方向垂直的圆面上采样若干个点，判断点到交点之间是否存在物体
    Vector3 zDir = (C - O).GetUnitVector();
    Vector3 xDir = zDir.GetAnVerticalVector(); // 随便选一个与OC向量垂直的向量作为圆面的x轴
    Vector3 yDir = (zDir * xDir).GetUnitVector();

    int shadeCount = 0; // 被遮挡的采样点个数
    int sampleCount = shade_quality;
    // 判断是否碰撞的物体中有光源
    for (int i = 0; i < sampleCount; i++) {
        // 极坐标的r要开根的原因：
        // https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
        double radius = R * sqrt(ran()), theta = 2 * PI * ran();
        // 将极坐标转成笛卡尔坐标，便于使用坐标轴向量获得空间上的点
        double x = radius * cos(theta), y = radius * sin(theta);

        double phi = acos(x / R);
        double z = R * sin(phi);

        Vector3 lightPoint = O + x * xDir + y * yDir + z * zDir;
        Vector3 V = lightPoint - C;
        double dist = V.Module();
        for (Primitive *now = primitive_head; now != NULL; now = now->GetNext()) {
            CollidePrimitive tmp = now->Collide(C, V);
            if (tmp.isCollide && EPS < (dist - tmp.dist)) {
                shadeCount++;
                break;
            }
        }
    }
    double shade = 1.0 - (double) shadeCount / (double) sampleCount;
    return shade;
}


Primitive *SphereLight::CreateLightPrimitive() {
    SphereLightPrimitive *res = new SphereLightPrimitive(O, R, color);
    lightPrimitive = res;
    return res;
}

