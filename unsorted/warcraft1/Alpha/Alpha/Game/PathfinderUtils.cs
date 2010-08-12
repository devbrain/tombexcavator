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
using System.Collections;

using Alpha.Util;

namespace Alpha.Game
{
    public sealed class PathfinderUtils
    {
        // Concatenate a path by entitying consecutive collinear segments.
        // This will also remove null segments from the path.
        //
        // Modifies the path.
        public static void ConcatPath(ArrayList path)
        {
            int i, j = 1;
            Point a, b, c;
            if (path.Count >= 3) {
                a = (Point)path[0];
                b = (Point)path[1];
                for (i = 2; i < path.Count; ++i) {
                    c = (Point)path[i];
                    if ((c.X - b.X) * (b.Y - a.Y) == (b.X - a.X) * (c.Y - b.Y)) {
                        path[j] = b = c;
                    } else {
                        a = b;
                        path[++j] = b = c;
                    }
                }
                path.RemoveRange(j + 1, path.Count - j - 1);
            }
            // If the last segment is null, remove it.
            // The rest are removed in the loop.
            if (path.Count >= 2) {
                a = (Point)path[path.Count - 2];
                b = (Point)path[path.Count - 1];
                if (a.X == b.X && a.Y == b.Y) {
                    //Console.WriteLine("Equal ends");
                    path.RemoveAt(path.Count - 1);
                }
            }
        }

        // Helper for FastLineWalk.
        //
        // integer math magic.
        private static int MagicFunc(int target, int start, int dif, int length, int tilesize)
        {
            if (dif > 0) {
                return ((target * tilesize - start) * length - 1) / dif + 1;
            } else if (dif < 0) { 
                return (-((target + 1) * tilesize - start - 1) * length - 1) / -dif + 1;
            } else {
                return Int32.MaxValue;
            }
        }

        // Given a 2d Array of free and occupied tiles this function checks if movement
        // is possible on a line between two random points. Those points can fall anywhere
        // in a tile, so it's a lot harder than it seems.
        //
        // block is the 2d tile array.
        // blockSize is the size of the entity. A tile is considerent free if:
        //     block[x, y] >= blockSize, otherwise it's occupied.
        //
        // tileSize is the size of a tile. Conversion to tile coords is done by dividing with
        // tileSize.
        //
        // startCoord and endCoord are start and end positions of the segment to walk on.
        //
        // startPosition and endPosition are the actual positions on the segment to start
        // and end at, relative to segLength. The positions that are checked are from
        // startCoord + (endCoord - startCoord) * i / segLength, where i varies from
        // startPosition to endPosition.
        //
        // This function returns the last free i position, or startCoord - 1 if even
        // the start position is occupied.
        //
        // You CAN NOT transform this function so that it only takes startCoord
        // and endCoord because the movement laws use integer division.
        public static int FastLineWalk(sbyte[,] block, int blocksize, int tilesize,
                Point startCoord, Point endCoord, int startp, int endp, int length)
        {
            int startx, starty, difx, dify;
            // Sign of movement in x and y directions.
            int signx, signy;
            // Current and end path coords coordinates.
            int cpx, cpy, epx, epy;
            // Some coords.
            int x, y;

            // Next position at which x/y changes.
            int npx, npy;

            int pos;
            bool opt = true;

            startx = startCoord.X;
            starty = startCoord.Y;
            difx = endCoord.X - startx;
            dify = endCoord.Y - starty;
            signx = Math.Sign(difx);
            signy = Math.Sign(dify);

            if (length < Math.Abs(difx) || length < Math.Abs(dify)) {
                throw new ArgumentException("Length must be greater or equal than" +
                        " the distances between start and end coordinates, to ensure" +
                        " that the way doesn't skip any path tiles.", "length");
            }

            // Start pos
            x = startx + difx * startp / length;
            y = starty + dify * startp / length;
            opt &= x % tilesize != 0 || y % tilesize != 0;
            cpx = x / tilesize;
            cpy = y / tilesize;

            // End pos.
            x = startx + difx * endp / length;
            y = starty + dify * endp / length;
            opt &= x % tilesize != 0 || y % tilesize != 0;
            epx = x / tilesize;
            epy = y / tilesize;

            opt = false;

            //Console.WriteLine("From {0}, {1} to {2} {3} in tiles", cpx, cpy, epx, epy);

            // Potential optimizations.
            // We disable optimizations when start/stop is on a corner.
            if (opt) {
                // Check start pos.
                if (blocksize > block[cpx, cpy]) {
                    return startp - 1;
                }

                // Doesn't leave the tile.
                if (cpx == epx && cpy == epy) {
                    return endp;
                }
                
                // Constant X
                if (cpx == epx) {
                    for (cpy = cpy + signy; (epy - cpy) * signy >= 0; cpy += signy) {
                        if (blocksize > block[cpx, cpy]) {
                            return MagicFunc(cpy, starty, dify, length, tilesize) - 1;
                        }
                    }
                    return endp;
                }

                // Constant X
                if (cpx == epx) {
                    for (cpx = cpx + signx; (epx - cpx) * signx >= 0; cpx += signx) {
                        if (blocksize > block[cpx, cpy]) {
                            return MagicFunc(cpx, startx, difx, length, tilesize) - 1;
                        }
                    }
                    return endp;
                }
            }

            npx = npy = startp;

            while (cpx != epx || cpy != epy) {
                pos = Math.Min(npx, npy);
                x = startx + difx * pos / length;
                y = starty + dify * pos / length;
                if (pos == npx) {
                    cpx = x / tilesize;
                    npx = MagicFunc(cpx + signx, startx, difx, length, tilesize);
                }
                if (npy == pos) {
                    cpy = y / tilesize;
                    npy = MagicFunc(cpy + signy, starty, dify, length, tilesize);
                }
                //Console.WriteLine("Fast event {0} {1} {2}", cpx, cpy, pos);
                if (blocksize > block[cpx, cpy]) {
                    if (x % tilesize == 0 && y % tilesize == 0) {
                        x = startx + difx * (pos + 1) / length;
                        y = starty + dify * (pos + 1) / length;
                        if (blocksize > block[x / tilesize, y / tilesize]) {
                            return pos - 1;
                        }
                    } else {
                        return pos - 1;
                    }
                }
            }

            return endp;
        }

        // This is just like FastLineWalk only much slower.
        public static int SlowLineWalk(sbyte[,] block, int blockSize, int tileSize,
                Point startCoord, Point endCoord, int startPos, int endPos, int segLength)
        {
            int pos, x, y;
            int lpx = -1, lpy = -1;

            for (pos = startPos; pos <= endPos; ++pos) {
                x = startCoord.X + (endCoord.X - startCoord.X) * pos / segLength;
                y = startCoord.Y + (endCoord.Y - startCoord.Y) * pos / segLength;
                if (x / tileSize != lpx || y / tileSize != lpy) {
                    lpx = x / tileSize;
                    lpy = y / tileSize;
                    //Console.WriteLine("Slow event {0} {1} {2}", lpx, lpy, pos);
                }
                if (blockSize > block[x / tileSize, y / tileSize]) {
                    if (x % tileSize == 0 && y % tileSize == 0) {
                        x = startCoord.X + (endCoord.X - startCoord.X) * (pos + 1) / segLength;
                        y = startCoord.Y + (endCoord.Y - startCoord.Y) * (pos + 1) / segLength;
                        if (blockSize > block[x / tileSize, y / tileSize]) {
                            return pos - 1;
                        }
                    } else {
                        return pos - 1;
                    }
                }
            }
        
            return pos - 1;
        }
    }

/*
    // Data structure which can be used to get the minimum of a set.
    // This is heavily optimized and rather awkward to use.
    //
    // When elements are added the user gets back an integer handle
    // which can later be used in updating the value.
    //
    // GetMin returns the key of the minimum element, not the handle.
    //
    // The right way to use this is to store the handle in every element
    // which you want to add. This wierd handle system ensures that no
    // hashes are required anywhere while using very little memory.
    //
    // This could be implemented as a heap; but it's not. Instead a
    // simpler (and faster) implementation is used with the same log(n)
    // time complexity.
    //
    // FIXME: Shrinking support. Right now this structure never frees
    // memory (except when destroyed).
    public class MinStruct
    {
        // Adds an element to the struct and returns a "handle" which
        // can be laters used to update the element.
        public int AddElement(int key, int val);

        // Update a certain element by handle, changing the value.
        // FIXME: change key?
        public void UpdateElement(int handle, int val);

        // Delete a certain element.
        public void DeleteElement(int handle);

        // Gets the minimum element. It returns the KEY of the element,
        // or -1 if empty.
        public int GetMin();
    }
*/

    // Simple heap. You add int key, val and you can extract the key with minimum val.
    // This was implemented to serve the needs of the current pathfinding
    // code, so it fairly thin.
    //
    // Avoids boxing and such.
    public class IntHeap 
    {
        // An element of the heap.
        private struct HeapElement
        {
            // X and Y values.
            public int Key;
            // Key to sort after.
            public int Val;
        }

        // The elements in the heap.
        private HeapElement[] Elements;

        // Capacity, the number of allocated elements.
        private int Capacity;

        // The size of the heap.
        public int count;
        public int Count {
            get {
                return count;
            }
        }

        // Create a new heap. By default it's empty.
        public IntHeap()
        {
            count = 0;
            Capacity = 2;
            Elements = new HeapElement[Capacity];
        }

        // Double the heap's capacity.
        private void Grow()
        {
            HeapElement[] e;
            e = new HeapElement[2 * Capacity];
            Array.Copy(Elements, e, Capacity);
            Elements = e;
            Capacity *= 2;
        }

        // Add a new x, y value with key k.
        public void Add(int key, int val)
        {
            int i;
            HeapElement t;
            if (Count + 1 == Capacity) {
                Grow();
            }
            count++;
            Elements[Count].Key = key;
            Elements[Count].Val = val;
            for (i = Count; i > 1 && Elements[i].Val < Elements[i / 2].Val; i /= 2) {
                t = Elements[i];
                Elements[i] = Elements[i / 2];
                Elements[i / 2] = t;
            }
        }

        // Extract the key with the smallest val and return it.
        public int Pop()
        {
            int i, bs, res;
            HeapElement t;
            res = Elements[1].Key;

            // Heap magic.
            Elements[1] = Elements[count--];
            i = 1;
            while (2 * i <= Count) {
                bs = Elements[2 * i].Val < Elements[i].Val ? 2 * i : i;
                if (2 * i < Count) {
                    bs = Elements[2 * i + 1].Val < Elements[bs].Val ? 2 * i  + 1 : bs;
                }
                if (bs != i) {
                    t = Elements[i];
                    Elements[i] = Elements[bs];
                    Elements[bs] = t;
                    i = bs;
                } else {
                    break;
                }
            }
            return res;
        }

        // Clear the heap
        public void Clear()
        {
            count = 0;
        }
    }

    // This class is a wrapper around a large rectangular table of tiles.
    // You can Block or Unblock tile rectangles and get the largest free
    // square starting at every point. You can block a tile multiple times
    // and it will get counted, you need an equal number of unblocks to free
    // it.
    //
    // For every free tile it stores the size of the largest square of free
    // tiles with the upper-left corner at that point, up to a maximum size
    // of LargestSquare. For every occupied tile it keeps the number of times
    // it has been blocked. Here is a sample table:
    //
    //  2  2  2  2  3  2  1
    //  1  1  1  1  2  2  1
    //  1 -1 -1 -1  1  2  1
    //  1 -1 -1 -2 -1  2  1
    // -1 -2 -1 -2 -1  2  1
    // -1 -1  1  1  1  1  1
    //
    // The table covers an rectangle sized Width * Height starting at 0, 0. The
    // area outside those bounds is considered "forever blocked". It is never
    // free and blocking/unblocking has no effect there.
    //
    // This is used in the pathfinder so speed is critical.
    // FIXME: A TON of optimizations are possible.
    public class BlockTable
    {
        // The underlying array.
        private sbyte[,] table;
        public sbyte[,] Table {
            get {
                return table;
            }
        }

        // Indexer, it can access the underlying table.
        // You should never modify the table unless you know what you're doing.
        // If you do, you should call UpdateTable.
        public sbyte this[int x, int y] {
            get {
                // Out of bounds if forever blocked.
                // A smart JIT would eliminate array bounds checking here.
                if (x < 0 || y < 0 || x >= Width || y >= Height) {
                    return 0;
                }
                return table[x, y];
            }
            set {
                // Throw an exception if out of bounds, I don't care.
                table[x, y] = value;
            }
        }

        // Width of the table.
        public int Width {
            get {
                return table.GetLength(0);
            }
        }

        // The height of the table.
        public int Height {
            get {
                return table.GetLength(1);
            }
        }

        // The largest free square to keep track of.
        // A large value makes modifications slower.
        //
        // NOTE: changing this value requires Width * Height time.
        private int largestSquare;
        public int LargestSquare {
            get {
                return largestSquare;
            }
            set {
                if (value < 1) {
                    throw new ArgumentOutOfRangeException("value");
                }
                largestSquare = value;
                Update();
            }
        }

        // Normalize and clamp a rectangle.
        private void ClampRect(ref int x1, ref int y1, ref int x2, ref int y2)
        {
            int t;
            if (x1 > x2) {
                t = x1;
                x1 = x2;
                x2 = t;
            }
            if (y1 > y2) {
                t = y1;
                y1 = y2;
                y2 = t;
            }
            x1 = Math.Max(x1, 0);
            y1 = Math.Max(y1, 0);
            x2 = Math.Min(x2, Width - 1);
            y2 = Math.Min(y2, Height - 1);
        }

        // Block a rectangle from x1 y1 to x2 y2, inclusive.
        public void Block(int x1, int y1, int x2, int y2)
        {
            int i, j;
            ClampRect(ref x1, ref y1, ref x2, ref y2);
            for (i = x1; i <= x2; ++i) {
                for (j = y1; j <= y2; ++j) {
                    if (Table[i, j] > 0) {
                        Table[i, j] = 0;
                    } else {
                        --Table[i, j];
                    }
                }
            }
            UpdateArea(x1, y1, x2 + LargestSquare - 1, y2 + LargestSquare - 1);
        }

        // Unblock a rect sized w * h starting at x, y.
        // Throws an exception if it "underflows"
        public void Unblock(int x1, int y1, int x2, int y2)
        {
            int i, j;
            ClampRect(ref x1, ref y2, ref x2, ref y2);
            for (i = x1; i <= x2; ++i) {
                for (j = y1; j <= y2; ++j) {
                    if (Table[i, j] > 0) {
                        throw new System.Exception("BlockTable underflow");
                    }
                    ++Table[i, j];
                }
            }
            UpdateArea(x1, y1, x2 + LargestSquare - 1, y2 + LargestSquare - 1);
        }

        // Update the whole table.
        public void Update()
        {
            UpdateForArea(0, 0, Width - 1, Height - 1);
        }

        // Updates the table for the modification of a certain rectangular area.
        // The area is the area that you modified. This recalculates the largest
        // free square values. This can modify values outside the mod area.
        public void UpdateForArea(int x1, int y1, int x2, int y2)
        {
            UpdateArea(x1, y1, x2 + LargestSquare - 1, y2 + LargestSquare - 1);
        }

        public void UpdateForArea(Rectangle rect)
        {
            UpdateArea(rect.X1, rect.Y1, rect.X2 + LargestSquare - 1, rect.Y2 + LargestSquare - 1);
        }

        // Update a certain area of the map.
        // FIXME: Optimize this, it is critical for speed.
        private void UpdateArea(int x1, int y1, int x2, int y2)
        {
            int i, j;
            int a, b, c;
            ClampRect(ref x1, ref y1, ref x2, ref y2);
            for (i = x1; i <= x2; ++i) {
                for (j = y1; j <= y2; ++j) {
                    if (Table[i, j] > 0) {
                        // This[,] returns 0 on out of bounds instead of throwing an exception.
                        a = this[i - 1, j - 1];
                        b = this[i - 1, j];
                        c = this[i, j - 1];
                        Table[i, j] = (sbyte)(1 + Math.Max(0,
                                    Math.Min(Math.Min(a, LargestSquare - 1), Math.Min(b, c))));
                    }
                }
            }
        }

        // Returns true if the rectangle contains all free tiles.
        // w and h can be larger than LargestEntity.
        //
        // FIXME: optimize, don't check the whole area.
        public bool AreaIsFree(int x1, int y1, int x2, int y2)
        {
            int i, j, t;

            // Order coords.
            if (x1 > x2) {
                t = x1;
                x1 = x2;
                x2 = t;
            }
            if (y1 > y2) {
                t = y1;
                y1 = y2;
                y2 = t;
            }

            // False if out of bounds.
            if (x1 < 0 || x2 >= Width || y1 < 0 || y2 >= Height) {
                return false;
            }

            for (i = x1; i <= x2 ; ++i) {
                for (j = y1; j <= y2; ++j) {
                    if (Table[i, j] <= 0) {
                        return false;
                    }
                }
            }
            return true;
        }

        // Constructor. Width and height are forever, but largest square
        // can be changed later
        public BlockTable(int width, int height, int largestSquare)
        {
            // We use bordering. Width/Height HIDES this.
            table = new sbyte[width, height];

            this.largestSquare = largestSquare;

            // Initialize the table.
            int x, y, val;
            for (x = 0; x < Width; ++x) {
                for (y = 0; y < Height; ++y) {
                    val = Math.Min(LargestSquare, Math.Min(x + 1, y + 1));
                    Table[x, y] = (sbyte)val;
                }
            }
        }
    }
}
