#include "systems/light/light_debug.h"

b8 joj::are_directional_lights_equals(DirectionalLight& l1, DirectionalLight& l2)
{
    if (l1.ambient.x != l2.ambient.x ||
        l1.ambient.y != l2.ambient.y ||
        l1.ambient.z != l2.ambient.z ||
        l1.ambient.w != l2.ambient.w)
    {
        return false;
    }

    if (l1.diffuse.x != l2.diffuse.x ||
        l1.diffuse.y != l2.diffuse.y ||
        l1.diffuse.z != l2.diffuse.z ||
        l1.diffuse.w != l2.diffuse.w)
    {
        return false;
    }

    if (l1.specular.x != l2.specular.x ||
        l1.specular.y != l2.specular.y ||
        l1.specular.z != l2.specular.z ||
        l1.specular.w != l2.specular.w)
    {
        return false;
    }

    if (l1.direction.x != l2.direction.x ||
        l1.direction.y != l2.direction.y ||
        l1.direction.z != l2.direction.z)
    {
        return false;
    }

    if (l1.pad != l2.pad)
    {
        return false;
    }

    return true;
}

b8 joj::are_point_lights_equals(PointLight& l1, PointLight& l2)
{
    if (l1.ambient.x != l2.ambient.x ||
        l1.ambient.y != l2.ambient.y ||
        l1.ambient.z != l2.ambient.z ||
        l1.ambient.w != l2.ambient.w)
    {
        return false;
    }

    if (l1.diffuse.x != l2.diffuse.x ||
        l1.diffuse.y != l2.diffuse.y ||
        l1.diffuse.z != l2.diffuse.z ||
        l1.diffuse.w != l2.diffuse.w)
    {
        return false;
    }

    if (l1.specular.x != l2.specular.x ||
        l1.specular.y != l2.specular.y ||
        l1.specular.z != l2.specular.z ||
        l1.specular.w != l2.specular.w)
    {
        return false;
    }

    if (l1.position.x != l2.position.x ||
        l1.position.y != l2.position.y ||
        l1.position.z != l2.position.z)
    {
        return false;
    }

    if (l1.range != l2.range)
    {
        return false;
    }

    if (l1.att.x != l2.att.x ||
        l1.att.y != l2.att.y ||
        l1.att.z != l2.att.z)
    {
        return false;
    }

    if (l1.pad != l2.pad)
    {
        return false;
    }

    return true;
}

b8 joj::are_spot_lights_equals(SpotLight& l1, SpotLight& l2)
{
    if (l1.ambient.x != l2.ambient.x ||
        l1.ambient.y != l2.ambient.y ||
        l1.ambient.z != l2.ambient.z ||
        l1.ambient.w != l2.ambient.w)
    {
        return false;
    }

    if (l1.diffuse.x != l2.diffuse.x ||
        l1.diffuse.y != l2.diffuse.y ||
        l1.diffuse.z != l2.diffuse.z ||
        l1.diffuse.w != l2.diffuse.w)
    {
        return false;
    }

    if (l1.specular.x != l2.specular.x ||
        l1.specular.y != l2.specular.y ||
        l1.specular.z != l2.specular.z ||
        l1.specular.w != l2.specular.w)
    {
        return false;
    }

    if (l1.position.x != l2.position.x ||
        l1.position.y != l2.position.y ||
        l1.position.z != l2.position.z)
    {
        return false;
    }

    if (l1.Range != l2.Range)
    {
        return false;
    }

    if (l1.direction.x != l2.direction.x ||
        l1.direction.y != l2.direction.y ||
        l1.direction.z != l2.direction.z)
    {
        return false;
    }

    if (l1.Spot != l2.Spot)
    {
        return false;
    }

    if (l1.att.x != l2.att.x ||
        l1.att.y != l2.att.y ||
        l1.att.z != l2.att.z)
    {
        return false;
    }

    if (l1.pad != l2.pad)
    {
        return false;
    }

    return true;
}