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
    // Simple int rectangle struct.
    // Defined as two points.
    //
    // There is no need for Corner1 to be smaller than Corner2.
    // If you want that, call .Normalize()
    public struct Rectangle
    {
        // One corner of the rectangle.
        private Point corner1;
        public Point Corner1 {
            get {
                return corner1;
            }
            set {
                corner1 = value;
            }
        }

        // Corner1.X
        public int X1 {
            get {
                return Corner1.X;
            }
            set {
                corner1.X = value;
            }
        }

        // Corner1.Y
        public int Y1 {
            get {
                return Corner1.Y;
            }
            set {
                corner1.Y = value;
            }
        }

        // The other corner of the rectangle.
        private Point corner2;
        public Point Corner2 {
            get {
                return corner2;
            }
            set {
                corner2 = value;
            }
        }

        // Corner2.X
        public int X2 {
            get {
                return Corner2.X;
            }
            set {
                corner2.X = value;
            }
        }

        // Corner2.Y
        public int Y2 {
            get {
                return Corner2.Y;
            }
            set {
                corner2.Y = value;
            }
        }

        // Overload ==
        public static bool operator==(Rectangle a, Rectangle b)
        {
            return (a.Corner1 == b.Corner1) && (a.Corner2 == b.Corner2);
        }

        // Overload !=
        public static bool operator!=(Rectangle a, Rectangle b)
        {
            return (a.Corner1 != b.Corner1) || (a.Corner2 != b.Corner2);
        }

        // If two points are equal.
        public override bool Equals(object other)
        {
            return (other is Rectangle) && this == (Rectangle)other;
        }

        // Hash value. FIXME: crappy value.
        public override int GetHashCode()
        {
            return X1 ^ Y2 + X2 ^ Y1;
        }

        // Normalize the rectangle
        // Ensure X1 <= X2 and Y1 <= Y2
        public Rectangle Normalize()
        {
            return new Rectangle(Math.Min(X1, X2), Math.Min(Y1, Y2), Math.Max(X1, X2), Math.Max(Y1, Y2));
        }

        // Width of the rectangle, X2 - X1
        // Might be negative, normalize to change.
        public int Width {
            get {
                return X2 - X1;
            }
        }

        // Absolute height of the rectangle.
        public int Height {
            get {
                return Y2 - Y1;
            }
        }

        // Returns true if the rectangle strictly includes another rectangle.
        public bool StrictlyIntersects(Rectangle r)
        {
            return X1 < r.X2 && r.X1 < X2 && Y1 < r.Y2 && r.Y1 < Y2;
        }

        // Returns true if the rectangle strictly includes another rectangle.
        public bool StrictlyIncludes(Rectangle r)
        {
            return X1 < r.X1 && r.X2 < X2 && Y1 < r.Y1 && r.Y2 < Y2;
        }

        // Returns true if the rectangle strictly includes another rectangle.
        public bool StrictlyIncludes(Point p)
        {
            return X1 < p.X && p.X < X2 && Y1 < p.Y && p.Y < Y2;
        }

        // Format as a string.
        public override string ToString()
        {
            return String.Format("{{X1 = {0}; Y1 = {1}; X2 = {2}; Y2 = {3}}}", X1, Y1, X2, Y2);
        }

        // Constructor, should be obvious.
        public Rectangle(int x1, int y1, int x2, int y2)
        {
            this.corner1 = new Point(x1, y1);
            this.corner2 = new Point(x2, y2);
        }

        // Create from 2 points.
        public Rectangle(Point p1, Point p2): this(p1.X, p1.Y, p2.X, p2.Y)
        {
        }

        // Create a rectangle from a corner and a size.
        public static Rectangle FromXYWidthHeight(int x, int y, int width, int height)
        {
            return new Rectangle(x, y, x + width, y + height);
        }

        // Implicit conversion into SD.Rectangle
        public static implicit operator System.Drawing.Rectangle(Alpha.Util.Rectangle r)
        {
            return System.Drawing.Rectangle.FromLTRB(r.X1, r.Y1, r.X2, r.Y2);
        }

        // Implicit conversion from SD.Rectangle
        public static implicit operator Alpha.Util.Rectangle(System.Drawing.Rectangle r)
        {
            return new Alpha.Util.Rectangle(r.X, r.Y, r.X + r.Width, r.Y + r.Height);
        }

        // Implicit conversion into RectangleF
        public static implicit operator Alpha.Util.RectangleF(Alpha.Util.Rectangle r)
        {
            return new Alpha.Util.RectangleF(r.X1, r.Y1, r.X2, r.Y2);
        }

        // Implicit conversion into RectangleD
        public static implicit operator Alpha.Util.RectangleD(Alpha.Util.Rectangle r)
        {
            return new Alpha.Util.RectangleD(r.X1, r.Y1, r.X2, r.Y2);
        }
    }

    // Simple float rectangle struct.
    // Defined as two points.
    //
    // There is no need for Corner1 to be smaller than Corner2.
    // If you want that, call .Normalize()
    public struct RectangleF
    {
        // One corner of the rectangle.
        private PointF corner1;
        public PointF Corner1 {
            get {
                return corner1;
            }
            set {
                corner1 = value;
            }
        }

        // Corner1.X
        public float X1 {
            get {
                return Corner1.X;
            }
            set {
                corner1.X = value;
            }
        }

        // Corner1.Y
        public float Y1 {
            get {
                return Corner1.Y;
            }
            set {
                corner1.Y = value;
            }
        }

        // The other corner of the rectangle.
        private PointF corner2;
        public PointF Corner2 {
            get {
                return corner2;
            }
            set {
                corner2 = value;
            }
        }

        // Corner2.X
        public float X2 {
            get {
                return Corner2.X;
            }
            set {
                corner2.X = value;
            }
        }

        // Corner2.Y
        public float Y2 {
            get {
                return Corner2.Y;
            }
            set {
                corner2.Y = value;
            }
        }

        // Normalize the rectangle
        // Ensure X1 <= X2 and Y1 <= Y2
        public RectangleF Normalize()
        {
            return new RectangleF(Math.Min(X1, X2), Math.Min(Y1, Y2), Math.Max(X1, X2), Math.Max(Y1, Y2));
        }

        // Absolute width of the rectangle.
        public float Width {
            get {
                return X2 - X1;
            }
        }

        // Absolute height of the rectangle.
        public float Height {
            get {
                return Y2 - Y1;
            }
        }

        // Format as a string.
        public override string ToString()
        {
            return String.Format("{{X1 = {0}, Y1 = {1}, X2 = {2}, Y2 = {3}}}", X1, Y1, X2, Y2);
        }

        // Constructor, should be obvious.
        public RectangleF(float x1, float y1, float x2, float y2)
        {
            this.corner1 = new PointF(x1, y1);
            this.corner2 = new PointF(x2, y2);
        }

        // Create from 2 points.
        public RectangleF(PointF p1, PointF p2): this(p1.X, p1.Y, p2.X, p2.Y)
        {
        }

        // Create a rectangle from a corner and a size.
        public static RectangleF FromXYWidthHeight(float x, float y, float width, float height)
        {
            return new RectangleF(x, y, x + width, y + height);
        }

        // Implicit conversion into RectangleD
        public static implicit operator Alpha.Util.RectangleD(Alpha.Util.RectangleF r)
        {
            return new Alpha.Util.RectangleD(r.X1, r.Y1, r.X2, r.Y2);
        }
    }

    // Simple double precission floating point rectangle struct.
    // Defined as two points.
    //
    // There is no need for Corner1 to be smaller than Corner2.
    // If you want that, call .Normalize()
    public struct RectangleD
    {
        // One corner of the rectangle.
        private PointD corner1;
        public PointD Corner1 {
            get {
                return corner1;
            }
            set {
                corner1 = value;
            }
        }

        // Corner1.X
        public double X1 {
            get {
                return Corner1.X;
            }
            set {
                corner1.X = value;
            }
        }

        // Corner1.Y
        public double Y1 {
            get {
                return Corner1.Y;
            }
            set {
                corner1.Y = value;
            }
        }

        // The other corner of the rectangle.
        private PointD corner2;
        public PointD Corner2 {
            get {
                return corner2;
            }
            set {
                corner2 = value;
            }
        }

        // Corner2.X
        public double X2 {
            get {
                return Corner2.X;
            }
            set {
                corner2.X = value;
            }
        }

        // Corner2.Y
        public double Y2 {
            get {
                return Corner2.Y;
            }
            set {
                corner2.Y = value;
            }
        }

        // Normalize the rectangle
        // Ensure X1 <= X2 and Y1 <= Y2
        public RectangleD Normalize()
        {
            return new RectangleD(Math.Min(X1, X2), Math.Min(Y1, Y2), Math.Max(X1, X2), Math.Max(Y1, Y2));
        }

        // Return a rectangle that is smaller than this in every dimension by amount.
        // The new rectangle's width will be 2 * amount smaller.
        public RectangleD Shrink(double amount)
        {
            int sx = Math.Sign(X2 - X1);
            int sy = Math.Sign(Y2 - Y1);
            return new RectangleD(
                    X1 + sx * amount, Y1 + sy * amount,
                    X2 - sx * amount, Y2 - sy * amount);
        }

        // Absolute width of the rectangle.
        public double Width {
            get {
                return X2 - X1;
            }
        }

        // Absolute height of the rectangle.
        public double Height {
            get {
                return Y2 - Y1;
            }
        }

        // Format as a string.
        public override string ToString()
        {
            return String.Format("{{X1 = {0}; Y1 = {1}; X2 = {2}; Y2 = {3}}}", X1, Y1, X2, Y2);
        }

        // Round to a Rectangle.
        // The result is the biggest Rectangle included in this.
        public Rectangle InnerRound()
        {
            Rectangle r = new Rectangle(0, 0, 0, 0);

            r.X1 = (int)(X1 > X2 ? Math.Floor(X1) : Math.Ceiling(X1));
            r.Y1 = (int)(Y1 > Y2 ? Math.Floor(Y1) : Math.Ceiling(Y1));
            r.X2 = (int)(X1 < X2 ? Math.Floor(X2) : Math.Ceiling(X2));
            r.Y2 = (int)(X2 < Y2 ? Math.Floor(Y2) : Math.Ceiling(Y2));

            return r;
        }

        // Round to a Rectangle.
        // The result is the biggest Rectangle included in this.
        public Rectangle OuterRound()
        {
            Rectangle r = new Rectangle(0, 0, 0, 0);

            r.X1 = (int)(X1 < X2 ? Math.Floor(X1) : Math.Ceiling(X1));
            r.Y1 = (int)(Y1 < Y2 ? Math.Floor(Y1) : Math.Ceiling(Y1));
            r.X2 = (int)(X1 > X2 ? Math.Floor(X2) : Math.Ceiling(X2));
            r.Y2 = (int)(X2 > Y2 ? Math.Floor(Y2) : Math.Ceiling(Y2));

            return r;
        }

        public static RectangleD Intersection(RectangleD r1, RectangleD r2)
        {
            r1 = r1.Normalize();
            r2 = r2.Normalize();
            return new RectangleD(Math.Max(r1.X1, r2.X1), Math.Max(r1.Y1, r2.Y1),
                    Math.Min(r1.X2, r2.X2), Math.Min(r1.Y2, r2.Y2)).Normalize();
        }

        // Constructor, should be obvious.
        public RectangleD(double x1, double y1, double x2, double y2)
        {
            this.corner1 = new PointD(x1, y1);
            this.corner2 = new PointD(x2, y2);
        }

        // Create from 2 points.
        public RectangleD(PointD p1, PointD p2): this(p1.X, p1.Y, p2.X, p2.Y)
        {
        }

        // Create a rectangle from a corner and a size.
        public static RectangleD FromXYWidthHeight(double x, double y, double width, double height)
        {
            return new RectangleD(x, y, x + width, y + height);
        }
    }
}

