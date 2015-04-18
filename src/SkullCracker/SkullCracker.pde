//Import necessary libraries
import android.view.MotionEvent;
import android.content.res.AssetManager;
import android.media.SoundPool;
import android.media.AudioManager;
import apwidgets.*;
import android.text.InputType;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.content.Context;
import ketai.ui.*;
import java.io.*;
JSONObject json;
JSONArray patients;
String urlprefix = "http://alaromana.com/images/";
PImage img;
APWidgetContainer homeWidget, settingsWidget, clientsWidget, imageViewer, annotateView;
APButton update, help, viewpatients, settings, back_iv, back_cw, back_sw, back_av, annotate, save;
APEditText annotation;
ArrayList<PatientData> patientList = new ArrayList<PatientData>();
int currentPatient;
String view;
boolean loading = false;

void setup()
{
  imageMode(CENTER);
  orientation(PORTRAIT);
//  fetchJSON();
  loadPatients();
  initializeWidgets();
  print(width + " " + height);
}

void draw() 
{
  background(0);
  if (view == "image")
  {
    textSize(100);

    pushMatrix();
    translate(width/2, height/2);
    rotateX(xr);
    rotateY(yr);
    box.draw(cap);
    cap.draw();
    popMatrix();
    text(patientList.get(currentPatient).getpName(), 200, 150);
  }
  if (loading)
  {
    textSize(100);
    text("Loading Patient Data...", 0, height/2-200);
  }
  if (view == "annotate")
  {
      
//     String imageloc = urlprefix + patientList.get(currentPatient).getfilename();

     img = loadImage(patientList.get(currentPatient).getfilename());
     image(img,width/2,height/2);
  }
}
void onRotate(float x, float y, float ang)
{
  print(ang);
}
void onPinch(float x, float y, float d)
{
//  wSize = constrain(wSize+d, 10, 2000);
//  hSize = constrain(wSize+d, 10, 2000);
//  if (prevPinchX >= 0 && prevPinchY >= 0 && (frameCount - prevPinchFrame < 10)) 
//  {
//    translateX += (x - prevPinchX);
//    translateY += (y - prevPinchY);
//  }
//  prevPinchX = x;
//  prevPinchY = y;
//  prevPinchFrame = frameCount;
  println("Pinch " + x + " " + y + " " + d);
}

//set active view
void widgetOverlay()
{
    if (view == "home")
    {
      homeWidget.show();
      settingsWidget.hide(); 
      clientsWidget.hide();
      imageViewer.hide(); 
      annotateView.hide(); 
    }
    else if (view == "settings")
    {
      homeWidget.hide();
      settingsWidget.show(); 
      clientsWidget.hide();
      imageViewer.hide(); 
      annotateView.hide(); 
    }
    else if (view == "patients")
    {
      homeWidget.hide();
      settingsWidget.hide(); 
      clientsWidget.show();
      imageViewer.hide(); 
      annotateView.hide(); 
    }
    else if (view == "image")
    {
      homeWidget.hide();
      settingsWidget.hide(); 
      clientsWidget.hide();
      imageViewer.show(); 
      annotateView.hide(); 
      size(1080, 1776, P3D);
      ortho();
      box = new Boxxen();
//      box.loadTriangles();////////////////////////////////////////////////////////
      cap = new CutawayPlane(10, 0);
    }
    else if (view == "annotate")
    {
      homeWidget.hide();
      settingsWidget.hide(); 
      clientsWidget.hide();
      imageViewer.hide(); 
      annotateView.show(); 
    }
}

//initialize widgets and containers
void initializeWidgets()
{
    //add widget containers
    homeWidget = new APWidgetContainer(this); 
    settingsWidget = new APWidgetContainer(this); 
    clientsWidget = new APWidgetContainer(this); 
    imageViewer = new APWidgetContainer(this); 
    annotateView = new APWidgetContainer(this); 
    
    //draw widgets
    update = new APButton(width/2, (height/2)-100 ,"Update");
    help = new APButton(width/2, (height/2)+100, "Help");
    viewpatients = new APButton(width/2, (height/2)-100 , "View Patients");
    settings  = new APButton(width/2, (height/2)+100, "Settings");
    back_sw = new APButton(0, 50, "back");
    back_cw = new APButton(0, 50, "back");
    back_iv = new APButton(0, 50, "back");
    back_av = new APButton(0, 50, "back");
    annotate = new APButton(width-250, 50, "annotate");
    save = new APButton(width-250, 50, "save");
    annotation = new APEditText(150, 50, (width*2/3)-50, 150 );  
    addPatientWidgets();
    
    //initialize widgets on panel
    homeWidget.addWidget(viewpatients);
    homeWidget.addWidget(settings);
    settingsWidget.addWidget(back_sw); 
    settingsWidget.addWidget(help); 
    settingsWidget.addWidget(update);
    clientsWidget.addWidget(back_cw);
    imageViewer.addWidget(back_iv);
    imageViewer.addWidget(annotate);
    annotateView.addWidget(annotation);
    annotateView.addWidget(back_av);
    annotateView.addWidget(save);
    
    view = "home";
    widgetOverlay();
}

//fetch JSON from web and load clientlist
void fetchJSON()
{
    loading = true;
    String patientspath = urlprefix+"patients.txt";
    String[] homePage = null;
    homePage = loadStrings(patientspath);

    StringBuilder builder = new StringBuilder();
    for(String row : homePage) 
    {
        builder.append(row);
    }
    String json_Str = builder.toString();
    saveJSON(json_Str);
    patientList.clear();
    fillPatientList(json_Str);
    //download images
    for(PatientData patient : patientList)
    {
       patient.downloadImg(); 
    }
    loading = false;
}

//populate patient objects
void fillPatientList(String json_Str)
{
    JSONObject json = JSONObject.parse(json_Str);
    String other = json.getString("other");
    patients = json.getJSONArray("patients");
    for (int i = 0; i < patients.size(); i++) 
    {
      JSONObject patient = patients.getJSONObject(i);
      String id = patient.getString("id");
      String filename = patient.getString("file_name");
      String patientname = patient.getString("name");
      PatientData pd = new PatientData(filename, id, patientname);
      patientList.add(pd);
    } 
}

//read patients from device
void loadPatients()
{
    File sketchDir = getFilesDir();
    // read strings from file into tags
    try 
    {
      FileReader input = new FileReader(sketchDir.getAbsolutePath() + "/" + "patientInfo" + ".txt");
      BufferedReader bInput = new BufferedReader(input);
      String ns = bInput.readLine();
      StringBuilder builder = new StringBuilder();
      while (ns != null) 
      {
        builder.append(ns);
        ns = bInput.readLine();
      }
      String json_Str = builder.toString();
      fillPatientList(json_Str);
    }
    catch (Exception e) 
    {
      fetchJSON();
    }
}

//write JSON to device
void saveJSON(String json)
{
    File sketchDir = getFilesDir();
    java.io.File outFile;
    try 
    {
      outFile = new java.io.File(sketchDir.getAbsolutePath() + "/"+"patientInfo"+".txt");
      if (!outFile.exists())
        outFile.createNewFile();
      FileWriter outWriter = new FileWriter(sketchDir.getAbsolutePath() + "/"+"patientInfo"+".txt");

      outWriter.write(json);
      outWriter.flush();
    }
    catch (Exception e) 
    {
    }
}

//upon updating client information, removing old data
void clearPatientFiles()
{
    File sketchDir = getFilesDir();
    java.io.File outFile;
    try 
    {
      outFile = new java.io.File(sketchDir.getAbsolutePath() + "/"+"patientInfo"+".txt");
      outFile.delete();
      removePatientWidgets();
      fetchJSON();
      addPatientWidgets(); 
      widgetOverlay();
    }
    catch (Exception e) 
    {
    }
}

//adding buttons for the currently loaded patients
void addPatientWidgets()
{
    for(int i = 0; i < patientList.size(); i++)
    {
      patientList.get(i).placeButton(i);
      clientsWidget.addWidget(patientList.get(i).getButton());
    } 
}

// remove patient widgets when resyncing
void removePatientWidgets()
{
    for(int i = 0; i < patientList.size(); i++)
    {
      clientsWidget.removeWidget(patientList.get(i).getButton());
    } 
}

//track what widget is clicked on
void onClickWidget(APWidget widget)
{  
  //if it was save that was clicked
  if(widget == update)
  { 
     thread("clearPatientFiles");
  }
  //if it was cancel that was clicked
  else if(widget == help)
  { 
    print("help");
  }
  else if(widget == viewpatients)
  {
    view = "patients";
  }
  else if(widget == settings)
  {
    view = "settings";
  }
  else if(widget == back_iv)
  {
    view = "patients";
  }
  else if(widget == back_cw)
  {
    view = "home";
  }
  else if(widget == back_sw)
  {
    view = "home";
  }
  else if(widget == back_av)
  {
    view = "image";
  }
  else if(widget == annotate)
  {
    view = "annotate";
  } 
  for (int i = 0; i < patientList.size(); i++)
  {
     if (widget == patientList.get(i).getButton())
     {
         view = "image";
         currentPatient = i;
     }
  }
  widgetOverlay();
}

