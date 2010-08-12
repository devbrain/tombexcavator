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
using System.Collections;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Binary;

using Alpha.Util;

namespace Alpha.Game
{
    public class TimedCommand
    {
        // Time to execute at.
        private int time;
        public int Time {
            get {
                return time;
            }
        }

        // The serialized command.
        private byte[] SavedCommand;

        // Serialize the command and add it.
        public TimedCommand(GameState state, Command cmd)
        {
            MemoryStream stream = new MemoryStream();
            Serialization.SaveObject(cmd, state, stream);
            stream.Close();
            this.time = state.Cycle;
            this.SavedCommand = stream.GetBuffer();
        }

        // Deserialize the command. The GameState should be the exact same as in the moment of recording.
        public Command GetCommand(GameState state)
        {
            MemoryStream stream = new MemoryStream(SavedCommand);
            return (Command)Serialization.LoadObject(state, stream);
        }
    }

    // A replay is little more than a bunch of timed commands.
    public class Replay
    {
        // The actual commands to execute.
        private ArrayList Commands;

        // Get a specific command.
        public TimedCommand this[int index] {
            get {
                return Commands[index] as TimedCommand;
            }
        }

        // Get the number of commands in the current replay.
        public int Count {
            get {
                return Commands.Count;
            }
        }

        // Game start information.
        private IGameStarter starter;
        public IGameStarter Starter {
            get {
                return starter;
            }
        }

        // Add a new TimedCommand
        public void Add(TimedCommand tcmd)
        {
            if (Commands.Count > 0 && (Commands[Commands.Count - 1] as TimedCommand).Time > tcmd.Time) {
                throw new InvalidOperationException("You have to add commands in the replay in order");
            }
            Commands.Add(tcmd);
        }

        // Add a new command.
        // This needs the GameState when the command was executed.
        public void Add(GameState state, Command cmd)
        {
            Add(new TimedCommand(state, cmd));
        }

        // Constructor.
        // You need information about the game's start to create a replay.
        public Replay(IGameStarter starter)
        {
            this.starter = starter;
            Commands = new ArrayList();
        }
    }

    // This is a class that can help with recording replays.
    // It will attach itself to the game and record in the GameState
    public class ReplayRecorder
    {
        // The replay we are recording.
        private Replay replay;
        public Replay Replay {
            get {
                return replay;
            }
        }

        // The game we are recording.
        private Game game;
        public Game Game {
            get {
                return game;
            }
        }

        // If we are recording.
        private bool recording;
        public bool Recording {
            get {
                return recording;
            }
        }

        // Start recording a game. It's assumed the game is a new game
        // just created using the started, otherwise the replay won't be any good.
        public ReplayRecorder(IGameStarter starter, Game game)
        {
            this.game = game;
            replay = new Replay(starter);
            Start();
        }

        // Start recording a certain game from the current situation.
        // This works by creating a saved game, so it's a bit expensive.
        public ReplayRecorder(Game game)
        {
            throw new NotImplementedException();
        }

        // Start recording. Only called from constructor.
        private void Start()
        {
            recording = true;
            game.ExecuteCommand += new ExecuteCommandEventHandler(Game_ExecuteCommand);
        }

        // Stop recording.
        public void Stop()
        {
            game.ExecuteCommand -= new ExecuteCommandEventHandler(Game_ExecuteCommand);
            recording = false;
        }

        private void Game_ExecuteCommand(object sender, ExecuteCommandEventArgs args)
        {
            //Console.WriteLine("recording {0} at {1}", args.Command, Game.State.Cycle);
            replay.Add(Game.State, args.Command);
        }
    }

    // A game that simply follows an replay and doesn't accept commands.
    public class ReplayGame: SinglePlayerGame
    {
        // The replay to follow.
        public readonly Replay Replay;

        // The position in the replay queue.
        private int ReplayPosition;

        // Advance the replay.
        // This will execute commands that have a timestamp of State.Cycle
        private void AdvanceReplay()
        {
            while (ReplayPosition < Replay.Count &&
                    Replay[ReplayPosition].Time == State.Cycle) {
                OnExecuteCommand(Replay[ReplayPosition++].GetCommand(State));
            }
            if (ReplayPosition < Replay.Count &&
                    Replay[ReplayPosition].Time <= State.Cycle) {
                throw new Exception("Replay screwed up, missed a command?");
            }
        }

        // Create a game after that replay.
        public ReplayGame(Replay replay): base(replay.Starter.CreateGame())
        {
            if (replay == null) {
                throw new ArgumentNullException("replay");
            }
            Replay = replay;
        }

        // Starts the game.
        public override void StartGame(int stime)
        {
            base.StartGame(stime);
            // Yes, this makes sense.
            // This will execute commands given at time = 0, stuff like
            // StartNewGame, initial diplomacy, etc.
            AdvanceReplay();
        }

        // Do the next cycle, look in the replay.
        protected override void NextCycle()
        {
            base.NextCycle();
            AdvanceReplay();
        }

        // A replay doesn't receive commands.
        public override void SendCommand(Command cmd)
        {
            throw new InvalidOperationException("A ReplayGame doesn't receive commands");
        }
    }
}
