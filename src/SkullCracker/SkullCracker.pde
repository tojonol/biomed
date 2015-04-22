//Import necessary libraries
import android.view.MotionEvent;
//import android.content.res.AssetManager;
//import android.media.SoundPool;
//import android.media.AudioManager;
import apwidgets.*;
//import android.text.InputType;
//import android.view.inputmethod.EditorInfo;
//import android.view.inputmethod.InputMethodManager;
//import android.content.Context;
import ketai.ui.*;
import java.io.*;
JSONObject json;
JSONArray patients;
String urlprefix = "http://alaromana.com/images/";
PImage img;
APWidgetContainer homeWidget, settingsWidget, clientsWidget, imageViewer, annotateView, helpView;
APButton update, help, viewpatients, settings, back_iv, back_cw, back_sw, back_av, back_settings, annotate, save;
APButton cutOutObjectS, cutOutObjectL, cutInObjectS, cutInObjectL;
APEditText annotation;
ArrayList<PatientData> patientList = new ArrayList<PatientData>();
int currentPatient, organSet, cutaxis;
String view;
boolean loading = false;
Boxxen box;
CutawayPlane cap;
KetaiGesture gesture;
float prevPinchX = -1, prevPinchY = -1;
int prevPinchFrame = -1;


void setup()
{
  textAlign(CENTER);
  imageMode(CENTER);
  orientation(PORTRAIT);
  gesture = new KetaiGesture(this);
  loadPatients();
  initializeWidgets();
  size(1080, 1776, P3D);
  ortho();
  cap = new CutawayPlane(10, 0);
  
  box = new Boxxen();
}

void draw() 
{
  background(0);
  if (view == "image")
  {
    if (patientList.get(currentPatient).getActiveOrgan() != organSet)
    {
        organSet =  patientList.get(currentPatient).getActiveOrgan();
        box.update(patientList.get(currentPatient).getOrgan(organSet));
    }
    else
    {
    }
    textSize(50);

    pushMatrix();
    translate(width/2, height/2);
    rotateX(xr);
    rotateY(yr);
    box.draw(cap);
    cap.draw();
    popMatrix();
    text(patientList.get(currentPatient).getpName(), width/2, 150);
  }
  if (loading)
  {
    textSize(100);
    text("Loading Patient Data...", width/2, height/2-200);
  }
  if (view == "annotate")
  {
    
    textSize(50);

    pushMatrix();
    translate(width/2, height/2);
    rotateX(xr);
    rotateY(yr);
    box.draw(cap);
    cap.draw();
    popMatrix();
    text(patientList.get(currentPatient).getpName(), width/2, 150);
//      print(patientList.get(currentPatient).getActiveOrgan());
//     img = loadImage(patientList.get(currentPatient).getActiveOrgan());
//     image(img,width/2,height/2);
  }
}
void onRotate(float x, float y, float d)
{
  if(view == "image")
  {
    if (prevPinchX >= 0 && prevPinchY >= 0 && (frameCount - prevPinchFrame < 10)) 
    {
      if (x-prevPinchX>=0)
      {
         xr -= .01;        
      }
      else
      {
         xr += .01; 
      }
      if(y - prevPinchY>=0)
      {
        yr += .01;
      }
      else
      {
        yr -= .01;
      }
    }
    prevPinchX = x;
    prevPinchY = y;
    prevPinchFrame = frameCount;
    print(d);
  }
}
//event listener for double tap
void onDoubleTap(float x, float y)
{
  if(view=="image")
  {
     print("DT!"); 
     cutaxis += 1;
     cutaxis = cutaxis%3;
     if(cutaxis == 0)cap.setCutDim(0); 
     else if (cutaxis == 1) cap.setCutDim(1);
     else if (cutaxis == 2) cap.setCutDim(2);
  }
}
//event listener for flick
void onFlick( float x, float y, float px, float py, float v)
{
  if(view =="image")
  {
//    if(x-px > 50)
//    {
//      cap.location -= 3;
//    }
//    else if (x-px < 50)
//    {
//      cap.location += 3;
//    }
  }
}
void onPinch(float x, float y, float d)
{
  if(view=="image")
  {  
//    if (prevPinchX >= 0 && prevPinchY >= 0 && (frameCount - prevPinchFrame < 10)) 
//    {
//      if (x-prevPinchX>=0)
//      {
//         xr -= .1;        
//      }
//      else
//      {
//         xr += .1; 
//      }
//      if(y - prevPinchY>=0)
//      {
//        yr += .1;
//      }
//      else
//      {
//        yr -= .1;
//      }
//    }
//    prevPinchX = x;
//    prevPinchY = y;
//    prevPinchFrame = frameCount;
//    println("Pinch " + x + " " + y + " " + d);
  }
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
      helpView.hide();
    }
    else if (view == "settings")
    {
      homeWidget.hide();
      settingsWidget.show(); 
      clientsWidget.hide();
      imageViewer.hide(); 
      annotateView.hide();  
      helpView.hide();
    }
    else if (view == "patients")
    {
      homeWidget.hide();
      settingsWidget.hide(); 
      clientsWidget.show();
      imageViewer.hide(); 
      annotateView.hide();  
      helpView.hide();
    }
    else if (view == "image")
    {
      homeWidget.hide();
      settingsWidget.hide(); 
      clientsWidget.hide();
      imageViewer.show(); 
      annotateView.hide(); 
      helpView.hide(); 
    }
    else if (view == "annotate")
    {
      homeWidget.hide();
      settingsWidget.hide(); 
      clientsWidget.hide();
      imageViewer.hide(); 
      annotateView.show(); 
      helpView.hide(); 
    }
    else if (view == "help")
    {
      homeWidget.hide();
      settingsWidget.hide(); 
      clientsWidget.hide();
      imageViewer.hide(); 
      annotateView.hide(); 
      helpView.show(); 
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
    helpView = new APWidgetContainer(this);
    
    //set up buttons
    update = new APButton(200, (height/2)-100, width-400, 200,"Update");
    help = new APButton(200, (height/2)+100, width-400, 200, "Help");
    viewpatients = new APButton(200, (height/2)-100, width-400, 200, "View Patients");
    settings  = new APButton(200, (height/2)+100, width-400, 200, "Settings");
    back_sw = new APButton(0, 50, "back");
    back_cw = new APButton(0, 50, "back");
    back_iv = new APButton(0, 50, "back");
    back_av = new APButton(0, 50, "back");
    back_settings = new APButton(0, 50, "back");
    annotate = new APButton(width-300, 50, "annotate");
    save = new APButton(width-250, 50, "save");
    annotation = new APEditText(50, 175, width-100, 150 );  
    cutOutObjectS = new APButton(300, height-200, 200, 150, "<");
    cutOutObjectL = new APButton(50, height-200, 200, 150, "<<<"); 
    cutInObjectL = new APButton(width - 250, height-200, 200, 150, ">>>");
    cutInObjectS = new APButton(width - 500, height-200, 200, 150, ">");
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
    imageViewer.addWidget(cutOutObjectS);
    imageViewer.addWidget(cutOutObjectL);
    imageViewer.addWidget(cutInObjectS);
    imageViewer.addWidget(cutInObjectL);
    annotateView.addWidget(annotation);
    annotateView.addWidget(back_av);
    annotateView.addWidget(save);
    helpView.addWidget(back_settings);
    
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
       patient.downloadImgs(); 
    }
    loading = false;
}

//populate patient objects
void fillPatientList(String json_Str)
{
    JSONObject json = JSONObject.parse(json_Str);
//    String other = json.getString("other");
    patients = json.getJSONArray("patients");
    for (int i = 0; i < patients.size(); i++) 
    {
      JSONObject patient = patients.getJSONObject(i);
      String id = patient.getString("id");
      String patientname = patient.getString("name");
      JSONArray JSONorgans = patient.getJSONArray("organs");
      ArrayList<OrganData> organList = new ArrayList<OrganData>();
      for(int j = 0; j< JSONorgans.size(); j++)
      {
          JSONObject organ = JSONorgans.getJSONObject(j);
          OrganData organObject = new OrganData(id, organ.getString("organ_name"),organ.getString("file_name"), organ.getJSONArray("mesh"));
          organList.add(organObject);
      }
      PatientData pd = new PatientData(id, patientname, organList);
      patientList.add(pd);
    } 
}

//read JSON into memory from device
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
      patientList.get(i).placePatientButton(i);
      clientsWidget.addWidget(patientList.get(i).getPatientButton());
    } 
    
}

// remove patient widgets when resyncing
void removePatientWidgets()
{
    for(int i = 0; i < patientList.size(); i++)
    {
      clientsWidget.removeWidget(patientList.get(i).getPatientButton());
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
    imageViewer.removeWidget(patientList.get(currentPatient).getRadioGroup());
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
  else if(widget == help)
  {
    view = "help"; 
  }
  else if(widget == cutOutObjectS)
  {
    cap.location -= 1;
  }
  else if(widget == cutOutObjectL)
  {
    cap.location -= 5;
  }
  else if(widget == cutInObjectS)
  {
    cap.location += 1;
  }
  else if(widget == cutInObjectL)
  {
    cap.location += 5;
  }
  else if(widget == back_settings)
  {
    view = "settings";
  }
  for (int i = 0; i < patientList.size(); i++)
  {
     if (widget == patientList.get(i).getPatientButton())
     {
         view = "image";
         currentPatient = i;  
         imageViewer.addWidget(patientList.get(i).getRadioGroup());
//         size(1080, 1776, P3D);
//         ortho();
  //      box.loadTriangles(patientList.get(currentPatient).getpName());////////////////////////////////////////////////////////
//         cap = new CutawayPlane(10, 0);
        box.update(patientList.get(i).getOrgan(0));
        organSet = 0;
     }
  }
  widgetOverlay();
}

public boolean surfaceTouchEvent(MotionEvent event) {

  //call to keep mouseX, mouseY, etc updated
  super.surfaceTouchEvent(event);

  //forward event to class for processing
  return gesture.surfaceTouchEvent(event);
}

