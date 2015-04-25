import java.util.*;
import java.awt.*;
import java.awt.event.*;
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

class SeedsViewer {
  private static SeedsViewer instance;
  static int regionNo;

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

    Button addRegionButton = new Button("New Region");
    addRegionButton.addActionListener(new AddRegionListener(this));

    leftContainer.add(this.regionList);
    leftContainer.add(addRegionButton);
    rightContainer.add(this.pointsList);

    this.mainFrame.add(leftContainer);
    this.mainFrame.add(rightContainer);

    this.mainFrame.setVisible(true);  
    IJ.log("LOLLZ");
  }

  public static SeedsViewer getInstance() {
    if (instance == null)
      instance = new SeedsViewer();

    return instance;
  }

  public void addRegion() {
    String name = "Region-" + regionNo++;
    this.regionSeeds.put(name, new ArrayList<Point>());
    this.regionList.add(name);
  }
}

public class SeedPicker extends PlugInTool {

	public void mousePressed(ImagePlus imp, MouseEvent e) {
    Point point = new Point(e.getX(), e.getY(), imp.getCurrentSlice());

    IJ.log("click: " + point.toString());

    SeedsViewer.getInstance();
	}

	public void showOptionsDialog() {
		IJ.log("icon double-clicked");
	}
}


