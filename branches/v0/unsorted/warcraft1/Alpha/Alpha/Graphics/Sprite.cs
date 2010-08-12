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
using Alpha.Util;

namespace Alpha.Graphics
{
    // This class represents as sprite.
    // A sprites has a certain SIZE and a number of FRAMES.
    //
    // Invidividual frames are Textures/Materials.
    public class Sprite
    {
        // Individual frames.
    	private Texture[] frames;
        public Texture[] Frames {
            get {
                return frames;
            }
            set {
                if (value == null) {
                    throw new ArgumentNullException("value");
                }
                frames = value;
            }
        }

        // Size of the sprite.
        private SizeF size;
        public SizeF Size {
            get {
                return size;
            }
            set {
                size = value;
            }
        }

        // Indexer.
        public Texture this[int index] {
            get {
                return Frames[index];
            }
            set {
                Frames[index] = value;
            }
        }

        // Get a frame as an Image.
        public Image GetImage(int index)
        {
            return new Image(Frames[index], Size);
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

        // Shortcut.
        public int FrameCount {
            get {
                return Frames.Length;
            }
        }

        // Shortcut.
        public void Bind(int frame)
        {
            Frames[frame].Bind();
        }

        // Shortcut.
        public void Unbind(int frame)
        {
            Frames[frame].Unbind();
        }

        // Constructor
        public Sprite(Texture[] frames, SizeF size)
        {
            Frames = frames;
            Size = size;
        }

        // Constructor with explicit width and height.
        public Sprite(Texture[] frames, float w, float h)
        {
            Frames = frames;
            Width = w;
            Height = h;
        }

        // Constructor
        public Sprite(int frameCount, float w, float h)
        {
            Frames = new Texture[frameCount];
            Width = w;
            Height = h;
        }
    }

    // Link to an image inside a sprite.
    // Yes, weak is is meaningfull
    public class SpriteImageLink: LazyLink
    {
        // Link to the array.
        public readonly ILazyLink SpriteLink;

        // Index in the array.
        public readonly int Index;

        // This will LazyLink.Get the array and return a certain element.
        protected override object Create()
        {
            return ((Sprite)SpriteLink.Get()).GetImage(Index);                                                          ;
        }

        // Constructor, parameters are obvious.
        public SpriteImageLink(bool weak, ILazyLink sprite, int index): base(weak)
        {
            SpriteLink = sprite;
            Index = index;
        }
    }

    // This class can take a frame and rotation and ajust those values for
    // a sprite that has multiple images for different orientations.
    //
    // The most important function is Adjust, called by consumers to orient sprites
    // The orientation depends on the fields of SpriteOrienter.
    //
    // This class divides the circle of possible directions into a number of regions,
    // and gives a diferent result for each region. Region 0 is centered around UpDirection,
    // and they go Clockwise(or CounterClockwise) depending on the value of ClockWise frame.
    //
    // The number of the region you're in is the resulting frame.
    // The resulting angle is the diference between the center of the region and the actual
    // orientation (can be force to 0 with IgnoreRotation).
    //
    // If you set Mirrored to true then you get 2 * (of - 1) regions(of is Orientation Frames).
    // frames 0 through of - 1 map to regions 0 through of - 1, as usual. The remaining frames,
    // (of, 2 * of - 3) map to (of - 2, 1), in that order, and return mirror set to true.
    // This way, instead of loading a full set of graphics for simetrical objects, you only load
    // graphics for half the orientations and mirror the rest around UpDirection.
    // There's obviousely no mirroring for frames 0 and of - 1.
    //
    // All angles are in degrees, in standard trigonometric order
    // This means that an UpDirection means looking towards x+
    public class SpriteOrienter
    {
        // This is the up direction. This is the direction for rotation frame 0.
        public double UpDirection;

        // If frames are clockwise. Defaults to false
        public bool ClockwiseFrames = false;

        // The number of distinct orientations the Sprite supports.
        private int orientationFrames = 1;
        public int OrientationFrames {
            get {
                return orientationFrames;
            }
            set {
                if (value < 1) {
                    throw new ArgumentException("value");
                }
                orientationFrames = value;
            }
        }

        // If the sprite is Mirrored, this only has an effect with OrientationFrames > 1
        // Please see class documentation.
        public bool Mirrored = false;

        // If this is set to true, the correct sprite is picked up and then
        // the difference between the angle and the sprite's angle is returned.
        // 
        // If this is disabled 0 is returned as an angle and blocky artwork might look
        // nicer.
        public bool RotationAdjustment = true;

        // If this is set to 0 then the output frame is the initial frame plus the rotation frame,
        // otherwise it's OrientationFrames * inFrame + rotFrame.
        public bool MultiplyInputFrame = false;

        // Adjust a sprite for it's orientation.
        // inFrame and inRotation are the input frame and rotation(around the center)
        // outFrame and outRotation are the result frame and rotation to properly draw the sprite.
        //
        // angles are in degrees and standard trigonometric order.
        public void Adjust(int inFrame, double inRotation, out int outFrame, out double outRotation, out bool hmirror)
        {
            int rotFrame = 0;
            int of = OrientationFrames;
            double rot;

            if (ClockwiseFrames) {
                inRotation *= -1;
            }
            rot = inRotation - UpDirection;

            hmirror = false;
            if (of > 1) {
                if (Mirrored) {
                    int fof = 2 * (of - 1);
                    rotFrame = (int)Math.Floor((rot + 180 / fof) * fof / 360);
                    rotFrame = (rotFrame % fof + fof) % fof;
                    rot = rot - rotFrame * 360 / fof;
                    if (rotFrame >= of) {
                        rotFrame = of - 1 - (rotFrame - (of - 1));
                        hmirror = !ClockwiseFrames;
                    } else {
                        hmirror = ClockwiseFrames;
                    }
                } else {
                    rotFrame = (int)Math.Floor((rot + 180 / of) * of / 360);
                    rotFrame = (rotFrame % of + of) % of;
                    rot = rot - rotFrame * 360 / of;
                }
            }

            if (MultiplyInputFrame) {
                outFrame = inFrame * OrientationFrames + rotFrame;
            } else {
                outFrame = inFrame + rotFrame;
            }

            if (RotationAdjustment) {
                outRotation = -rot;
            } else {
                outRotation = 0;
            }
        }

        // Draw a certain frame of a Sprite using the specified orienter.
        // NOTE: orienter can be null.
        public static void Draw(Sprite sprite, int frame, double angle, SpriteOrienter orienter)
        {
            bool hmirror = false;

            // Adjust with SpriteOrienter
            if (orienter != null) {
                orienter.Adjust(frame, angle, out frame, out angle, out hmirror);
            }

            double w = sprite.Width;
            double h = sprite.Height;
            
            sprite.Bind(frame);
            GL.PushMatrix();
            GL.RotateEuler(0, 0, angle);
            GL.Begin(DrawMode.Quads);

            int tx = hmirror ? 1 : 0;
            GL.TexCoord(tx, 0);
            GL.Vertex(- w / 2, - h / 2, 0);
            GL.TexCoord(tx, 1);
            GL.Vertex(- w / 2, + h / 2, 0);
            GL.TexCoord(1 - tx, 1);
            GL.Vertex(+ w / 2, + h / 2, 0);
            GL.TexCoord(1 - tx, 0);
            GL.Vertex(+ w / 2, - h / 2, 0);
            GL.TexCoord(0, 0);
            GL.Vertex(- w / 2, - h / 2, 0);

            GL.End();
            GL.PopMatrix();
            sprite.Unbind(frame);
        }
    }
}
