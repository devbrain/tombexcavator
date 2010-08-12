// Copyright (C) 2005-2006 Crestez Leonard    cleonard@go.ro
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

using Alpha.Graphics;
using Alpha.Game;
using Alpha.Util;

// Unit hit attack effect, used by AttackAbility.
public class HitEffect: ISpellEffect
{
    // Caster for the spell.
    public readonly Unit hitter;
    public Unit Hitter {
        get {
            return hitter;
        }
    }

    // Shortcut to GameData.
    public WarGameData Data {
        get {
            return (WarGameData)Hitter.Data;
        }
    }

    // Hit the target.
    public void Execute(TargetInfo target)
    {
        if (Data.AttackAbility.Validate(Hitter, target) != null) {
            return;
        }
        Unit dest = (Unit)target.Entity;

        dest.TakeDamage(Hitter.CalculateDamage(dest));

        // Show hit spawn.
        if (Hitter.Type.AttackHitSpawn != null) {
            Utilities.TryCreatePlaceEntity(
                    Hitter.Type.AttackHitSpawn, Hitter.Player, target, 0);
        }
    }

    // Constructor.
    public HitEffect(Unit hitter)
    {
        if (hitter == null) {
            throw new ArgumentNullException("hitter");
        }
        this.hitter = hitter;
    }
}

// Attack ability
public class AttackAbility: GameDataObject, IAbility
{
    // Constant.
    public ActorAnimation GetAnimation(Entity caster)
    {
        return ActorAnimation.Action;
    }

    // Returns UnitType.AttackRange
    public int GetRange(Entity caster)
    {
        return ((Unit)caster).Type.AttackRange;
    }

    // Checks if the target is valid.
    public ValidationError Validate(Entity caster, TargetInfo target)
    {
        Unit cunit = caster as Unit;
        ValidationError err;

        if (cunit == null) {
            return new ValidationError("Only units can attack.");
        }
        if (cunit.Type.CanAttack == false) {
            return new ValidationError("Unit can't attack.");
        }
        if ((err = caster.ValidateTarget(target)) != null) {
            return err;
        }

        Unit tunit = target.Entity as Unit;
        if (tunit == null) {
            return new ValidationError("Unit target required.");
        }
        if (tunit == caster) {
            return new ValidationError("Unit can't attack itself.");
        }
        if (tunit.Dead) {
            return new ValidationError("Target unit is dead.");
        }

        return null;
    }

    // Checks Validate.
    public bool GetRepeatCast(Entity entity, TargetInfo target)
    {
        return Validate(entity, target) == null;
    }

    // Execute, deal damage.
    public void Execute(Entity entity, TargetInfo target)
    {
        if (Validate(entity, target) != null) {
            return;
        }
        Unit unit = (Unit)entity;
        ISpellEffect effect = new HitEffect(unit);
        if (unit.Type.AttackMissile != null) {
            unit.Type.AttackMissile.Launch(entity, target, effect);
        } else {
            effect.Execute(target);
        }
    }

    public AttackAbility(GameData data): base(data)
    {
    }
}

// Warcraft1 TargetFinder. Mostly hardcoded.
public class TargetFinder: TargetFinderBase
{
    // FIXME: Vision range.
    public override int GetAcquireRadius(Entity entity)
    {
        return Math.Max(((Unit)entity).Type.AttackRange, 5 * WarGameData.TileSize);
    }

    // 3/2 AttackRange
    public override int GetKeepRadius(Entity entity)
    {
        return ((Unit)entity).Type.AttackRange * 3 / 2;
    }

    // Convenience accesor for WarGameData.
    public new WarGameData Data {
        get {
            return (WarGameData)base.Data;
        }
    }

    public override bool EvaluateTarget(Entity cent, Entity tent, out IAbility abil, out int score)
    {
        Unit cunit = (Unit)cent;
        Unit tunit = (Unit)tent;

        int range = GetAcquireRadius(cent);

        // Cleric heal, always better than cleric attack.
        if (Data.HealSpell.Validate(cunit, new TargetInfo(tunit)) == null &&
                cent.Player.ShouldAttack(tent) == false) {
            // Distance doesn't matter, healing has a huge range anyway.
            abil = Data.HealSpell;
            score = 7 * range - tunit.HitPoints;
            return true;
        }

        // Necrolyte raise dead. Validates caster(mana) and targets dead units.
        if (Data.RaiseDeadSpell.ValidateCaster(cunit) == null &&
                tunit.Dead && tunit.Type.CanRaiseDead) {
            // Raises closest corpse, but range is huge anyway
            abil = Data.RaiseDeadSpell;
            score = 7 * range - IntMath.Distance(cent.Pos, tent.Pos);
            return true;
        }

        // Attacking.
        if (Data.AttackAbility.Validate(cent, new TargetInfo(tent)) == null &&
                cent.Player.ShouldAttack(tent)) {
            // FIXME: prefer low hp targets.
            // FIXME: prefer low armor depending on pierce, etc.
            int dist = IntMath.Distance(cent.Pos, tent.Pos) - cunit.Type.AttackRange;
            // Offensive units are always better targets.
            if (tunit.Type.CanAttack) {
                score = 5 * range - dist;
            } else {
                score = 3 * range - dist;
            }
            abil = Data.AttackAbility;
            return true;
        }

        // Nothing.
        abil = null;
        score = 0;
        return false;
    }

    public TargetFinder(GameData data): base(data)
    {
    }
}

// Simple area damage effect.
// FIXME: sort of a hack.
public class AreaDamageEffect: GameObject, ISpellEffect
{
    // Range around the target to damage.
    public int Range;

    // Damage amount.
    public int Damage;

    public void Execute(TargetInfo target)
    {
        // Deal area damage.
        Unit.AreaDamage(target.Map, target.X, target.Y, Range, Damage);
    }

    public AreaDamageEffect(GameState state, int damage, int range): base(state)
    {
        this.Range = range;
        this.Damage = damage;
    }
}

// Simple summon effect using an IEntityFactory
// FIXME: move to Alpha.Game
public class SummonEffect: GameObject, ISpellEffect
{
    // Factory used to create the entity to be placed at the target.
    public IEntityFactory Factory;

    // Player to own the new unit.
    // FIXME: sucks.
    public Player Player;

    // Range around the target to place the new unit in.
    // Otherwise the unit is destroyed.
    public int PlacementRange;

    public void Execute(TargetInfo target)
    {
        Utilities.TryCreatePlaceEntity(Factory, Player, target, PlacementRange);
    }

    // Initializing constructor.
    public SummonEffect(GameState state, IEntityFactory factory, Player player): base(state)
    {
        this.Factory = factory;
        this.Player = player;
    }
}

// Abstract spell class. All WC1 spells are instantaneous.
public abstract class Spell: GameDataObject, IAbility
{
    // Converts to WC1 GameData.
    public new WarGameData Data {
        get {
            return (WarGameData)base.Data;
        }
    }

    // Cost in mana points. 0 disables
    public int SpellPointCost = 0;

    // The type of the caster. Can be null, disables this check.
    public UnitType CasterType;

    // Effect shown over the target when casting.
    public IEntityFactory TargetSpawn;

    // If the spell should be cast multiple times.
    public bool RepeatCast;

    // Upgrade required, or UpgradeId.Max to disable
    public UpgradeId UpgradeRequired = UpgradeId.Max;

    // If true then the spell needs a target.
    // If this is false then the caster can only cast on himself.
    // If this is false then a lot of other properties are not used.
    public virtual bool NeedsTarget {
        get {
            return true;
        }
    }

    // True if it caster can target itself
    // Defaults to false.
    public virtual bool CanTargetSelf {
        get {
            return false;
        }
    }

    // If an unit target is required.
    public virtual bool NeedsUnitTarget {
        get {
            return false;
        }
    }

    // If it requires a living target.
    public virtual bool NeedsLivingTarget {
        get {
            return true;
        }
    }

    // If it requires a creature (non-building) target.
    public virtual bool NeedsCreatureTarget {
        get {
            return true;
        }
    }

    // Range. If -1 then UnitType.AttackRange is used.
    public int Range = -1;

    // Validates caster.
    public virtual ValidationError ValidateCaster(Unit caster)
    {
        if (SpellPointCost != 0 && caster.SpellPoints < SpellPointCost) {
            return new ValidationError("Not enough spell points.");
        }
        if (CasterType != null && caster.Type != CasterType) {
            return new ValidationError("Unit can't cast this spell.");
        }
        if (UpgradeRequired != UpgradeId.Max &&
                caster.Player.UpgradeLevels[(int)UpgradeRequired] == 0) {
            return new ValidationError("Unit can't cast this spell.");
        }
        return null;
    }

    // Validation.
    public virtual ValidationError Validate(Unit caster, TargetInfo target)
    {
        ValidationError err;

        // Validate caster.
        if ((err = ValidateCaster(caster)) != null) {
            return err;
        }
        
        // Handle self-cast spells.
        if (!NeedsTarget) {
            if (target.Entity != caster) {
                return new ValidationError("Spell doesn't need a target.");
            }
            return null;
        }

        // Validate target.
        if ((err = caster.ValidateTarget(target)) != null) {
            return err;
        }
        if (target.Entity == caster && !CanTargetSelf) {
            return new ValidationError("Unit can't target itself.");
        }

        // Validate unit target.
        if (NeedsUnitTarget) {
            Unit tunit = target.Entity as Unit;
            if (tunit == null) {
                return new ValidationError("Unit target required.");
            }
            if (tunit.Dead && NeedsLivingTarget) {
                return new ValidationError("Target unit is dead.");
            }
            if (tunit.Type.Building && NeedsCreatureTarget) {
                return new ValidationError("Can't target a building.");
            }
        }

        return null;
    }

    // Cast the spell. Assumes validation went OK.
    // Override this to do the monkey.
    public abstract void Cast(Unit caster, TargetInfo target);

    // Constructor.
    public Spell(WarGameData data): base(data)
    {
        TargetSpawn = data.SparksCloud;
    }

    // IAbility.GetAnimation, constant ActorAnimation.Action.
    ActorAnimation IAbility.GetAnimation(Entity caster)
    {
        return ActorAnimation.Action;
    }

    // See this.Range.
    int IAbility.GetRange(Entity caster)
    {
        if (Range == -1) {
            return ((Unit)caster).Type.AttackRange;
        } else {
            return Range;
        }
    }

    // Checks if caster is unit and then calls Spell.Validate.
    ValidationError IAbility.Validate(Entity caster, TargetInfo target)
    {
        Unit unit = caster as Unit;
        if (unit == null) {
            return new ValidationError("Caster is not an unit.");
        }
        return Validate(unit, target);
    }

    // See RepeatCast.
    bool IAbility.GetRepeatCast(Entity caster, TargetInfo target)
    {
        if (RepeatCast) {
            return ((IAbility)this).Validate(caster, target) == null;
        } else {
            return false;
        }
    }

    // Calls this.Cast after validating.
    void IAbility.Execute(Entity caster, TargetInfo target)
    {
        if (((IAbility)this).Validate(caster, target) == null) {
            this.Cast((Unit)caster, target);
        }
    }
}

// Summon spell.
// HACK: used for poison cloud and fire rain too.
public class SummonSpell: Spell
{
    // Type of entity to summon.
    public IEntityFactory EntityFactory;

    // If true then summon multiple times.
    public bool MultiSummon = true;

    // Constant placement range.
    public static readonly int PlacementRange = 2 * WarGameData.TileSize;

    // Doesn't need a target by default.
    private bool needsTarget = false;
    public override bool NeedsTarget {
        get {
            return needsTarget;
        }
    }

    // FIXME: argh! C# 1.0
    public void SetNeedsTarget(bool val)
    {
        needsTarget = val;
    }

    // Execute the spell.
    public override void Cast(Unit caster, TargetInfo target)
    {
        do {
            Entity spawn = Utilities.TryCreatePlaceEntity(
                    EntityFactory, caster.Player, target, PlacementRange);
            if (spawn != null) {
                if (TargetSpawn != null) {
                    Utilities.TryCreatePlaceEntity(TargetSpawn, caster.Player, new TargetInfo(spawn), 0);
                }
                if (SpellPointCost != 0) {
                    caster.SpellPoints -= SpellPointCost;
                }
            }
        } while (caster.SpellPoints > SpellPointCost && MultiSummon);
    }

    public SummonSpell(WarGameData data): base(data)
    {
    }
}

// Produces warlock poison clouds.
public class PoisonCloudType: CloudType
{
    // For how long this spell will last.
    public int Duration = 800;

    // Damage to deal every DamageInterval
    public int Damage = 5;

    // Interval at which to deal damage.
    public int DamageInterval = 32;

    // AreaDamage range.
    public int DamageRange = WarGameData.TileSize * 3 / 2;

    // Create a new damage cloud.
    public PoisonCloudType(GameData data, IActorFactory factory): base(data)
    {
        ActorFactory = factory;
        MaxAge = Duration;
    }

    // Update cloud.
    public override void CloudNextCycle(Cloud cloud)
    {
        base.CloudNextCycle(cloud);

        if (cloud.Removed) {
            return;
        }

        if (cloud.Age % DamageInterval == 0) {
            Unit.AreaDamage(cloud.Map, cloud.X, cloud.Y, DamageRange, Damage);
        }
    }
}

// Trivial factory for FireRain
public class FireRainFactory: IEntityFactory
{
    public Entity CreateEntity(GameState state, Player player)
    {
        return new FireRain(state, player);
    }
}

// Wizzard fire rain.
// This code is very very sensible and relies on details of wc1 data
public class FireRain: Entity
{
    // How many pixels above to drop from.
    // This is sort of faked, the missile is dropped from y - DropHeight,
    // Z is not affected.
    public const int DropHeight = 120;

    // The size of the area to drop in, in map tiles.
    // Should be an odd number.
    public const int DropRange = 7;

    // How much time until the drop hits.
    public const int DropInterval = 32;

    // How many rounds to drop
    public const int RoundCount = 4;

    // How many drops to drop.
    public const int DropCount = 4;

    // Damage to deal to an entity caught under the rain.
    public const int Damage = 32;

    // AreaDamage range.
    public const int DamageRange = WarGameData.TileSize * 3 / 2;

    // How old this spell is.
    private int Age = 0;

    // Locations of the drops. New drops from the same rain fall on the
    // same positions.
    private Point[] DropLocations;

    // Empty constructor.
    public FireRain(GameState state, Player p): base(state, p)
    {
    }

    // HACK: initialize locations.
    protected override void OnPlacedOnMap(GameMap map, Point pos)
    {
        // Initialize DropLocations.
        // Must be different.
        int i, j;
        DropLocations = new Point[DropCount];
        for (i = 0; i < DropCount; ++i) {
            DropLocations[i].X = X + (State.SyncRand(DropRange) - DropRange / 2) * WarGameData.TileSize;
            DropLocations[i].Y = Y + (State.SyncRand(DropRange) - DropRange / 2) * WarGameData.TileSize;
            for (j = 0; j < i; ++j) {
                if (DropLocations[i] == DropLocations[j]) {
                    --i;
                    break;
                }
            }
        }
        base.OnPlacedOnMap(map, pos);
    }

    // Start another round of rain.
    private void Drop()
    {
        ISpellEffect effect = new MultiEffect(
            new AreaDamageEffect(State, Damage, DamageRange),
            new SummonEffect(State, Global.GameData.ExplosionCloud, State.GetPlayer(4))
        );
        foreach (Point p in DropLocations) {
            Global.GameData.FireDropMissile.Launch(State.GetPlayer(4), Map,
                    new Point(p.X, p.Y - DropHeight * WarGameData.TileSize / 16),
                    new TargetInfo(Map, p.X, p.Y), effect);
        }
    }

    // Update cloud.
    public override void NextCycle()
    {
        base.NextCycle();
        if (Age % DropInterval == 0 && OffMap == false) {
            Drop();
        }
        if (Age >= DropCount * DropInterval) {
            Remove();
            return;
        }
        ++Age;
    }
}

// Cleric healing.
public class HealSpell: Spell
{
    // True.
    public override bool NeedsUnitTarget {
        get {
            return true;
        }
    }

    // True.
    public override bool NeedsCreatureTarget {
        get {
            return true;
        }
    }

    // Maximum number of hit points to gain.
    public int MaxHeal = 40;

    // Checks not fully healed.
    public override ValidationError Validate(Unit caster, TargetInfo target)
    {
        ValidationError err;
        if ((err = base.Validate(caster, target)) != null) {
            return err;
        }
        Unit tunit = target.Entity as Unit;
        if (tunit.HitPoints >= tunit.Type.MaxHitPoints) {
            return new ValidationError("Target already fully healed.");
        }
        return null;
    }

    public override void Cast(Unit caster, TargetInfo target)
    {
        if (Validate(caster, target) != null) {
            return;
        }
        Unit tunit = (Unit)target.Entity;

        int heal;
        heal = Math.Min(tunit.Type.MaxHitPoints - tunit.HitPoints, MaxHeal);
        heal = Math.Min(heal, caster.SpellPoints / SpellPointCost);
        tunit.HitPoints += heal;
        caster.SpellPoints -= heal * SpellPointCost;

        if (TargetSpawn != null) {
            Utilities.TryCreatePlaceEntity(TargetSpawn, caster.Player, target, 0);
        }
    }

    // Constructor.
    public HealSpell(WarGameData data): base(data)
    {
        SpellPointCost = 3;
        Range = 8 * WarGameData.TileSize;
        CasterType = data.Units[(int)UnitTypeId.Cleric];
    }
}

// Necrolyte raise dead.
public class RaiseDeadSpell: Spell
{
    public override bool NeedsLivingTarget {
        get {
            return false;
        }
    }

    public override void Cast(Unit cunit, TargetInfo target)
    {
        if (Validate(cunit, target) != null) {
            return;
        }
        int r = 3 * WarGameData.TileSize;
        IList list = cunit.Map.Cache.SelectList(target.X - r, target.Y - r, target.X + r, target.Y + r);

        foreach (Entity u in list) {
            Unit tunit = u as Unit;
            if (tunit == null) {
                continue;
            }
            if (tunit.Dead && tunit.Type.CanRaiseDead) {
                int x = u.X;
                int y = u.Y;

                u.RemoveFromMap();
                Entity skel = Utilities.TryCreatePlaceEntity(Data.Units[(int)UnitTypeId.Skeleton],
                        cunit.Player, cunit.Map, x, y, 3 * WarGameData.TileSize);

                if (skel == null) {
                    Console.WriteLine("Failed to create skeleton at {0} {1}", x, y);
                    // Can't fail.
                    u.PlaceOnMap(cunit.Map, x, y);
                } else {
                    u.Remove();
                    skel.Actor.StartAnimation(ActorAnimation.Birth);
                    Utilities.OrientEntity(skel, cunit.Pos);
                    cunit.SpellPoints -= SpellPointCost;
                    if (cunit.SpellPoints < SpellPointCost) {
                        break;
                    }
                    if (TargetSpawn != null) {
                        Utilities.TryCreatePlaceEntity(TargetSpawn, cunit.Player, new TargetInfo(skel), 0);
                    }
                }
            }
        }
    }

    public RaiseDeadSpell(WarGameData data): base(data)
    {
        SpellPointCost = 70;
        Range = 5 * WarGameData.TileSize;
        CasterType = data.Units[(int)UnitTypeId.Necrolyte];
    }
}

// Cleric invisibility spell.
public class InvisibilitySpell: Spell
{
    // Only works on unit.
    public override bool NeedsUnitTarget {
        get {
            return true;
        }
    }

    // Max length of invisibility.
    public int SpellDuration;

    // Check if the target is valid.
    public override ValidationError Validate(Unit unit, TargetInfo target)
    {
        ValidationError err;
        if ((err = base.Validate(unit, target)) != null) {
            return err;
        }
        Unit tunit = (Unit)target.Entity;
        if (tunit.InvisibilityTicks > 0) {
            return new ValidationError("Target already invisible");
        }
        return null;
    }

    // Execute the ability.
    public override void Cast(Unit caster, TargetInfo target)
    {
        if (Validate(caster, target) != null) {
            return;
        }
        Unit tunit = (Unit)target.Entity;

        tunit.InvisibilityTicks = SpellDuration;
        caster.SpellPoints -= SpellPointCost;
        Utilities.TryCreatePlaceEntity(TargetSpawn, caster.Player, target, 0);
    }

    public InvisibilitySpell(WarGameData data): base(data)
    {
        SpellPointCost = 125;
        Range = 8 * WarGameData.TileSize;
        SpellDuration = 3000;
        CasterType = data.Units[(int)UnitTypeId.Cleric];
    }
}

// Cleric invisibility spell.
public class UnholyArmorSpell: Spell
{
    // Only works on unit.
    public override bool NeedsUnitTarget {
        get {
            return true;
        }
    }

    // Max length of invisibility.
    public int SpellDuration;

    // Check if the target is valid.
    public override ValidationError Validate(Unit unit, TargetInfo target)
    {
        ValidationError err;
        if ((err = base.Validate(unit, target)) != null) {
            return err;
        }
        Unit tunit = (Unit)target.Entity;
        if (tunit.InvulnerabilityTicks > 0) {
            return new ValidationError("Target already has unholy armor.");
        }
        return null;
    }

    // Execute the ability.
    public override void Cast(Unit caster, TargetInfo target)
    {
        if (Validate(caster, target) != null) {
            return;
        }
        Unit tunit = (Unit)target.Entity;
        tunit.InvulnerabilityTicks = SpellDuration;
        if (tunit.HitPoints > 30) {
            tunit.HitPoints /= 2;
        } else {
            tunit.HitPoints = 1;
        }
        caster.SpellPoints -= SpellPointCost;
        Utilities.TryCreatePlaceEntity(TargetSpawn, caster.Player, target, 0);
    }

    public UnholyArmorSpell(WarGameData data): base(data)
    {
        SpellPointCost = 125;
        Range = 8 * WarGameData.TileSize;
        SpellDuration = 1000;
        CasterType = data.Units[(int)UnitTypeId.Necrolyte];
    }
}
