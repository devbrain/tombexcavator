package base;

public interface EditableData extends Modifiable 
{
	public void load(String project_file) throws java.io.IOException;
	public boolean save() throws java.io.IOException;
	public void new_project(String file);
	public String get_project_file();
	public void set_project_file(String file);
	public void export(String file);
	
}
