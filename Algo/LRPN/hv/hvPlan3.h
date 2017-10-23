/*******************************************************************************
* ASTex:                                                                       *
* Copyright (C) IGG Group, ICube, University of Strasbourg, France             *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: https://astex-icube.github.io                                      *
* Contact information: astex@icube.unistra.fr                                  *
*                                                                              *
*******************************************************************************/



// plan3.h: interface for the plan3 class.
//
// Defines a plane in 3D space, it is defined by equation: aX+bY+cZ+d=0
//
// By JMD 10/8/06
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAN3_H__7DA1FB24_546C_442A_A00F_9F3F688FEBCD__INCLUDED_)
#define AFX_PLAN3_H__7DA1FB24_546C_442A_A00F_9F3F688FEBCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace hview {

#include "hvLine3.h"

	template <class T> class plan3
	{
	protected:
		T a, b, c, d;   	/* ax + by + cz +d =0 */

	public:
		// constructor
		plan3<T>() { a = T(0); b = T(0); c = T(0); d = T(0); }

		// defines a plane with one point p and two vectors u and v
		// p is the origin, u and v must be two non-colinear vectors
		plan3<T>(const hvVec3<T> &p, const hvVec3<T> &u, const hvVec3<T> &v)
		{
			hvVec3<T> n; n.cross(u, v);
			n.normalize(n.norm());
			a = n.X(); b = n.Y(); c = n.Z();
			d = -n.dot(p);
		}

		// defines a plane with a point p and a normal vector n
		// p is the origin, n is normal vector to plane
		plan3<T>(const hvVec3<T> &p, const hvVec3<T> &n)
		{
			a = n.X(); b = n.Y(); c = n.Z();
			d = -n.dot(p);
		}

		// get the plane normal
		hvVec3<T> normal() const { return hvVec3<T>(a, b, c); }

		// compute plane equation using p coordinates, e.g. ap.X+bp.Y+cp.Z+d
		// result is scalar value: 0 means p is exactly on the plane
		T pointInPlan(const hvVec3<T> &p) const
		{
			return d + p.dot(normal());
		}

		void reverse() { a = -a; b = -b; c = -c; d = -d; }

	};

}

#endif // !defined(AFX_PLAN3_H__7DA1FB24_546C_442A_A00F_9F3F688FEBCD__INCLUDED_)
