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
using System.Net;
using System.Net.Sockets;
using System.Drawing;

namespace Alpha.Game
{
    // Arguments for the Game.ExecuteCommand
    public class ExecuteCommandEventArgs: EventArgs
    {
        // The command executed.
        public readonly Command Command;

        public ExecuteCommandEventArgs(Command command)
        {
            Command = command;
        }
    }

    // Handler for the ExecuteCommand event.
    public delegate void ExecuteCommandEventHandler(object sender, ExecuteCommandEventArgs args);

    // Interface for a class that can create a GameState.
    public interface IGameStarter
    {
        // Create the game.
        GameState CreateGame();
    }

    // This is an abstract class representing a "game"
    // It is a wrapper around a GameState class that can handle things
    // like timing and the like.
    //
    // TODO: figure out how to do implementent multiplayer games.
    public abstract class Game
    {
        // The actual state of the game.
        private GameState state;
        public GameState State {
            get {
                return state;
            }
        }
        
        // If the game is paused (GameState doesn't progress).
        public abstract bool Paused {
            get;
        }

        // The length of one game cycle, in miliseconds.
        public abstract int CycleLength {
            get;
        }

        // Animation can be done in steps less than a cycle. In that case
        // use this property, it's the same as State.Cycle, except with
        // greater precission.
        public abstract double SubCycle {
            get;
        }

        // If this is set to true then the time spent updating the game
        // will be measured and stored in TimeSpent and CyclesCalculated.
        //
        // Profiling data is from enabling or last call to ResetProfilingData.
        private bool profilingEnabled = false;
        public bool ProfilingEnabled {
            get {
                return profilingEnabled;
            }
            set {
                if (ProfilingEnabled == false && value == true) {
                    ResetProfileData();
                }
                profilingEnabled = value;
            }
        }

        // Reset profiling data.
        public void ResetProfileData()
        {
            timeSpent = 0;
            cyclesCalculated = 0;
        }

        // How many ticks were spent in State.NextCycles
        private int timeSpent;
        public int TimeSpent {
            get {
                return timeSpent;
            }
        }

        // How many Cycles were calculated.
        private int cyclesCalculated;
        public int CyclesCalculated {
            get {
                return cyclesCalculated;
            }
        }

        // Function called to advance one cycle.
        protected virtual void NextCycle()
        {
            if (ProfilingEnabled) {
                timeSpent -= Environment.TickCount;
            }
            ++cyclesCalculated;
            State.NextCycle();
            if (ProfilingEnabled) {
                timeSpent += Environment.TickCount;
            }
        }

        // Update the game. Time is the current time.
        public abstract void Update(int time);

        // Send a command to the game. The commands are not guaranteed to be executed
        // right away, in the same order, or can even be dropped (for instance in a replay.)
        public abstract void SendCommand(Command cmd);

        // Event fired after a command is executed.
        public event ExecuteCommandEventHandler ExecuteCommand;

        // This is the function you should call from derrived classes to execute a command.
        // It deals with the replay too.
        protected virtual void OnExecuteCommand(Command cmd)
        {
            cmd.Execute(State);
            if (ExecuteCommand != null) {
                ExecuteCommand(this, new ExecuteCommandEventArgs(cmd));
            }
        }

        // Create a new game.
        public Game(GameState state)
        {
            this.state = state;
        }
    }

    // Class for a simple single-player game.
    public class SinglePlayerGame: Game
    {
        // If the game is paused (GameState doesn't progress).
        // You can pause the game with PauseGame and start it with StartGame()
        private bool paused;
        public override bool Paused {
            get {
                return paused;
            }
        }

        // Pause the game. You have to call StartGame again to restart it.
        public void PauseGame()
        {
            paused = true;
            subCycle = 0;
        }

        // The length of one game cycle, in miliseconds.
        private int cycleLength;
        public override int CycleLength {
            get {
                return cycleLength;
            }
        }

        // Set the length of the cycle, in milisecond
        public void SetCycleLength(int length)
        {
            cycleLength = length;
        }

        // See Game.SubCycle
        private double subCycle;
        public override double SubCycle {
            get {
                return subCycle;
            }
        }

        // How many cycles to allow the game timer to fall behind before throtling.
        // Throtling means reseting FixedClock and FixedCycle to the current time
        // and State.Cycle in Update.
        // 
        // This only happens if the game runs miserably slow.
        // 0 disables this feature.
        private int maxCyclesBehind = 25;
        public int MaxCyclesBehind {
            get {
                return maxCyclesBehind;
            }
            set {
                maxCyclesBehind = value;
            }
        }

        // What we know is that at FixedClock we had FixedCycle, and since then
        // speed has been constant.
        private int FixedClock;
        private int FixedCycle;

        // Update the game.
        // Time is the current time, from some timer.
        // You can use Application.ThisClock.
        public override void Update(int time)
        {
            if (!Paused) {
                subCycle = FixedCycle + ((double)(time - FixedClock)) / CycleLength;
                
                // Check throtling.
                if (MaxCyclesBehind > 0 && subCycle - State.Cycle > MaxCyclesBehind) {
                    Console.WriteLine("Game timer {0} cycles behind, throtling",
                            (int)(subCycle - State.Cycle));
                    FixedClock = time;
                    FixedCycle = State.Cycle;
                    subCycle = FixedCycle;
                }

                while (State.Cycle + 1 <= subCycle) {
                    NextCycle();
                }
            }
        }

        // Create a new single player game.
        public SinglePlayerGame(GameState state): base(state)
        {
            paused = false;
            cycleLength = 50;
       }

        // Start running the game
        // startClock is the starting time, from some timer.
        // You can use Application.ThisClock as the timer.
        //
        // You can call this even if the game is already started, it will just
        // reset the timer.
        public virtual void StartGame(int startTime)
        {
            FixedClock = startTime;
            FixedCycle = State.Cycle;
            subCycle = 0;
            paused = false;
        }

        // Interpret commands. This class executes right away
        public override void SendCommand(Command com)
        {
            OnExecuteCommand(com);
        }
    }
}
