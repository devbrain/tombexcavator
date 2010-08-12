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
using System.Collections;

namespace Alpha.Game
{
    // This enum sums up player relations.
    // A PlayerRelation is an or combination of Enemy, Friend, Neutral with
    // a SharedVision bit.
    [Flags]
    public enum PlayerRelation
    {
        Neutral = 0,
        Enemy = 1,
        Friend = 2,
        SharedVision = 4
    }

    // The player class.
    // This contains information about a player. It should include
    // resources, scores, entity upgrades, and more
    public class Player: GameObject
    {
        // The color of the player. Cool eh?
        // FIXME: move to PlayerStats.
        public Color Color = Color.FromArgb(255, 255, 255, 255);

        // Constant identifier used by the player.
        public readonly int Ident;

        // Get the player mask.
        // This is 1 << Ident.
        public int Mask {
            get {
                return 1 << Ident;
            }
        }

        // Relations between players.
        public readonly PlayerRelation[] Relation;

        // Get relations to another player.
        public PlayerRelation GetRelation(Player player)
        {
            if (player.State != State) {
                throw new ArgumentException("player", "Doesn't belong to the same GameState");
            }
            return Relation[player.Ident];
        }

        // Constructor. This creates a player with a certain id.
        // If the id is -1 it will try to find the first free id.
        public Player(GameState state, int ident): base(state)
        {
            // Validate identifier.
            if (ident == -1) {
                for (ident = 0; ident < GameState.MaxPlayers; ++ident) {
                    if (state.GetPlayer(ident) == null) {
                        break;
                    }
                }
                if (ident == GameState.MaxPlayers) {
                    throw new InvalidOperationException("Reached maximum number of players");
                }
            } else {
                if (ident < 0 || ident >= GameState.MaxPlayers) {
                    throw new ArgumentOutOfRangeException("ident", "Valid player ids are 0 to 31");
                }
                if (state.GetPlayer(ident) != null) {
                    throw new ArgumentException("ident", "A player with that id already exists");
                }
            }

            // Set identifier and register
            this.Ident = ident;
            State.RegisterPlayer(this);

            Relation = new PlayerRelation[GameState.MaxPlayers];
        }

        // Returns if the player should attack a specific entity.
        public bool ShouldAttack(Entity entity)
        {
            if (entity.State != State) {
                throw new Exception("Doesn't belong to the same GameState");
            }
            if (entity.Player == this) {
                return false;
            }
            return (GetRelation(entity.Player) & PlayerRelation.Enemy) != 0;
        }
    }
}

