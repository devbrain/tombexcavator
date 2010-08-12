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
using System.Collections;

using Alpha.Util;
using Alpha.Game;
using Alpha.Graphics;

// Various cheats.
[Flags]
public enum Cheat {
    Invincibility = 1,
    BuildSpeed = 2,
    Mana = 4,
    Resource = 8,
}

// Command that toggles a cheat for a player.
// Cheats that don't toggle (resources, upgrades) are just executed.
public class CheatCommand: Command
{
    public readonly Cheat Cheat;

    public readonly WarPlayer Player;

    public override void Execute(GameState state)
    {
        if (Cheat == Cheat.Resource) {
            Player.WoodAmount += 5000;
            Player.GoldAmount += 10000;
            Player.CheatsEnabled |= Cheat;
            Console.WriteLine("Enabled Resource cheat");
        } else {
            Player.CheatsEnabled ^= Cheat;
            if ((Player.CheatsEnabled & Cheat) != 0) {
                Console.WriteLine("Enabled {0} cheat", Cheat);
            } else {
                Console.WriteLine("Disabled {0} cheat", Cheat);
            }
        }
    }

    public CheatCommand(WarPlayer player, Cheat cheat)
    {
        this.Cheat = cheat;
        this.Player = player;
    }
}

// WC1 game state derivate. FIXME: sux?
public class WarGameState: GameState
{
    public new WarGameData Data {
        get {
            return (WarGameData)Data;
        }
    }

    // Mask of allowed features.
    public uint AllowedFeatures;

    // Check if a feature is allowed.
    public bool IsAllowed(AllowFeature feature)
    {
        if (feature == AllowFeature.None) {
            return true;
        }
        if (feature < 0 || feature > AllowFeature.Max) {
            throw new ArgumentOutOfRangeException("feature");
        }
        return (AllowedFeatures & (1 << (int)feature)) != 0;
    }

    // Sets a mask of allowed features.
    // Called from map loading code.
    // FIXME: slightly retarded.
    public void SetAllowedFeatures(uint mask)
    {
        AllowedFeatures = mask;
    }

    // Create statistics for a player.
    public override Player CreatePlayer(int id)
    {
        return new WarPlayer(this, id);
    }

    // Constructor.
    public WarGameState(WarGameData data): base(data)
    {
        AllowedFeatures = (1 << (int)AllowFeature.Max) - 1;
    }
}

// WC1 player class.
public class WarPlayer: Player
{
    // The amount of gold available.
    public int GoldAmount;

    // The amount of wood available.
    public int WoodAmount;

    // Bit mask of enabled cheats.
    public Cheat CheatsEnabled = 0;

    // Upgrade levels for this player.
    // FIXME: max levels not enforced anywhere.
    public readonly int[] UpgradeLevels;

    // Set to true when researching to prevent simultaneous researching.
    public readonly bool[] UpgradeResearching;

    // Constructor. Rather empty.
    public WarPlayer(WarGameState state, int id): base(state, id)
    {
        UpgradeLevels = new int[(int)UpgradeId.Max];
        UpgradeResearching = new bool[(int)UpgradeId.Max];
    }
}

// Unit type information
public class UnitType: GameDataObject, IEntityFactory
{
    // Race of this entity.
    public Race Race;

    // Identifier.
    public UnitTypeId Ident;

    // Name of the entity.
    public string Name = "Unnamed";

    // LazyLink to the icon.
    public ILazyLink Icon;

    // If the entity can cast spell, and has SpellPoints;
    public bool SpellCaster;

    // If this is set to true the entity lives a limited amount of time.
    // See EntityStats.TimeToLive and stuff
    public bool LimitedTimeToLive = false;

    // If the entity can attack.
    public bool CanAttack;

    // If the entity is a building.
    public bool Building;

    // If the entity can be made into a skeleton using the necrolyte raise dead spell.
    public bool CanRaiseDead = true;

    // Speed of the entity.
    public int Speed;

    // Size of the entity, in tiles.
    public int Size;

    // Maximum hit point of the entity.
    public int MaxHitPoints;

    // Range for AttackAbility.
    public int AttackRange;

    // Piercing damage.
    public int PierceDamage;

    // Normal damage.
    public int NormalDamage;

    // Defense points
    public int Defense;

    // If it can store resources.
    public bool IsStorage = false;

    // If not null, this is how this unit is trained.
    public TrainActivity TrainActivity;

    // Actor used to animate the unit.
    public IActorFactory ActorFactory;

    // HACK: If true then after Animation.Death is broken
    // a secondary corpse is shown.
    public bool SecondaryCorpse;

    // Missile used for attacking. Can be null.
    public MissileType AttackMissile;

    // Cloud spawned when the unit dies.
    public IEntityFactory DeathSpawn;

    // Entity spawned when the entity hits.
    public IEntityFactory AttackHitSpawn;

    public Entity CreateEntity(GameState state, Player player)
    {
        return new Unit((GameState)state, (Player)player, this);
    }

    public UnitType(GameData data): base(data)
    {
    }
}

// Class representing an unit in the game.
// 
// Spell casters gain 1 spell point every 40 cycles for human players and 60 for computers.
// Summoned entities have 255 * 45 cycles to live, after which they lose 5 hp every 45 cycles.
public class Unit: Entity
{
    // Type information.
    public readonly UnitType Type;

    // Override to return a WC1 player.
    public new WarPlayer Player {
        get {
            return (WarPlayer)base.Player;
        }
    }

    // Maximum spell points, constant.
    public const int MaxSpellPoints = 255;

    // Starting spell points, constant
    public const int StartingSpellPoints = 85;

    // Spell points are incremented once every this many cycles.
    // FIXME: this is 60 for computer players.
    public const int SpellPointIncrementInterval = 40;

    // How much time until the next spell point increase
    public int SpellPointTimer;

    // Spell points
    public int SpellPoints;

    // Starting value for TimeToLive, for entities with
    // Type.LimitedTimeToLive;
    public const int MaxTimeToLive = 255;

    // Entities lose TTL(or hp if TTL is 0) every that many cycles.
    public const int TTLUpdateInterval = 45;

    // Entities that are past their TTL lose that many hp every second.
    public const int ExpiredTTLDecayRate = 5;

    // How much time until the next TTL decrease (or hp loss).
    public int TTLTimer;

    // Amount of resources in the unit. Used by peasants/peons and gold mines
    public int GoldAmount;
    public int WoodAmount;

    // How much time this entity has left.
    // You can set this to -1 at any time and the entity won't expire.
    public int TimeToLive;

    // Current hit points of the entity.
    private int hitPoints;
    public int HitPoints {
        get {
            return hitPoints;
        }
        set {
            if (value <= 0) {
                if (!Dead) {
                    OnDeath();
                }
                hitPoints = 0;
            } else {
                if (Dead) {
                    throw new Exception("Resurrection not supported");
                }
                hitPoints = Math.Min(Type.MaxHitPoints, value);
            }
        }
    }

    // If unit is showing it's second corpse.
    private bool SecondCorpse = false;

    // If this is >0 the unit is invulnerable.
    // Will decrease each frame if >0.
    public int InvulnerabilityTicks;

    // If this is >0 the unit is invulnerable.
    // Will decrease each frame if >0.
    public int InvisibilityTicks;

    // If the entity is dead.
    public bool Dead {
        get {
            return HitPoints <= 0;
        }
    }

    // Called when the entity dies.
    private void OnDeath()
    {
        // Disable movement and orders
        OrderQueue = null;
        Locator = null;
        //Locator.SetPathGoal(null);
        //Locator.Transparent = true;

        Actor.StartAnimation(ActorAnimation.Death);
        // Spawn death effect.
        if (Type.DeathSpawn != null) {
            Entity effect;
            effect = Type.DeathSpawn.CreateEntity(State, Player);
            effect.PlaceOnMap(Map, X, Y);
        }
    }

    // Kill the entity.
    // Equivalent to HitPoints = 0;
    public void Kill()
    {
        InvulnerabilityTicks = 0;
        HitPoints = 0;
    }

    // Deals to an entity. Same as HitPoints -= damage.
    public void TakeDamage(int damage)
    {
        // Invulnerable. TeeHee.
        if ((Player.CheatsEnabled & Cheat.Invincibility) != 0 ||
                InvulnerabilityTicks > 0) {
            return;
        }
        HitPoints -= damage;
    }

    // Get the entity speed.
    public override int GetSpeed()
    {
        return Type.Speed;
    }

    // Checks if target is unit and invisibility.
    public override ValidationError ValidateTarget(TargetInfo target)
    {
        ValidationError err;
        if ((err = base.ValidateTarget(target)) != null) {
            return err;
        }
        if (target.HasEntity) {
            Unit tunit = target.Entity as Unit;
            if ((tunit = (target.Entity as Unit)) == null) {
                return new ValidationError("Target entity is not an unit.");
            }
            // Only invisible for other players.
            if (tunit.InvisibilityTicks > 0 && tunit.Player != Player) {
                return new ValidationError("Target unit is invisible.");
            }
        }
        return null;
    }

    // Calculates the damage one hit will deal to the target.
    public int CalculateDamage(Unit target)
    {
        if ((Player.CheatsEnabled & Cheat.Invincibility) != 0) {
            return 255;
        }
        int norm = Type.NormalDamage;
        int pierce = Type.PierceDamage;
        norm -= target.Type.Defense;
        if (norm < 0) {
            norm = 0;
        }
        return Math.Min(norm + pierce, 255);
    }

    // Update game stats.
    public override void NextCycle()
    {
        base.NextCycle();

        if (Removed) {
            return;
        }

        if (Dead) {
            if (Actor.CanBreak()) {
                if (Type.SecondaryCorpse && !SecondCorpse) {
                    // FIXME: hack
                    SecondCorpse = true;
                    Actor = Global.GameData.EffectActors[(int)EffectId.SecondCorpse].CreateActor(this);
                    Actor.StartAnimation((ActorAnimation)(Type.Race == Race.Human ? -2 : -3));
                } else {
                    Remove();
                }
            }
            return;
        }

        // Update spell points.
        if (--SpellPointTimer <= 0) {
            if ((Player.CheatsEnabled & Cheat.Mana) != 0) {
                SpellPointTimer = 1;
            } else {
                SpellPointTimer = SpellPointIncrementInterval;
            }
            SpellPoints = Math.Min(SpellPoints + 1, MaxSpellPoints);
        }

        // Update TTL
        if (TimeToLive != -1) {
            if (--TTLTimer <= 0) {
                if (--TimeToLive <= 0) {
                    TimeToLive = 0;
                    TakeDamage(ExpiredTTLDecayRate);
                }
                TTLTimer = TTLUpdateInterval;
            }
        }

        // Update Invulnerability Ticks
        if (InvulnerabilityTicks > 0) {
            --InvulnerabilityTicks;
        }
        // Update Invisibility Ticks
        if (InvisibilityTicks > 0) {
            --InvisibilityTicks;
        }
    }

    // Constructor.
    public Unit(GameState state, Player p, UnitType type): base(state, p)
    {
        this.Type = type;

        Actor = Type.ActorFactory.CreateActor(this);
        TargetFinder = Global.GameData.TargetFinder;
        Cacher.Size = WarGameData.TileSize * Type.Size;

        CreateLocator();
        Locator.Transparent = false;
        ((TileLocator)Locator).Size = WarGameData.TileSize * Type.Size;

        if (Type.Building) {
            CreateOrderQueue(Global.GameData.IdleOrder);
        } else {
            CreateOrderQueue(Global.GameData.GuardOrder);
        }

        hitPoints = Type.MaxHitPoints;

        // Setup spell points.
        if (Type.SpellCaster) {
            SpellPoints = StartingSpellPoints;
            SpellPointTimer = SpellPointIncrementInterval;
        }

        // Setup TTL
        if (Type.LimitedTimeToLive) {
            TimeToLive = MaxTimeToLive;
            TTLTimer = TTLUpdateInterval;
        } else {
            TimeToLive = -1;
        }
    }

    // Deal damage to all entities in a specified range.
    public static void AreaDamage(GameMap map, int x, int y, int range, int damage)
    {
        if (map == null) {
            throw new ArgumentNullException("map");
        }
        IList entities = map.Cache.SelectList(x - range, y - range, x + range, y + range);
        foreach (Entity entity in entities) {
            if (entity is Unit) {
                ((Unit)entity).TakeDamage(damage);
            }
        }
    }
}

// Base class for activities. Training and upgrades.
// Adds resources, etc.
public abstract class ActivityBase: GameDataObject, IActivity
{
    // The UnitType which can execute this activity.
    public UnitType BuilderUnitType;

    // AllowFeature to check. Defaults to None, which disables this check
    public AllowFeature AllowTest = AllowFeature.None;

    // Resource costs.
    public int GoldCost;
    public int WoodCost;

    // How long it takes, in cycles.
    public int Duration;

    // Check if entity can execute this activity.
    public virtual ValidationError Validate(Object obj)
    {
        Unit unit;
        if (obj == null || obj is Unit == false || 
                (unit = (Unit)obj).Type != BuilderUnitType) {
            return new ValidationError("Unit can't execute this activity");
        }
        if (!((WarGameState)unit.State).IsAllowed(AllowTest)) {
            return new ValidationError("Not available in this scenario");
        }
        if (unit.Player.GoldAmount < GoldCost) {
            return new ValidationError("Not enough gold.");
        }
        if (unit.Player.WoodAmount < WoodCost) {
            return new ValidationError("Not enough wood.");
        }
        return null;
    }

    // Get UnitStats from entity, throwing an exception if UnitType is bad.
    // Will not check resource costs.
    protected Unit GetUnit(Object obj)
    {
        Unit unit;
        if (obj == null || obj is Unit == false || (unit = (Unit)obj).Type != BuilderUnitType) {
            throw new InvalidOperationException("This entity can't execute this activity");
        }
        return unit;
    }

    // Constant, returns duration. Implements cheating.
    // FIXME: not constant, bad?
    public int GetDuration(object obj)
    {
        if ((GetUnit(obj).Player.CheatsEnabled & Cheat.BuildSpeed) != 0) {
            return Duration / 10;
        } else {
            return Duration;
        }
    }

    // Get ready to rumble. Takes resource from the player.
    public virtual void Start(object obj)
    {
        Unit unit = GetUnit(obj);
        if (unit.Player.GoldAmount < GoldCost ||
            unit.Player.WoodAmount < WoodCost) {
            throw new InvalidOperationException("This entity can't execute this activity");
        }
        unit.Player.GoldAmount -= GoldCost;
        unit.Player.WoodAmount -= WoodCost;
    }

    // Gives your money back by default.
    public virtual void Abort(object obj, int progress)
    {
        Unit unit = GetUnit(obj);
        unit.Player.GoldAmount += GoldCost;
        unit.Player.WoodAmount += WoodCost;
    }

    // Override to do the monkey.
    public abstract void Finish(object obj);

    public ActivityBase(GameData data): base(data)
    {
    }
}

// Activity used to train new units.
public class TrainActivity: ActivityBase, IActivity
{
    // The type of unit to create.
    public UnitType TrainedUnitType;

    // Place the actual unit, or give your money back.
    public override void Finish(object obj)
    {
        Unit trainer = GetUnit(obj);
        Unit unit = (Unit)Utilities.TryCreatePlaceEntity(TrainedUnitType, trainer.Player,
                trainer.Map, trainer.X, trainer.Y, 3 * WarGameData.TileSize);
        if (unit == null) {
            // Money back.
            //Console.WriteLine("Failed to create {0}, giving money back.", TrainedUnitType.Name);
            trainer.Player.GoldAmount += GoldCost;
            trainer.Player.WoodAmount += WoodCost;
        }
    }

    public TrainActivity(GameData data): base(data)
    {
    }
}

// Activity used for upgrades.
// This is for a certain level of an UpgradeIdent.
public class UpgradeActivity: ActivityBase, IActivity
{
    // Upgrade identifier
    public UpgradeId UpgradeIdent;

    // The level to upgrade to.
    // Defaults to 1.
    public int UpgradeLevel = 1;

    public override ValidationError Validate(object obj)
    {
        ValidationError err;
        Unit unit = GetUnit(obj);

        // Check base.Validation.
        if ((err = base.Validate(unit)) != null ) {
            return err;
        }
        // Check level. Current level must be UpgradeLevel - 1
        if (unit.Player.UpgradeLevels[(int)UpgradeIdent] > UpgradeLevel - 1) {
            return new ValidationError("Upgrade already researched.");
        }
        if (unit.Player.UpgradeLevels[(int)UpgradeIdent] < UpgradeLevel - 1) {
            return new ValidationError("Must research lower levels first.");
        }
        // Check multiple researching.
        if (unit.Player.UpgradeResearching[(int)UpgradeIdent]) {
            return new ValidationError("Upgrade already being researched.");
        }
        
        return null;
    }

    // Override, mark as researching.
    public override void Start(object obj)
    {
        Unit unit = GetUnit(obj);
        unit.Player.UpgradeResearching[(int)UpgradeIdent] = true;
        base.Start(obj);
    }

    // Override, mark as researching.
    public override void Abort(object obj, int progress)
    {
        Unit unit = GetUnit(obj);
        unit.Player.UpgradeResearching[(int)UpgradeIdent] = false;
        base.Abort(obj, progress);
    }

    public override void Finish(object obj)
    {
        Unit unit = GetUnit(obj);
        unit.Player.UpgradeResearching[(int)UpgradeIdent] = false;
        ++unit.Player.UpgradeLevels[(int)UpgradeIdent];
    }

    public UpgradeActivity(GameData data): base(data)
    {
    }
}

