#ifndef DEF_SPLINE_H
#define DEF_SPLINE_H

#include "CatmullRom.h"

struct SplineComponent {
    CatmullRom3D spline;
    float t = 0.f;

	SplineComponent() = default;

    SplineComponent(const CatmullRom3D& spline)
    : spline{ spline }
    , t{ 0.f } {

    }

	SplineComponent(const CatmullRom3D& spline_, const Transform& transform)
		: t{ 0.f } {
		spline = spline_;
		spline.update_points(transform);
	}

    SplineComponent(CatmullRom3D&& spline) noexcept
    : spline{ std::move(spline) }
    , t{ 0.f } {

    }
};

#endif