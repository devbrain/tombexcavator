// Copyright (C) 2004-2006 Crestez Leonard    cleonard@go.ro
//
// This file is part of Alpha
//
// Alpha is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Alpha is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Alpha; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.Drawing;

namespace Alpha.Util
{
    // This class contains some small integer-only mathematical functions
    // that generally use floating point. They're used in the gamestate, where
    // floating point can't be used because of possible syncronisation problems.
    public class IntMath
    {
        // Integer square root function, guaranteed to work the exact same way
        // on different platform. Safe for networking.
        // Uses John Halleck's method, see
        // http://www.cc.utah.edu/~nahaj/factoring/isqrt.legalize.c.html
        // Taken from stratagus sourcecode.
        public static int Sqrt(int num)
        {
            int squaredbit;
            int remainder;
            int root;

            if (num < 1) {
                return 0;
            }

            // Load the binary constant 01 00 00 ... 00, where the number
            // of zero bits to the right of the single one bit
            // is even, and the one bit is as far left as is consistant
            // with that condition.)
            //
            // This portable load replaces the loop that used to be
            // here, and was donated by  legalize@xmission.com
            squaredbit  = (int)((unchecked((uint)~0) >> 1) & ~(unchecked((uint)~0) >> 2));

            // Form bits of the answer.
            remainder = num;
            root = 0;
            while (squaredbit > 0) {
                if (remainder >= (squaredbit | root)) {
                    remainder -= (squaredbit | root);
                    root >>= 1;
                    root |= squaredbit;
                } else {
                    root >>= 1;
                }
                squaredbit >>= 2;
            }

            return root;
        }

        // Integer distance from ax, ay to bx, by.
        // This uses integer Sqrt, as expected.
        public static int Distance(int ax, int ay, int bx, int by)
        {
            return IntMath.Sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
        }

        // Integer distance from a to b
        // This uses integer Sqrt, as expected.
        public static int Distance(System.Drawing.Point a, System.Drawing.Point b)
        {
            return IntMath.Sqrt((a.X - b.X) * (a.X - b.X) + (a.Y - b.Y) * (a.Y - b.Y));
        }

        // Integer distance from a to b
        // This uses integer Sqrt, as expected.
        public static int Distance(Point a, Point b)
        {
            return IntMath.Sqrt((a.X - b.X) * (a.X - b.X) + (a.Y - b.Y) * (a.Y - b.Y));
        }

        // Returns the orientation of a vector x, y as an int.
        // It returns an angle from 0 to 255, you could say it gives
        // you angle * 256 / 2 * Math.PI, with angle in radians.
        //
        // All calculations are done using only ints, so calculations are reliable
        // to give the exact same result no matter the platform.
        //
        // The angle is measured counter-clockwise, with 0 meaning x, y = 1, 0.
        // This is standard trigonometric notation, it's NOT like Entity.Orientation.
        public static int GetOrientation(int x, int y)
        {
            // Handle x == 0 and y == 0/
            if (x == 0 && y == 0) {
                throw new ArgumentException("Direction vector can't be 0, 0");
            }
            // Quadrant 1
            if (x > 0 && y >= 0) {
                return DoGetOrientation(x, y);
            }
            // Quadrant 2
            if (x <= 0 && y > 0) {
                return 64 + DoGetOrientation(y, -x);
            }
            // Quadrant 3
            if (x < 0 && y <= 0) {
                return 128 + DoGetOrientation(-x, -y);
            }
            // Quadrant 4
            if (x >= 0 && y < 0) {
                return 192 + DoGetOrientation(-y, x);
            }
            throw new Exception(String.Format("Vector {0}, {1} not handled.", x, y));
        }

        // This is the ReferenceLength to use in orientation calculations.
        // Bigger values provide better precission but slower calculations.
        private const int RefLength = 10000;

        // Same as GetOrientation but assumes first quadrant.
        private static int DoGetOrientation(int x, int y)
        {
            int ax, ay, bx, by, cx, cy, cl;
            int res = 0, jmp;
            ax = RefLength;
            ay = 0;
            bx = 0;
            by = RefLength;
            for (jmp = 32; jmp > 0; jmp /= 2) {
                cx = (ax + bx) / 2;
                cy = (ay + by) / 2;
                cl = IntMath.Sqrt(cx * cx + cy * cy);
                cx = cx * RefLength / cl;
                cy = cy * RefLength / cl;
                if (cy * x <= cx * y) {
                    ax = cx;
                    ay = cy;
                    res += jmp;
                } else {
                    bx = cx;
                    by = cy;
                }
            }
            return res;
        }
    }
}
