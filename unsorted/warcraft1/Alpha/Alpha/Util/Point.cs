// Copyright (C) 2006 Crestez Leonard    cleonard@go.ro
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

namespace Alpha.Util
{
    // This is a simple point class.
    public struct Point
    {
        // X coord.
        private int x;
        public int X {
            get {
                return x;
            }
            set {
                x = value;
            }
        }

        // Y coord.
        private int y;
        public int Y {
            get {
                return y;
            }
            set {
                y = value;
            }
        }

        // Set both x and y.
        public void Set(int x, int y)
        {
            this.X = x;
            this.Y = y;
        }

        // Overload ==
        public static bool operator==(Point a, Point b)
        {
            return (a.X == b.X) && (a.Y == b.Y);
        }

        // Overload !=
        public static bool operator!=(Point a, Point b)
        {
            return (a.X != b.X) || (a.Y != b.Y);
        }

        // If two points are equal.
        public override bool Equals(object other)
        {
            return (other is Point) && this == (Point)other;
        }

        // Hash value. FIXME: crappy value.
        public override int GetHashCode()
        {
            return X ^ Y;
        }

        // Constructor with default values.
        public Point(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        // Convert to string for formatting.
        public override string ToString()
        {
            return String.Format("{{X = {0}, Y = {1}}}", X, Y);
        }

        public static Point operator+(Point p1, Point p2)
        {
            return new Point(p1.X + p2.X, p1.Y + p2.Y);
        }

        public static Point operator-(Point p1, Point p2)
        {
            return new Point(p1.X - p2.X, p1.Y - p2.Y);
        }

        // Implicit conversion into SD.Point
        public static implicit operator System.Drawing.Point(Alpha.Util.Point p)
        {
            return new System.Drawing.Point(p.X, p.Y);
        }

        // Implicit conversion from SD.Point
        public static implicit operator Alpha.Util.Point(System.Drawing.Point p)
        {
            return new Alpha.Util.Point(p.X, p.Y);
        }

        // Explicit conversion from AU.PointF
        public static explicit operator Alpha.Util.Point(Alpha.Util.PointF p)
        {
            return new Alpha.Util.Point((int)p.X, (int)p.Y);
        }

        // Explicit conversion from AU.PointD
        public static explicit operator Alpha.Util.Point(Alpha.Util.PointD p)
        {
            return new Alpha.Util.Point((int)p.X, (int)p.Y);
        }
    }

    // This is a simple point class.
    // This is floating point, so stuff like GetHashCode and Equals
    // is rather meaningless.
    public struct PointF
    {
        // X coord.
        private float x;
        public float X {
            get {
                return x;
            }
            set {
                x = value;
            }
        }

        // Y coord.
        private float y;
        public float Y {
            get {
                return y;
            }
            set {
                y = value;
            }
        }

        // Default constructor.
        public void Set(float x, float y)
        {
            this.X = x;
            this.Y = y;
        }

        // Constructor with default values.
        public PointF(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        // Convert to string for formatting.
        public override string ToString()
        {
            return String.Format("{{X = {0}, Y = {1}}}", X, Y);
        }

        public static PointF operator+(PointF p1, PointF p2)
        {
            return new PointF(p1.X + p2.X, p1.Y + p2.Y);
        }

        public static PointF operator-(PointF p1, PointF p2)
        {
            return new PointF(p1.X - p2.X, p1.Y - p2.Y);
        }

        // Implicit conversion into SD.PointF
        public static implicit operator System.Drawing.PointF(Alpha.Util.PointF p)
        {
            return new System.Drawing.PointF(p.X, p.Y);
        }

        // Implicit conversion from SD.PointF
        public static implicit operator Alpha.Util.PointF(System.Drawing.PointF p)
        {
            return new Alpha.Util.PointF(p.X, p.Y);
        }

        // Implicit conversion from AU.Point
        public static implicit operator Alpha.Util.PointF(Alpha.Util.Point p)
        {
            return new Alpha.Util.PointF(p.X, p.Y);
        }

        // Explicit conversion from AU.PointD
        public static explicit operator Alpha.Util.PointF(Alpha.Util.PointD p)
        {
            return new Alpha.Util.PointF((float)p.X, (float)p.Y);
        }
    }

    // This is a simple point class.
    // This is floating point, so stuff like GetHashCode and Equals
    // is rather meaningless.
    //
    // unlike PointF, this uses doubles.
    public struct PointD
    {
        // X coord.
        private double x;
        public double X {
            get {
                return x;
            }
            set {
                x = value;
            }
        }

        // Y coord.
        private double y;
        public double Y {
            get {
                return y;
            }
            set {
                y = value;
            }
        }

        // Default constructor.
        public void Set(double x, double y)
        {
            this.X = x;
            this.Y = y;
        }

        // Constructor with default values.
        public PointD(double x, double y)
        {
            this.x = x;
            this.y = y;
        }

        // Convert to string for formatting.
        public override string ToString()
        {
            return String.Format("{{X = {0}, Y = {1}}}", X, Y);
        }

        public static PointD operator+(PointD p1, PointD p2)
        {
            return new PointD(p1.X + p2.X, p1.Y + p2.Y);
        }

        public static PointD operator-(PointD p1, PointD p2)
        {
            return new PointD(p1.X - p2.X, p1.Y - p2.Y);
        }

        // Explicit conversion into SD.PointF (lossy).
        public static explicit operator System.Drawing.PointF(Alpha.Util.PointD p)
        {
            return new System.Drawing.PointF((float)p.X, (float)p.Y);
        }

        // Implicit conversion from SD.PointF
        public static implicit operator Alpha.Util.PointD(System.Drawing.PointF p)
        {
            return new Alpha.Util.PointD(p.X, p.Y);
        }

        // Implicit conversion from AU.PointF
        public static implicit operator Alpha.Util.PointD(Alpha.Util.Point p)
        {
            return new Alpha.Util.PointF(p.X, p.Y);
        }

        // Implicit conversion from AU.PointD
        public static implicit operator Alpha.Util.PointD(Alpha.Util.PointF p)
        {
            return new Alpha.Util.PointD(p.X, p.Y);
        }
    }
}
