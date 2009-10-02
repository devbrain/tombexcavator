package base;

import java.awt.Rectangle;

public interface LayerObject<T> extends Nameable<T>, Describable
{
	public void get_bounds(Rectangle r);
}
