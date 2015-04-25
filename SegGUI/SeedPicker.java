// Prototype plugin tool. There are more plugin tools at
// http://imagej.nih.gov/ij/plugins/index.html#tools
import ij.*;
import ij.process.*;
import ij.gui.*;
import java.awt.*;
import ij.plugin.tool.PlugInTool;
import java.awt.event.*;

public class SeedPicker extends PlugInTool {

	public void mousePressed(ImagePlus imp, MouseEvent e) {

    IJ.log(String.format("click: (%d, %d, %d)", e.getX(), e.getY(), imp.getCurrentSlice()));
	}

	public void mouseDragged(ImagePlus imp, MouseEvent e) {
		IJ.log("mouse dragged: "+e);
	}

	public void showOptionsDialog() {
		IJ.log("icon double-clicked");
	}
}
