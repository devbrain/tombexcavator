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

using Tao.OpenGl;
using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Imaging;

// This file contains a wrapper for Tao.OpenGl, with much nicer syntax.
// Stuff here is mostly a direct mapping to OpenGl, so you're better of
// just reading opengl man pages.
namespace Alpha.Graphics
{
    public enum DrawMode {
        Points = Gl.GL_POINTS,
        Lines = Gl.GL_LINES,
        LineStrip = Gl.GL_LINE_STRIP,
        LineLoop = Gl.GL_LINE_LOOP,
        Triangles = Gl.GL_TRIANGLES,
        TriangleFan = Gl.GL_TRIANGLE_FAN,
        Quads = Gl.GL_QUADS,
        QuadStrip = Gl.GL_QUAD_STRIP,
    }

    public enum MatrixMode {
        Projection = Gl.GL_PROJECTION,
        ModelView = Gl.GL_MODELVIEW,
        Texture = Gl.GL_TEXTURE,
    }

    public enum Feature {
        Texture = Gl.GL_TEXTURE_2D,
        Blend = Gl.GL_BLEND,
    }

    public enum ErrorCode {
        NoError = Gl.GL_NO_ERROR,
        InvalidEnum = Gl.GL_INVALID_ENUM,
        InvalidValue = Gl.GL_INVALID_VALUE,
        InvalidOperation = Gl.GL_INVALID_OPERATION,
        MatrixStackOverflow = Gl.GL_STACK_OVERFLOW,
        MatrixStackUnderflow = Gl.GL_STACK_UNDERFLOW,
        OutOfMemory = Gl.GL_OUT_OF_MEMORY,
    }
    
    public enum CompileMode {
        Compile = Gl.GL_COMPILE,
        CompileAndExecute = Gl.GL_COMPILE_AND_EXECUTE
    }

    public class GL
    {
        //
        // Error handling
        //
        
        public static ErrorCode GetError()
        {
            return (ErrorCode)Gl.glGetError();
        }

        // Check for OpenGL errors and throw and exception if something fails.
        public static void CheckErrors()
        {
            ErrorCode error;
            while ((error = GetError()) != ErrorCode.NoError) {
                throw new System.Exception(error.ToString());
            }
        }

        //
        // Misc stuff.
        //
      
        // Read the pixels in a specified rect and return them in a Bitmap.
        // This function reads data in 32-bit ARGB
        public static Bitmap ReadPixels(int x, int y, int width, int height)
        {
            Bitmap bmp;
            BitmapData data;

            bmp = new Bitmap(width, height);
            data = bmp.LockBits(Rectangle.FromLTRB(0, 0, width, height),
                    ImageLockMode.WriteOnly,
                    PixelFormat.Format32bppArgb);

            Gl.glReadPixels(x, y, width, height, Gl.GL_BGRA, Gl.GL_UNSIGNED_BYTE, data.Scan0);
            bmp.UnlockBits(data);
            bmp.RotateFlip(RotateFlipType.RotateNoneFlipY);

            return bmp;
        }
        
        public static void Viewport(int x, int y, int width, int height)
        {
            Gl.glViewport(x, y, width, height);
        }

        public static void Viewport(Rectangle rect)
        {
            Gl.glViewport(rect.X, rect.Y, rect.Width, rect.Height);
        }

        public static void ClearColor(Color color)
        {
            Gl.glClearColor(color.R / 255.0F, color.G / 255.0F, color.B / 255.0F, color.A / 255.0F);
            Gl.glClear(Gl.GL_COLOR_BUFFER_BIT);
        }

        public static void ClearColor(byte r, byte g, byte b, byte a)
        {
            Gl.glClearColor(r / 255.0F, g / 255.0F, b / 255.0F, a / 255.0F);
            Gl.glClear(Gl.GL_COLOR_BUFFER_BIT);
        }

        public static void ClearDepth(double depth)
        {
            Gl.glClearDepth(depth);
            Gl.glClear(Gl.GL_DEPTH_BUFFER_BIT);
        }

        public static void ClearDepth()
        {
            Gl.glClearDepth(1);
            Gl.glClear(Gl.GL_DEPTH_BUFFER_BIT);
        }

        public static void Enable(Feature f)
        {
            Gl.glEnable((int)f);
        }

        public static void Disable(Feature f)
        {
            Gl.glDisable((int)f);
        }

        private static Queue ScissorQueue;

        // Push a scissoring rectangle.
        // This is used to enable scissoring.
        // The rectangle itself is limited to that of the previous scissoring.
        // 
        // PushScissor/PopScissor are used to enabled/disable scissoring. Since they
        // work like a stack and limit one scissoring to the last, scissoring inside
        // another scissoring works as expected;
        public static void PushScissor(Rectangle rect)
        {
            if (ScissorQueue.Count == 0) {
                Gl.glEnable(Gl.GL_SCISSOR_TEST);
            } else {
                rect.Intersect((Rectangle)ScissorQueue.Peek());
            }
            //Console.WriteLine("Scissor({0})", rect);
            ScissorQueue.Enqueue(rect);
            Gl.glScissor(rect.X, rect.Y, rect.Width, rect.Height);
        }

        // Pop a scissoring rectangle.
        // This removes the top scissoring rectangle.
        // Please see PushScissor.
        public static void PopScissor()
        {
            if (ScissorQueue.Count == 0) {
                throw new InvalidOperationException("Scissor stack underflow");
            }
            ScissorQueue.Dequeue();
            if (ScissorQueue.Count == 0) {
                Gl.glDisable(Gl.GL_SCISSOR_TEST);
            } else {
                Rectangle rect = (Rectangle)ScissorQueue.Peek();
                Gl.glScissor(rect.X, rect.Y, rect.Width, rect.Height);
            }
        }

        //
        //  Matrix stuff.
        //
        
        public static void MatrixMode(MatrixMode mode)
        {
            Gl.glMatrixMode((int)mode);
        }

        public static void PushMatrix()
        {
            Gl.glPushMatrix();
        }

        public static void PopMatrix()
        {
            Gl.glPopMatrix();
        }

        public static void LoadIdentity()
        {
            Gl.glLoadIdentity();
        }

        public static void Translate(double x, double y, double z)
        {
            Gl.glTranslated(x, y, z);
        }

        public static void Translate(Vector3D V)
        {
            Gl.glTranslated(V.X, V.Y, V.Z);
        }

        public static void Rotate(double a, double x, double y, double z)
        {
            Gl.glRotated(a, x, y, z);
        }

        // Rotate using euler angle, namely rotates
        // around z, y and x in that order.
        public static void RotateEuler(double x, double y, double z)
        {
            Rotate(z, 0, 0, 1);
            Rotate(y, 0, 1, 0);
            Rotate(x, 1, 0, 0);
        }

        // More or less the same as RotateEuler, but with a vector parameter.
        public static void RotateEuler(Vector3D v)
        {
            RotateEuler(v.X, v.Y, v.Z);
        }

        public static void Scale(double x, double y, double z)
        {
            Gl.glScaled(x, y, z);
        }

        public static void Scale(Vector3D V)
        {
            Gl.glScaled(V.X, V.Y, V.Z);
        }

        public static void Scale(double s)
        {
            Gl.glScaled(s, s, s);
        }

        public static void Ortho(double left, double right, double bottom, double top, double near, double far)
        {
            Gl.glOrtho(left, right, bottom, top, near, far);
        }

        public static void Ortho(BoundingBox box)
        {
            Gl.glOrtho(box.A.X, box.B.X, box.A.Y, box.B.Y, box.A.Z, box.B.Z);
        }

        //
        //  Texturing.
        //

        public static int GenTexture()
        {
            int result;
            Gl.glGenTextures(1, out result);
            return result;
        }

        public static void DeleteTexture(int texture)
        {
            int[] textures = {texture};
            Gl.glDeleteTextures(1, textures);
        }

        public static void Bind(int glName)
        {
            Gl.glBindTexture(Gl.GL_TEXTURE_2D, glName);
        }

        public static void TexImage(Bitmap bmp)
        {
            BitmapData data;
            data = bmp.LockBits(Rectangle.FromLTRB(0, 0, bmp.Width, bmp.Height),
                    ImageLockMode.ReadOnly,
                    PixelFormat.Format32bppArgb);
            Gl.glTexParameteri(Gl.GL_TEXTURE_2D, Gl.GL_TEXTURE_MAG_FILTER, Gl.GL_LINEAR);
            Gl.glTexParameteri(Gl.GL_TEXTURE_2D, Gl.GL_TEXTURE_MIN_FILTER, Gl.GL_LINEAR);
            Glu.gluBuild2DMipmaps(Gl.GL_TEXTURE_2D, 4, bmp.Width, bmp.Height,
                    Gl.GL_BGRA, Gl.GL_UNSIGNED_BYTE, data.Scan0);
            bmp.UnlockBits(data);
        }

        // Receives a buffer with BGRA data.
        public static void TexImageStrict(int[,] buffer)
        {
            Gl.glTexParameteri(Gl.GL_TEXTURE_2D, Gl.GL_TEXTURE_MAG_FILTER, Gl.GL_NEAREST);
            Gl.glTexParameteri(Gl.GL_TEXTURE_2D, Gl.GL_TEXTURE_MIN_FILTER, Gl.GL_NEAREST);
            Gl.glTexParameteri(Gl.GL_TEXTURE_2D, Gl.GL_TEXTURE_WRAP_S, Gl.GL_CLAMP);
            Gl.glTexParameteri(Gl.GL_TEXTURE_2D, Gl.GL_TEXTURE_WRAP_T, Gl.GL_CLAMP);
            Glu.gluBuild2DMipmaps(Gl.GL_TEXTURE_2D, 4, 
                    buffer.GetUpperBound(1) + 1, buffer.GetUpperBound(0) + 1,
                    Gl.GL_BGRA, Gl.GL_UNSIGNED_BYTE, buffer);
        }

        //
        //  Immediate mode drawing.
        //
        
        public static void Begin(DrawMode mode)
        {
            Gl.glBegin((int)mode);
        }

        public static void End()
        {
            Gl.glEnd();
        }

        public static void Color(byte r, byte g, byte b, byte a)
        {
            Gl.glColor4ub(r, g, b, a);
        }

        public static void Color(byte r, byte g, byte b)
        {
            Gl.glColor3ub(r, g, b);
        }

        public static void Color(Color c)
        {
            Gl.glColor4ub(c.R, c.G, c.B, c.A);
        }

        public static void Vertex(double x, double y)
        {
            Gl.glVertex2d(x, y);
        }

        public static void Vertex(float x, float y)
        {
            Gl.glVertex2f(x, y);
        }

        public static void Vertex(int x, int y)
        {
            Gl.glVertex2i(x, y);
        }

        public static void Vertex(short x, short y)
        {
            Gl.glVertex2s(x, y);
        }

        public static void Vertex(double x, double y, double z)
        {
            Gl.glVertex3d(x, y, z);
        }

        public static void Vertex(float x, float y, float z)
        {
            Gl.glVertex3f(x, y, z);
        }

        public static void Vertex(int x, int y, int z)
        {
            Gl.glVertex3i(x, y, z);
        }

        public static void Vertex(short x, short y, short z)
        {
            Gl.glVertex3s(x, y, z);
        }

        public static void TexCoord(double x, double y)
        {
            Gl.glTexCoord2d(x, y);
        }

        public static void TexCoord(int x, int y)
        {
            Gl.glTexCoord2i(x, y);
        }

        //
        //  Display lists
        //

        public static int GenLists(int range)
        {
            return Gl.glGenLists(range);
        }

        public static void DeleteLists(int list, int range)
        {
            Gl.glDeleteLists(list, range);
        }

        public static void NewList(int list, CompileMode mode)
        {
            Gl.glNewList(list, (int)mode);
        }

        public static void EndList()
        {
            Gl.glEndList();
        }

        public static void CallList(int list)
        {
            Gl.glCallList(list);
        }

        // Initialization (static constructor)
        static GL()
        {
            ScissorQueue = new Queue();
        }
    }
}
