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

namespace Alpha.Util
{
    // This class is a fast wrapper around a fixed sized byte[]
    // It maintains a Position, that can be changed freely.
    // 
    // It has functionality often provided by a MemoryStream + BinaryReader,
    // byte it's a lot faster. It doesn't provide any error handling of it's own,
    // reading past the end of the stream will result in a IndexOutOfRangeException. 
    // this still allows for error handling, and gives the JIT an oportentityy to use
    // ArrayBoundsCheck elimination.
    //
    // TODO: Write support for something except Byte
    // TODO: Int64 anyone?
    public class FastStream
    {
        // The position in the byte[]. Peek/Read/Write functions use it.
        // Reading past the end of the array or with Position < 0 will result
        // in an array bound exception, this can be a lot faster than 
        // doing our own checking.
        public int Position;

        // The actual data buffer.
        public readonly byte[] Buffer;

        // Get the size of the buffer. Returns Buffer.Length.
        public int Size {
            get {
                return Buffer.Length;
            }
        }

        // Create the stream, with the specifed size and filled with 0.
        public FastStream(int size)
        {
            Buffer = new byte[size];
            Position = 0;
        }

        // Peek one byte, and don't advance the position.
        public Byte PeekByte()
        {
            return (Byte)Buffer[Position];
        }

        // Read one byte, and advance the position by 1.
        public Byte ReadByte()
        {
            return (Byte)Buffer[Position++];
        }

        // Write one byte, and advance the position by 1.
        public void WriteByte(Byte val)
        {
            Buffer[Position++] = (Byte)val;
        }

        // Peek one Int16, and don't advance the position.
        public Int16 PeekInt16()
        {
            return (Int16)(Buffer[Position] | (Buffer[Position + 1] << 8));
        }

        // Read one Int16, and advance the position by 2.
        public Int16 ReadInt16()
        {
            Int16 res = (Int16)(Buffer[Position] | (Buffer[Position + 1] << 8));
            Position += 2;
            return res;
        }

        // Peek one UInt16, and don't advance the position.
        public UInt16 PeekUInt16()
        {
            return (UInt16)(Buffer[Position] | (Buffer[Position + 1] << 8));
        }

        // Read one UInt16, and advance the position by 2.
        public UInt16 ReadUInt16()
        {
            UInt16 res = (UInt16)(Buffer[Position] | (Buffer[Position + 1] << 8));
            Position += 2;
            return res;
        }

        // Peek one Int32, and don't advance the position.
        public Int32 PeekInt32()
        {
            Int32 res = (Int32)(Buffer[Position] |
                    (Buffer[Position + 1] << 8) |
                    (Buffer[Position + 2] << 16) |
                    (Buffer[Position + 3] << 24));
            return res;
        }

        // Read one Int32, and advance the position by 4.
        public Int32 ReadInt32()
        {
            Int32 res = (Int32)(Buffer[Position] |
                    (Buffer[Position + 1] << 8) |
                    (Buffer[Position + 2] << 16) |
                    (Buffer[Position + 3] << 24));
            Position += 4;
            return res;
        }

        // Peek one UInt32, and don't advance the position.
        public UInt32 PeekUInt32()
        {
            UInt32 res = (UInt32)(Buffer[Position] |
                    (Buffer[Position + 1] << 8) |
                    (Buffer[Position + 2] << 16) |
                    (Buffer[Position + 3] << 24));
            return res;
        }

        // Read one UInt32, and advance the position by 4.
        public UInt32 ReadUInt32()
        {
            UInt32 res = (UInt32)(Buffer[Position] |
                    (Buffer[Position + 1] << 8) |
                    (Buffer[Position + 2] << 16) |
                    (Buffer[Position + 3] << 24));
            Position += 4;
            return res;
        }
    }
}
