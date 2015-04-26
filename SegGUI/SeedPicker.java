import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.io.FileWriter;
import java.io.IOException;
import java.io.File;

import javax.swing.BoxLayout;

import ij.*;
import ij.process.*;
import ij.gui.*;
import ij.plugin.tool.PlugInTool;

class Point {
  int x, y, slice;

  Point(int x, int y, int slice) {
    this.x = x;
    this.y = y;
    this.slice = slice;
  }

  public String toString() {
    return String.format("(%d, %d, %d)", this.x, this.y, this.slice);
  }
}

class AddRegionListener implements ActionListener {
  SeedsViewer viewer;

  AddRegionListener(SeedsViewer viewer) {
    this.viewer = viewer;
  }

  public void actionPerformed(ActionEvent e) {     
    this.viewer.addRegion();
  }
}

class SaveSeedsListener implements ActionListener {
  SeedsViewer viewer;

  SaveSeedsListener(SeedsViewer viewer) {
    this.viewer = viewer;
  }

  public void actionPerformed(ActionEvent e) {     
    this.viewer.saveSeeds();
  }
}

class RegionSelectionChangeListener implements ItemListener {
  SeedsViewer viewer;

  RegionSelectionChangeListener(SeedsViewer viewer) {
    this.viewer = viewer; 
  }

  public void itemStateChanged(ItemEvent e) {
    viewer.regionSelectionChanged();
  }
}

class SeedsViewer {
  private static SeedsViewer instance;
  static int regionNo;

  public boolean reachable;

  Frame mainFrame;
  java.awt.List regionList;
  java.awt.List pointsList;
  HashMap<String, ArrayList<Point>> regionSeeds;

  private SeedsViewer() {
    this.regionSeeds = new HashMap<String, ArrayList<Point>>();

    this.mainFrame = new Frame("Region Seeds");
    this.mainFrame.setSize(500, 300);
    this.mainFrame.setLayout(new BoxLayout(this.mainFrame, BoxLayout.X_AXIS));

    Container leftContainer = new Container();
    Container rightContainer = new Container();
    leftContainer.setLayout(new BoxLayout(leftContainer, BoxLayout.Y_AXIS));
    rightContainer.setLayout(new BoxLayout(rightContainer, BoxLayout.Y_AXIS));

    this.regionList = new java.awt.List(1);
    this.pointsList = new java.awt.List(1);

    this.regionList.addItemListener(new RegionSelectionChangeListener(this));

    Button addRegionButton = new Button("New Region");
    addRegionButton.addActionListener(new AddRegionListener(this));
    addRegionButton.setPreferredSize(new Dimension(10, 10));

    Button saveSeedsButton = new Button("Save Seeds");
    saveSeedsButton.addActionListener(new SaveSeedsListener(this));
    saveSeedsButton.setPreferredSize(new Dimension(10, 10));

    leftContainer.add(this.regionList);
    leftContainer.add(addRegionButton);
    leftContainer.add(saveSeedsButton);
    rightContainer.add(this.pointsList);

    this.mainFrame.add(leftContainer);
    this.mainFrame.add(rightContainer);

    final Frame mframe = this.mainFrame;
    this.mainFrame.addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent we) {
        mframe.dispose();
        SeedsViewer.getInstance().reachable = false;
      }
    });

    this.mainFrame.setVisible(true);
    this.reachable = true;
  }

  public static SeedsViewer getInstance() {
    if (instance == null)
      instance = new SeedsViewer();

    return instance;
  }

  public void regionSelectionChanged() {
    this.pointsList.removeAll();

    String activeRegion = this.regionList.getSelectedItem();
    for (Point p : this.regionSeeds.get(activeRegion)) {
      this.pointsList.add(p.toString());
    }
  }

  public void saveSeeds() {
    FileDialog dialog = new FileDialog(this.mainFrame, "Save", FileDialog.SAVE);
    dialog.setMultipleMode(false);

    dialog.show();
    File saveFile = dialog.getFiles()[0];

    String ss = serializeSeeds(this.regionSeeds);

    FileWriter fw;
    try {
      fw = new FileWriter(saveFile);
      //IJ.log(saveFile);
      fw.write(ss, 0, ss.length());
      fw.close();
    } catch (IOException e) {
      IJ.log("Failed to open file for saving.");
    }
  }

  public static String serializeSeeds(HashMap<String, ArrayList<Point>> seeds) {
    String s = "";

    for (String name : seeds.keySet()) {
      ArrayList<Point> points = seeds.get(name);
      s += name + " ";

      for (Point p : points) {
        s += p.x + " " + p.y + " " + p.slice + " ";
      }

      s = s.substring(0, s.length()-1);
      s += "\n";
    }

    s = s.substring(0, s.length()-1);
    return s;
  }

  public void addRegion() {
    String name = "Region-" + regionNo++;
    this.regionSeeds.put(name, new ArrayList<Point>());
    this.regionList.add(name);
  }


  public void addPoint(Point point) {
    String activeRegion = this.regionList.getSelectedItem();
    ArrayList<Point> activeRegionSeeds = this.regionSeeds.get(activeRegion);

    activeRegionSeeds.add(point);
    this.pointsList.add(point.toString());
  }
}

public class SeedPicker extends PlugInTool {

	public void mousePressed(ImagePlus imp, MouseEvent e) {
    Point point = new Point(e.getX(), e.getY(), imp.getCurrentSlice());

    SeedsViewer viewer = SeedsViewer.getInstance();
    if (!viewer.reachable) {
      viewer.mainFrame.setVisible(true);
    }

    viewer.addPoint(point);
	}

	public void showOptionsDialog() {
		IJ.log("icon double-clicked");
	}
}


