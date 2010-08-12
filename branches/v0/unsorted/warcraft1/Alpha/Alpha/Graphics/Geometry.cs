// Copyright (C) 2005 Crestez Leonard    cleonard@go.ro
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

namespace Alpha.Graphics
{
    // Vector with three double components
    // 3D comes from three double, similar to glVertex3d
    // Capital D makes more sense in C#.
    public struct Vector3D
    {
        // X coordinate.
        public double X;

        // Y coordinate.
        public double Y;

        // Z coordinate.
        public double Z;

        // Indexer.
        public double this[int index] {
            get {
                if (index == 0) {
                    return X;
                } else if (index == 1) {
                    return Y;
                } else if (index == 2) {
                    return Z;
                } else {
                    throw new ArgumentOutOfRangeException("index");
                }
            }
            set {
                if (index == 0) {
                    X = value;
                } else if (index == 1) {
                    Y = value;
                } else if (index == 2) {
                    X = value;
                } else {
                    throw new ArgumentOutOfRangeException("index");
                }
            }
        }

        // Null vector.
        public static readonly Vector3D Zero = new Vector3D(0, 0, 0);

        // Unit vector
        public static readonly Vector3D Unit = new Vector3D(1, 1, 1);

        // Constructor that initializes all values.
        public Vector3D(double X, double Y, double Z)
        {
            this.X = X;
            this.Y = Y;
            this.Z = Z;
        }

        public static Vector3D operator+(Vector3D a, Vector3D b)
        {
            return Geometry.Add(a, b);
        }

        public static Vector3D operator-(Vector3D a)
        {
            return Geometry.Neg(a);
        }

        public static Vector3D operator-(Vector3D a, Vector3D b)
        {
            return Geometry.Sub(a, b);
        }

        public static Vector3D operator*(Vector3D a, double x)
        {
            return Geometry.Mul(a, x);
        }

        public static Vector3D operator*(double x, Vector3D a)
        {
            return Geometry.Mul(a, x);
        }

        public static Vector3D operator/(Vector3D a, double x)
        {
            return Geometry.Mul(a, 1 / x);
        }

        public static Vector3D operator/(double x, Vector3D a)
        {
            return new Vector3D(x / a.X, x / a.Y, x / a.Z);
        }

        public double SquaredLength {
            get {
                return X * X + Y * Y + Z * Z;
            }
        }

        public double Length {
            get {
                return Math.Sqrt(SquaredLength);
            }
        }

        public override String ToString()
        {
            return String.Format("Vector3d(X = {0}, Y = {1}, Z = {2})", X, Y, Z);
        }
    }

    // Axis aligned bounding box.
    public struct BoundingBox
    {
        // First corner of the box.
        public Vector3D A;

        // Second corner of the box.
        public Vector3D B;

        // Infinite BoundingBox (almost)
        public static readonly BoundingBox Infinite;

        // Null BoundingBox. Never visible.
        public static readonly BoundingBox Null;

        // static constructor.
        static BoundingBox()
        {
            Infinite.A.X = Infinite.A.Y = Infinite.A.Z = Double.NegativeInfinity;
            Infinite.B.X = Infinite.B.Y = Infinite.B.Z = Double.PositiveInfinity;
            Null.A.X = Null.A.Y = Null.A.Z = Double.NegativeInfinity;
            Null.B.X = Null.B.Y = Null.B.Z = Double.NegativeInfinity;
        }

        // Helper
        public BoundingBox(Vector3D a, Vector3D b)
        {
            A = a;
            B = b;
        }

        // Normalize bounding box.
        // Makes sure that A.C < B.C (C in X Y Z) by swapping the values.
        public void Normalize()
        {
            double t;

            if (A.X > B.X) {
                t = A.X;
                A.X = B.X;
                B.X = t;
            }
            if (A.Y > B.Y) {
                t = A.Y;
                A.Y = B.Y;
                B.Y = t;
            }
            if (A.Z > B.Z) {
                t = A.Z;
                A.Z = B.Z;
                B.Z = t;
            }
        }

        // Return normalized bounding box.
        public static BoundingBox Normalize(BoundingBox A)
        {
            A.Normalize();
            return A;
        }

        // Intersect two normalized bounding boxes.
        public static bool NormIntersection(BoundingBox A, BoundingBox B)
        {
            return (A.A.X <= B.B.X && B.A.X < A.B.X &&
                    A.A.Y <= B.B.Y && B.A.Y < A.B.Y &&
                    A.A.Z <= B.B.Z && B.A.Z < A.B.Z);
        }

        // Intersect two bounding boxes.
        public static bool Intersection(BoundingBox A, BoundingBox B)
        {
            return NormIntersection(Normalize(A), Normalize(B));
        }
    }

    // This class contains static methods for geometric calculations.
    public sealed class Geometry
    {
        // Vector add
        public static Vector3D Add(Vector3D A, Vector3D B)
        {
            return new Vector3D(A.X + B.X, A.Y + B.Y, A.Z + B.Z);
        }

        // Vector negation
        public static Vector3D Neg(Vector3D A)
        {
            return new Vector3D(-A.X, -A.Y, -A.Z);
        }

        // Vector substraction.
        public static Vector3D Sub(Vector3D A, Vector3D B)
        {
            return new Vector3D(A.X - B.X, A.Y - B.Y, A.Z - B.Z);
        }

        // Vector multiplication
        public static Vector3D Mul(Vector3D A, double x)
        {
            return new Vector3D(A.X * x, A.Y * x, A.Z * x);
        }

        // Vector dot product.
        public static Vector3D Dot(Vector3D A, Vector3D B)
        {
            return new Vector3D(A.X * B.X, A.Y * B.Y, A.Z * B.Z);
        }

        // Vector cross product.
        public static Vector3D Cross(Vector3D A, Vector3D B)
        {
            return new Vector3D(A.Y * B.Z - A.Z * B.Y, A.Z * B.X - A.X * B.Z, A.X * B.Y - A.Y * B.X);
        }
    }
}

