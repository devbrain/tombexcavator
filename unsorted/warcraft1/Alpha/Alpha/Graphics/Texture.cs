// Copyright (C) 2004-2005 Crestez Leonard    cleonard@go.ro
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

namespace Alpha.Graphics
{
    // Thin wrapper around an opengl texture.
    // The texture is kept on the graphics card, and is therefore impossible
    // to retrieve (or just impractical for speed reasons)
    //
    // This class also supports modifying the texture matrix.
    //
    // TODO: Make a more general material class, that can do fancy stuff,
    // and multiple passes.
    // TODO: Move texture matrix stuff to material.
    // TODO: This means we have to mark it somehow as impossible to wrap.
    // TODO: A LOT later: Multi-texturing?
    public class Texture
    {
        // The opengl texture name, as for glBindTexture.
        private int glName;

        // if the texture has already been disposed of.
        private bool disposed = false;

        // Scale the texture coordinates.
        public double XScale, YScale;

        // Translate the texture coordinates.
        public double XTrans, YTrans;

        // Create a new texture from the specified bitmap.
        public Texture(Bitmap bmp): this()
        {
            Bind();
            GL.TexImage(bmp);
            Unbind();
        }

        // Private empty constructor. This allows for named constructors.
        private Texture()
        {
            glName = GL.GenTexture();
            XScale = YScale = 1;
        }

        // Create a new texture from the specified file.
        public Texture(string file): this(new Bitmap(file))
        {
        }

        // Create a new texture from the specified Color buffer.
        // The int values are 8bit ARGB
        public static Texture StrictTexture(int[,] buffer)
        {
            int[,] newbuf;
            int height, newHeight, width, newWidth, y, x;
           
            height = buffer.GetUpperBound(0) + 1;
            width = buffer.GetUpperBound(1) + 1;
            for (newWidth = 1; newWidth < width; newWidth <<= 1) {
            }
            for (newHeight = 1; newHeight < height; newHeight <<= 1) {
            }
            if (newWidth != width || newHeight != height) {
                newbuf = new int[newHeight, newWidth];

                for (x = 0; x < height; ++x) {
                    for (y = 0; y < width; ++y) {
                        newbuf[x, y] = buffer[x, y];
                    }
                }
            } else {
                newbuf = buffer;
            }
 
            Texture tex = new Texture();
            tex.glName = GL.GenTexture();
            tex.XScale = (double)width / newWidth;
            tex.YScale = (double)height / newHeight;
            tex.Bind();
            GL.TexImageStrict(newbuf);
            tex.Unbind();
            return tex;
        }

        private static bool TextureBound = false;

        // Bind the current texture.
        // FIXME: optimize, state changes are expensive, even when they don't
        // actually change anything.
        public void Bind()
        {
            if (disposed) {
                throw new InvalidOperationException("Can't bind a disposed texture");
            }
            if (TextureBound) {
                throw new InvalidOperationException("Must unbind old texture first");
            }
            TextureBound = true;
            GL.Enable(Feature.Texture);
            GL.Bind(glName);

            GL.MatrixMode(MatrixMode.Texture);
            GL.PushMatrix();
            GL.Translate(XTrans, YTrans, 0);
            GL.Scale(XScale, YScale, 1);
            GL.MatrixMode(MatrixMode.ModelView);
        }

        // Unbind the current texture. Empty right now.
        public void Unbind()
        {
            GL.MatrixMode(MatrixMode.Texture);
            GL.PopMatrix();
            GL.MatrixMode(MatrixMode.ModelView);
            TextureBound = false;
        }

        // Delete the texture.
        public void Dispose()
        {
            if (!disposed) {
                GL.DeleteTexture(glName);
                disposed = true;
            }
        }

        // If you don't Dispose of the texture this will be called.
        // Unfortunately opengl textures can only be freed from the thread that aquired them.
        // Right now we leak.
        //
        // I repeat, you MUST properly dispose of the textures.
        ~Texture()
        {
        }
    }

    // This class represents an image, a "sized texture"
    // This is nothing more than a texture + size information.
    public class Image
    {
    	// This is the texture behind the image.
    	private Texture texture;
        public Texture Texture {
            get {
                return texture;
            }
            set {
                if (value == null) {
                    throw new ArgumentNullException("value");
                }
                texture = value;
            }
        }

        // Size information.
        private SizeF size;
        public SizeF Size {
            get {
                return size;
            }
            set {
                size = value;
            }
        }

        // Convenience property for width
        public float Width {
            get {
                return size.Width;
            }
            set {
                size.Width = value;
            }
        }

        // Convenience property for height.
        public float Height {
            get {
                return size.Height;
            }
            set {
                size.Height = value;
            }
        }

        // Implicit conversion to texture.
        public static implicit operator Texture(Image img)
        {
            return img.Texture;
        }

        // Constructor
        public Image(Texture tex, SizeF size)
        {
            texture = tex;
            Size = size;
        }

        // Constructor with explicit width and height.
        public Image(Texture tex, float w, float h)
        {
            texture = tex;
            Width = w;
            Height = h;
        }
    }
}
