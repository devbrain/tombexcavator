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

namespace Alpha.Game
{
    // Class encompassing a validation error.
    // By convention you should pass null when there are no errors.
    //
    // This is used instead of exceptions because playing around with
    // exceptions is considered bad practice. ValidationError occur
    // all the time in normal usage.
    //
    // FIXME: right now this is just a string container.
    // FIXME: make subclasses with parameters and use those.
    public class ValidationError
    {
        // Message string.
        private string message;
        public string Message {
            get {
                return message;
            }
        }

        // Constructor. Message can't be null.
        public ValidationError(string message)
        {
            if (message == null) {
                throw new ArgumentNullException("message");
            }
            this.message = message;
        }

        // For printing. This just prints the message.
        public override string ToString()
        {
            return Message;
        }
    }

    // Base class for Alpha.Game exception.
    public class GameException: Exception
    {
        public GameException(string message, Exception inner): base(message, inner)
        {
        }

        public GameException(string message): this(message, null)
        {
        }
    }

    // Exception thrown when validation fails.
    // Throw this if if you encounter unacceptable validation errors.
    public class ValidationException: GameException
    {
        // The validation error which caused this exception.
        private ValidationError error;
        public ValidationError Error {
            get {
                return error;
            }
        }

        // Real constructor.
        public ValidationException(string message, ValidationError error, Exception inner):
                base(message, inner)
        {
            if (error == null) {
                throw new ArgumentNullException("error");
            }
            this.error = error;
        }

        // Convenience constructor.
        public ValidationException(ValidationError error):
            this(String.Format("Failed validation: {0}", error), error, null)
        {
        }

        // Convenience constructor.
        public ValidationException(string message, ValidationError error):
            this(message, error, null)
        {
        }
    }
}

