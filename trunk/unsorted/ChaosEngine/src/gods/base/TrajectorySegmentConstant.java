package gods.base;

import gods.base.HostileTrajectory.Type;

public interface TrajectorySegmentConstant {

	public abstract boolean to_location();

	public abstract Locatable get_location();

	public abstract int get_duration();

	public Type get_type();

}