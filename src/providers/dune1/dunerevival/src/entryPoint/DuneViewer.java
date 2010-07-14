package entryPoint;

import gui.BackgroundImageDesktopPane;
import gui.FileTree;
import gui.RessourceInternalFrame;
import gui.SpriteInternalFrame;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyVetoException;
import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JSplitPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import viewer.LoadException;
import viewer.Viewer;

public class DuneViewer extends JFrame /*implements TreeSelectionListener*/ {

	class ViewerRessource {
		public File file;

		public RessourceMenuItem menuItem;

		public RessourceInternalFrame internalFrame;
		// TODO : ajouter les donnees
	}

	class RessourceMenuItem extends JMenuItem {
		public RessourceInternalFrame linkedFrame;

		public RessourceMenuItem(RessourceInternalFrame linkedFrame) {
			this.linkedFrame = linkedFrame;
		}
	}

	private JMenuItem helpMenuItem;
	private JMenu jMenu5;
	private FileTree jTree1;
	private JSplitPane jSplitPane1;
	private JMenuItem copyMenuItem;
	private JMenuItem cutMenuItem;
	private JMenu jMenu4;
	private JMenuItem newFileMenuItem;
	private JMenu jMenu3;
	private JMenu jMenu2;
	private JMenuBar jMenuBar1;
	private BackgroundImageDesktopPane desktopPane;
	private ViewerRessource selectedRessource;
	private ArrayList<ViewerRessource> openRessources = new ArrayList<ViewerRessource>();

	public Viewer viewerCore = new Viewer();
	
	public DuneViewer(int width, int height) throws LoadException {
		super();
		
		viewerCore.load();
		
		initGUI(width, height);
	}

	private void initGUI(int width, int height) {
		try {

			setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
			{

				getContentPane().add(new JLabel(Messages.getString("DuneViewer.0")), BorderLayout.SOUTH); //$NON-NLS-1$
				jSplitPane1 = new JSplitPane();
				getContentPane().add(jSplitPane1, BorderLayout.CENTER);

				desktopPane = new BackgroundImageDesktopPane(
						Messages.getString("DuneViewer.1")); //$NON-NLS-1$
				desktopPane.setBackground(new Color(0.8f, 0.9f, 0.8f));
				jSplitPane1.add(desktopPane, JSplitPane.RIGHT);

				jTree1 = new FileTree(Messages.getString("DuneViewer.2"), this); //$NON-NLS-1$
				jSplitPane1.add(jTree1, JSplitPane.LEFT);

			}

			setSize(width, height);
			setTitle(Messages.getString("DuneViewer.3")); //$NON-NLS-1$
			setIconImage(Toolkit.getDefaultToolkit().getImage(
					Messages.getString("DuneViewer.4"))); //$NON-NLS-1$
			{
				jMenuBar1 = new JMenuBar();
				setJMenuBar(jMenuBar1);
				{
					jMenu3 = new JMenu();
					jMenuBar1.add(jMenu3);
					jMenu3.setText(Messages.getString("DuneViewer.5")); //$NON-NLS-1$
					{
						newFileMenuItem = new JMenuItem();
						jMenu3.add(newFileMenuItem);
						newFileMenuItem.setText(Messages.getString("DuneViewer.6")); //$NON-NLS-1$
						newFileMenuItem.addActionListener(new ActionListener() {
							public void actionPerformed(ActionEvent evt) {
								onExport();
							}
						});
					}
				}
				{
					jMenu4 = new JMenu();
					jMenuBar1.add(jMenu4);
					jMenu4.setText(Messages.getString("DuneViewer.7")); //$NON-NLS-1$
					{
						cutMenuItem = new JMenuItem();
						jMenu4.add(cutMenuItem);
						cutMenuItem.setText(Messages.getString("DuneViewer.8")); //$NON-NLS-1$
						cutMenuItem.addActionListener(new ActionListener() {
							public void actionPerformed(ActionEvent evt) {
								onOptions();
							}
						});
					}
					{
						copyMenuItem = new JMenuItem();
						jMenu4.add(copyMenuItem);
						copyMenuItem.setText(Messages.getString("DuneViewer.9")); //$NON-NLS-1$
						copyMenuItem.addActionListener(new ActionListener() {
							public void actionPerformed(ActionEvent evt) {
								onFindDune();
							}
						});
					}
				}
				{
					jMenu2 = new JMenu();
					jMenuBar1.add(jMenu2);
					jMenu2.setText(Messages.getString("DuneViewer.10")); //$NON-NLS-1$

					// JSeparator s = new JSeparator();
					// jMenu2.add(s);
				}
				{
					jMenu5 = new JMenu();
					jMenuBar1.add(jMenu5);
					jMenu5.setText(Messages.getString("DuneViewer.11")); //$NON-NLS-1$
					{
						helpMenuItem = new JMenuItem();
						jMenu5.add(helpMenuItem);
						helpMenuItem.setText(Messages.getString("DuneViewer.12")); //$NON-NLS-1$
						helpMenuItem.addActionListener(new ActionListener() {
							public void actionPerformed(ActionEvent evt) {
								onAbout();
							}
						});
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
			SwingUtilities.updateComponentTreeUI(this);
			// force chaque composant de la fenêtre à appeler sa méthode
			// updateUI
		} catch (InstantiationException e) {
		} catch (ClassNotFoundException e) {
		} catch (UnsupportedLookAndFeelException e) {
		} catch (IllegalAccessException e) {
		}
		
		
		setFocusOnRessource(null);
	}

	public void openRessource(File openedFile) {

		//one cannot open directories
		if (!openedFile.isDirectory()) {
			
			//is it really a Dune file ?
			if (viewerCore.recognizer.getCryoFile(openedFile.getName())!= null) {
				// verify that file is not already open :
				Iterator<ViewerRessource> it = openRessources.iterator();
				while (it.hasNext()) {
					ViewerRessource res = it.next();
					//if the file was previously opened we set focus to it's frame
					if (res.file == openedFile) {
						System.err.println(Messages.getString("DuneViewer.13")); //$NON-NLS-1$
						setFocusOnRessource(res);
						return;
					}
				}
	
				// if not already open, then open new internal frame :
	
				// create container
				ViewerRessource ressource = new ViewerRessource();
	
				ressource.file = openedFile;
	
				// create new frame
				if (true) { //in the future, create right frame-type for right ressource-type
					ressource.internalFrame = new SpriteInternalFrame(openedFile, this);
				}
				
				ressource.internalFrame.setVisible(true);
				ressource.internalFrame.setResizable(false);
				desktopPane.add(ressource.internalFrame);
	
				// calculate new Frame's coordinates
				int x = (openRessources.size() * 20) % desktopPane.getWidth();
				int y = (openRessources.size() * 20) % desktopPane.getWidth();
				ressource.internalFrame.setLocation(x, y);
	
				// add frame to the "Windows" menu
				ressource.menuItem = new RessourceMenuItem(ressource.internalFrame);
				// make sure that frame is focused when menu item is selected
				ressource.menuItem.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent evt) {
						System.err.println(Messages.getString("DuneViewer.14")); //$NON-NLS-1$
						onSelectRessourceMenuItem((RessourceMenuItem) evt.getSource());
					}
				});
				jMenu2.add(ressource.menuItem);
				ressource.menuItem.setText(openedFile.toString());
	
				openRessources.add(ressource);
	
				JOptionPane.showMessageDialog(this, "This file is recognized as a Cryo file, but i cannot be opened in this version of the Viewer");
				
				// set focus to new frame
				onSelectRessourceInternalFrame(ressource.internalFrame);
			}
			else
			{
				JOptionPane.showMessageDialog(this, "This file isn't recognized as a Cryo File");
			}
		}
	}

	public void setFocusOnRessource(ViewerRessource res) {
		if (res != null) {
			try {
				res.internalFrame.setSelected(true);
			} catch (PropertyVetoException ex) {
				// do nothing
			}
			selectedRessource = res;

			System.err.println(Messages.getString("DuneViewer.15") + res.file.toString() + Messages.getString("DuneViewer.16")); //$NON-NLS-1$ //$NON-NLS-2$

			// activate "File" menu
			jMenu3.setEnabled(true);
			//activate "window" menu
			jMenu2.setEnabled(true);
		} else {
			selectedRessource = null;
			System.err.println(Messages.getString("DuneViewer.17")); //$NON-NLS-1$
			// deactivate "File" menu
			jMenu3.setEnabled(false);
			//deactivate "window" menu
			jMenu2.setEnabled(false);
		}
	}

	public ViewerRessource findRessourceByFrame(RessourceInternalFrame frame) {
		// look for ressource to focus
		Iterator<ViewerRessource> it = openRessources.iterator();
		while (it.hasNext()) {
			ViewerRessource res = it.next();
			if (res.internalFrame == frame) {
				return res;
			}
		}
		return null;
	}

	public ViewerRessource findRessourceByMenuItem(RessourceMenuItem item) {
		// look for ressource to focus
		Iterator<ViewerRessource> it = openRessources.iterator();
		while (it.hasNext()) {
			ViewerRessource res = it.next();
			if (res.menuItem == item) {
				return res;
			}
		}
		return null;
	}

	public void onSelectRessourceInternalFrame(RessourceInternalFrame frame) {
		setFocusOnRessource(findRessourceByFrame(frame));
	}

	public void onSelectRessourceMenuItem(RessourceMenuItem item) {
		setFocusOnRessource(findRessourceByMenuItem(item));
	}

	public void onCloseRessourceWindow(RessourceInternalFrame frame) {
		ViewerRessource res = findRessourceByFrame(frame);
		if (frame != null) {
			jMenu2.remove(res.menuItem);
			openRessources.remove(res);
		}
		// set focus on another frame
		if (openRessources.size() != 0) {
			setFocusOnRessource(openRessources.get(0));
		} else {
			setFocusOnRessource(null);
		}

	}
/*
	public void valueChanged(TreeSelectionEvent e) {
		openRessourceWindow(e.getPath());
	}
*/
	public void onOptions() {
		System.err.println(Messages.getString("DuneViewer.18")); //$NON-NLS-1$
		String message = Messages.getString("DuneViewer.19"); //$NON-NLS-1$
		JOptionPane.showMessageDialog(this, message);
	}
	
	public void onExport() {
		System.err.println(Messages.getString("DuneViewer.20")); //$NON-NLS-1$
		String message = Messages.getString("DuneViewer.21"); //$NON-NLS-1$
		JOptionPane.showMessageDialog(this, message);
	}

	public void onFindDune() {
		System.err.println(Messages.getString("DuneViewer.22")); //$NON-NLS-1$
		String message = Messages.getString("DuneViewer.23"); //$NON-NLS-1$
		JOptionPane.showMessageDialog(this, message);
	}

	public void onAbout() {
		System.err.println(Messages.getString("DuneViewer.24")); //$NON-NLS-1$
		String message = Messages.getString("DuneViewer.25"); //$NON-NLS-1$
		JOptionPane.showMessageDialog(this, message);
	}

	public static void main(String[] args) {
		try {
			DuneViewer inst = new DuneViewer(800, 600);
			inst.setVisible(true);
		} catch (LoadException e) {
			JOptionPane.showMessageDialog(null, e.getMessage()+". Program will now exit.");
		}
	}

	
}
