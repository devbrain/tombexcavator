package base;

public interface Describable
{
	void serialize(sys.ParameterParser fw) throws java.io.IOException;
	String get_block_name();
}
