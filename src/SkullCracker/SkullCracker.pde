//Import necessary libraries
import android.view.MotionEvent;
import apwidgets.*;
import ketai.ui.*;
import java.io.*;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.content.Context;

JSONObject json;
JSONArray patients;
PImage img;
APWidgetContainer homeWidget, settingsWidget, clientsWidget, imageViewer, annotateView, helpView;
APButton update, help, viewpatients, settings, back_iv, back_cw, back_sw, back_av, back_settings, annotate, save;
APButton cutOutObjectS, cutOutObjectL, cutInObjectS, cutInObjectL;
APEditText annotation;
ArrayList<PatientData> patientList = new ArrayList<PatientData>();
int prevPinchFrame = -1, sliceIndex, currentPatient, organSet, cutaxis;
String urlprefix = "http://alaromana.com/images/", view;
boolean loading = false;
Boxxen box;
CutawayPlane cap;
KetaiGesture gesture;
float prevPinchX = -1, prevPinchY = -1, scaleRatio = 1, modX, modY, maxMesh, minMesh;
float scale[] = {0.98, 0.98, 2.4};

//general program setup
void setup()
{
  textAlign(CENTER);
  imageMode(CENTER);
  orientation(PORTRAIT);
  gesture = new KetaiGesture(this);
  String infile = "/"+"patientInfo"+".txt";
  loadPatients(infile);
  initializeWidgets();
  
  ortho();
  cap = new CutawayPlane(10, 0);
  cap.setCutDim(2);  
  box = new Boxxen();
  sliceIndex = 0;
  modX = width/2;
  modY = height/2;
}

public String sketchRenderer() 
{
  return P3D; 
}

//main draw method
void draw() 
{
  background(0);
  int[] currOrganOffset = new int[3];  
  
  //Help view
  if(view == "help")
  {
     String helpmsg = "\n\nWhen viewing images:\n\tDrag to rotate\n\tPinch to zoom\n\tUse arrow buttons to slice object\n\nWhen Annotating images:\n\tDrag to move object\n\tPinch to zoom\n\tClick label to view region";
     PFont f =  createFont("Arial",20,true);
     textFont(f);         
     textSize(60);
     text(helpmsg,width/2,200);
  }
  
  //ImageViewer Mode
  else if (view == "image")
  {
    currOrganOffset = patientList.get(currentPatient).getOrganData(organSet).organOffset;
    println(currOrganOffset[0] + "->" +currOrganOffset[1] + "->" +currOrganOffset[2]  );
    
    //Check radio button and set active organ
    if (patientList.get(currentPatient).getActiveOrgan() != organSet)
    {
        organSet =  patientList.get(currentPatient).getActiveOrgan();
        box.update(patientList.get(currentPatient).getOrganMesh(organSet));        
    }
    
    //Draw image information
    pointLight(100, 100, 100, 200, 200, 200);
    pointLight(100, 100, 100, -200, -200, -200);
    ambientLight(120, 120, 120);
    textSize(50);
    pushMatrix();
    translate(width/2, height/2);
    rotateX(xr);
    rotateY(yr);
    scale(scaleRatio);
    noStroke();
    box.draw(cap);
    cap.draw();
    popMatrix();
    text(patientList.get(currentPatient).getpName(), width/2, 150);
    img = patientList.get(currentPatient).getActiveOrganImage(sliceIndex);
  }
  
  //Loading patient data, notify user
  if (loading)
  {
    textSize(100);
    text("Loading Patient Data...", width/2, height/2-200);
  }
  
  //Annotate mode
  if (view == "annotate")
  {
    currOrganOffset = patientList.get(currentPatient).getOrganData(organSet).organOffset;
    println(currOrganOffset[0] + "->" +currOrganOffset[1] + "->" +currOrganOffset[2]  );
    textSize(50);
    pushMatrix();
    translate(modX, modY);

    //adjust for facing camera
    rotateX(0);
    rotateY(PI);
    scale(scaleRatio);
    box.draw(cap);
    cap.draw();
    popMatrix();
    text(patientList.get(currentPatient).getpName(), width/2, 150);
    OrganData currOrgan = patientList.get(currentPatient).getOrganData(organSet); 
  }
  
  //display the active overlay
  widgetOverlay();
}

//track mouse dragged
void mouseDragged() 
{
  //if in imageview, rotate object
  if(view == "image")
  {
    float rate = 0.01;
    xr += (pmouseY-mouseY) * rate;
    yr += (mouseX-pmouseX) * rate;
  }
  //if in annotate view, slide object
  else if(view == "annotate")
  {
    modY -= (pmouseY-mouseY);
    modX +=(mouseX-pmouseX);     
  }
}

//event listener for pinch
void onPinch(float x, float y, float d)
{
  //zoom on pinch for both imageview and annotate view
  if(view=="image" || view=="annotate")
  {  
    if (prevPinchX >= 0 && prevPinchY >= 0 && (frameCount - prevPinchFrame < 10)) 
    {
      if(d>0)
        scaleRatio += .1;
      else
        scaleRatio -= .1;
    }
    prevPinchX = x;
    prevPinchY = y;
    prevPinchFrame = frameCount;
    if (scaleRatio <= .1)
      scaleRatio = .1;
  }
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
    back_sw = new APButton(50, 50, "back");
    back_cw = new APButton(50, 50, "back");
    back_iv = new APButton(50, 50, "back");
    back_av = new APButton(50, 50, "back");
    back_settings = new APButton(50, 50, "back");
    annotate = new APButton(width-300, 50, "annotate");
    save = new APButton(width-200, 50, "save");
    annotation = new APEditText(50, 200, width-100, 150 );  
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
    String output = "/"+"patientInfo"+".txt";
    saveJSON(json_Str, output);
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
          JSONArray offsetJSON = organ.getJSONArray("offset");
          int[] offsetArray = {offsetJSON.getInt(0), offsetJSON.getInt(1), offsetJSON.getInt(2)}; 
          OrganData organObject = new OrganData(id, organ.getString("organ_name"), organ.getJSONArray("mesh"), organ.getJSONArray("files"), offsetArray);
          organList.add(organObject);
      }
      PatientData pd = new PatientData(id, patientname, organList);
      patientList.add(pd);
    } 
}

//read JSON into memory from device
void loadPatients(String infile)
{
    File sketchDir = getFilesDir();
    // read strings from file into tags
    try 
    {
      FileReader input = new FileReader(sketchDir.getAbsolutePath() + infile);
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
void saveJSON(String json, String outfile)
{
    File sketchDir = getFilesDir();
    java.io.File outFile;
    try 
    {
      outFile = new java.io.File(sketchDir.getAbsolutePath() + outfile);
      if (!outFile.exists())
        outFile.createNewFile();
      FileWriter outWriter = new FileWriter(sketchDir.getAbsolutePath() + outfile);

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
  //viewpatients clicked
  else if(widget == viewpatients)
  {
    view = "patients";
  }
  //settings was clicked
  else if(widget == settings)
  {
    view = "settings";
  }
  //goes back to clients list
  else if(widget == back_iv)
  {
    view = "patients";
    imageViewer.removeWidget(patientList.get(currentPatient).getOrganButtons());
  }
  //back from clients list
  else if(widget == back_cw)
  {
    view = "home";
  }
  //back from settings
  else if(widget == back_sw)
  {
    view = "home";
  }
  //back from annotate
  else if(widget == back_av)
  {
    OrganData currOrgan = patientList.get(currentPatient).getOrganData(organSet); 
    currOrgan.saveTags();
    ArrayList<ButtonElement> slicebuttons = currOrgan.getButtons();
    for(int i = 0; i<slicebuttons.size();i++)
    {
       annotateView.removeWidget(slicebuttons.get(i).button);
    }
    view = "image";
  }
  //switch to annotate view
  else if(widget == annotate)
  {
    OrganData currOrgan = patientList.get(currentPatient).getOrganData(organSet); 
    ArrayList<ButtonElement> slicebuttons = currOrgan.getButtons();
    for(int i = 0; i<slicebuttons.size();i++)
    {
       annotateView.addWidget(slicebuttons.get(i).button);
    }
    view = "annotate";
  } 
  //switch to help
  else if(widget == help)
  {
    view = "help"; 
  }
  //cut out of object by 1
  else if(widget == cutOutObjectS)
  {
    cap.cut(-1);
    sliceIndex = cap.location;
  }
  //cut out of object by 5
  else if(widget == cutOutObjectL)
  {
    cap.cut(-5);
    sliceIndex = cap.location;
  }
  //cut into object by 1
  else if(widget == cutInObjectS)
  {
    cap.cut(1);
    sliceIndex = cap.location;
  }
  //cut into object by 5
  else if(widget == cutInObjectL)
  {
    cap.cut(5);
    sliceIndex = cap.location;
  }
  //click back from help
  else if(widget == back_settings)
  {
    view = "settings";
  }
  //annotate save
  else if(widget == save)
  {
    hideVirtualKeyboard();
    OrganData currOrgan = patientList.get(currentPatient).getOrganData(organSet); 
    currOrgan.addTag(annotation.getText(), sliceIndex);
    //if its a new button just add it
    int sliceButtonLocation = currOrgan.slicebuttonpresent(sliceIndex);
    if (sliceButtonLocation==-1)
    {
      ButtonElement currButton = currOrgan.updateButton(sliceIndex);
      annotateView.addWidget(currButton.button);
    }
    else
    {
      annotateView.removeWidget(currOrgan.tagButtons.get(sliceButtonLocation).button);
      ButtonElement currButton = currOrgan.updateButton(sliceIndex);
      annotateView.addWidget(currButton.button);
    }    
    annotation.setText("");
  }
  //check if client was clicked
  for (int i = 0; i < patientList.size(); i++)
  {
     if (widget == patientList.get(i).getPatientButton())
     {
        currentPatient = i;  
        view = "image";
        imageViewer.addWidget(patientList.get(i).getOrganButtons());
        box.update(patientList.get(i).getOrganMesh(0));
        organSet = 0;
        img = patientList.get(currentPatient).getActiveOrganImage(sliceIndex);
        cap.lastCut=99999;
     }
  }
  //check which organ was clicked
  if (view == "annotate")
  {
       OrganData currOrgan = patientList.get(currentPatient).getOrganData(organSet); 

      ArrayList<ButtonElement> slicebuttons = currOrgan.getButtons();
      for(int i = 0; i<slicebuttons.size();i++)
      {
         if (widget == slicebuttons.get(i).button)
         {
             sliceIndex = slicebuttons.get(i).slice;
             cap.location = sliceIndex;
         }
      } 
  }
  widgetOverlay();
}

public boolean surfaceTouchEvent(MotionEvent event) 
{
  //call to keep mouseX, mouseY, etc updated
  super.surfaceTouchEvent(event);

  //forward event to class for processing
  return gesture.surfaceTouchEvent(event);
}


//helper method for hiding the keyboard
void hideVirtualKeyboard() 
{
  InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
  imm.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY, 0);
}
