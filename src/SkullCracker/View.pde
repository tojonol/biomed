public class ButtonElement
{
  OrganTag tag;
  APButton button;
  int offset;
   
  public ButtonElement(APButton button, int offset, OrganTag tag)
  {
    this.button = button;
    this.offset = offset;
    this.tag = tag;
  }  
}

import java.util.*;
public class OrganTag
{
  int slice, radius;
  int[] location;
  String tag;
  public OrganTag(String tag, int imageSlice, int[] location, int radius)
  {
    this.tag = tag;
    slice = imageSlice;
    this.location = location;
    this.radius = radius;
  } 
}

//Organ Data Class
public class OrganData
{
  String urlprefix = "http://alaromana.com/images/", organName, filename, id, tagfile;
  ArrayList<OrganTag> tags;
  ArrayList<String> imgList;
  ArrayList<ButtonElement> tagButtons;
  int[] organOffset;
  Set tagsliceset;
  JSONArray mesh;
  APRadioButton radio;
  public OrganData(String id_, String organ_name, JSONArray omesh, JSONArray files, int[] organOffset)
  {
    tags = new ArrayList<OrganTag>();
    imgList = new ArrayList<String>();
    tagButtons = new ArrayList<ButtonElement>();
    id = id_;
    organName = organ_name;
    mesh = omesh;
    radio = new APRadioButton(organName);
    tagfile = "p"+id+organName + ".txt";
    loadTags();
    placeTagButtons();
    this.organOffset = organOffset;
    for(int i = 0; i<files.size();i++)
    {
      String curr = files.getString(i);
      imgList.add(curr);
    }
  } 
 
  public ArrayList<ButtonElement> getButtons()
  {
     return tagButtons;
  }
  
  public void placeTagButtons()
  {
    for (OrganTag tag : this.tags)
    {
      addTagButton(tag);
    }
  }
  
  ButtonElement addTagButton(OrganTag tag)
  {
    int offset = this.tagButtons.size();
    APButton button = new APButton(width-400, 400+(offset*150), 400, 150, tag.tag);
    ButtonElement be = new ButtonElement(button, offset, tag);
    tagButtons.add(be);
    
    return be;
  }
  
 //get the radio button for a given organ
 public APRadioButton getOrganButton()
 {
    return radio; 
 }
 
 public void downloadAllImages()
 {
   try
   {
      for (int i = 0; i< imgList.size();i++)
      {
         String imageloc = urlprefix+"patients/"+id+"/"+organName+"/"+imgList.get(i);
         PImage currimg = loadImage(imageloc);
         String objectfile =  "p"+id+organName +imgList.get(i);
         print("Saving file: "+ objectfile);
         currimg.save(objectfile);
       }     
   }
   catch (Exception e) 
   {
      print("error dlin");
   }
 }
 
 //check if the given organ is checked
 public boolean isChecked()
 {
    if(radio.isChecked())
     {
        return true;
     }
    return false; 
 }
 
 //get organ's file
 public String getImagefile(int index)
 {
    if (index>=imgList.size())
      index = imgList.size()-1;
    else if (index < 0)
      index = 0;
    String objectfile = "p"+id+organName +imgList.get(index);
    return objectfile; 
 }
 
 //get organ's mesh
 public JSONArray getMesh()
 {
    return mesh; 
 }
 
 //load organ's tags from file
 public void loadTags()
 {
    File sketchDir = getFilesDir();
  
    try 
    {
      FileReader input = new FileReader(sketchDir.getAbsolutePath() + "/" + tagfile);
      BufferedReader bInput = new BufferedReader(input);
      String ns = bInput.readLine();
      StringBuilder builder = new StringBuilder();
      while (ns != null) 
      {
        builder.append(ns);
        ns = bInput.readLine();
      }
      String json_Str = builder.toString();
      JSONArray JSONtags = JSONArray.parse(json_Str);
//      print(JSONtags.size() + " tags read from file\n");
      for (int i = 0; i < JSONtags.size(); i++) 
      {
        JSONObject currtag = JSONtags.getJSONObject(i);
        String temptag = currtag.getString("tag");
        String tempslice = currtag.getString("slice");
        //addTag(temptag, Integer.parseInt(tempslice));
      } 
    }
    catch (Exception e) 
    {
    }
 }
 
 //save organs tags to file
  public void saveTags()
  { 
    if (tags.size()>0)
    {
      File sketchDir = getFilesDir();
      java.io.File outFile;
      try 
      {
        outFile = new java.io.File(sketchDir.getAbsolutePath() + "/"+tagfile);
        if (!outFile.exists())
          outFile.createNewFile();
        FileWriter outWriter = new FileWriter(sketchDir.getAbsolutePath() + "/"+tagfile);
        outWriter.write("[\n");
//        print(tags.size());
        for (int i=0; i<tags.size (); i++) 
        {
          outWriter.write("{ \"slice\": \"" + tags.get(i).slice + "\", \"tag\": \""+ tags.get(i).tag + "\" },");
        }
        outWriter.write("\n]");
        
        outWriter.flush();
        
      }
      catch (Exception e) 
      {
      }
    }
  }
   
  public void addTag(String tag, int sliceIndex, int[] location, int radius)
  {
      OrganTag organTag = new OrganTag(tag, sliceIndex, location, radius);
      tags.add(organTag);
      ButtonElement be = this.addTagButton(organTag);
      annotateView.addWidget(be.button);
  }
}

//data for each patient
public class PatientData
{
  private String id, name;
  PImage img;
  APButton button;
  APRadioGroup radioGroup;
  ArrayList<OrganData> organs;
  
   public PatientData(String id_, String name_, ArrayList<OrganData> organList)
  {
     radioGroup = new APRadioGroup(width-400, 200);
     radioGroup.setOrientation(APRadioGroup.VERTICAL);
     id = id_;
     name = name_;
     organs = organList;
     for (int i = 0; i < organList.size(); i++)
     { 
       radioGroup.addRadioButton(organList.get(i).getOrganButton());
     }
     organList.get(0).getOrganButton().setChecked(true);
  }
  
  //place the button representing a patient
  public void placePatientButton(int offset)
  {
      String buttonlabel = id+ ": "+name;
      button = new APButton(200, 300+(offset*200), width-400, 200, buttonlabel); 
   }
   
   //get the radio buttons for a patient
   public APRadioGroup getOrganButtons()
   {
      return radioGroup; 
   }
   
   //return the buttom for the patient
   public APButton getPatientButton()
   {
      return button; 
   }
   
   //download each organs image
   public void downloadImgs()
   {
      for(int i = 0; i< organs.size();i++)
      {
         organs.get(i).downloadAllImages(); 
      }
   }
   
   //get patients id
   public String getId()
   {
      return id; 
   }
   
   //get patient name
   public String getpName()
   {
      return name; 
   }
   
   //get the organs index
   public int getActiveOrgan()
   {
      for(int i = 0; i<organs.size(); i++)
      {
          if (organs.get(i).isChecked())
          {
            return i;
          }
      }
      return 0;
   }
   
   //get the organs mesh
   public JSONArray getOrganMesh(int index)
   {
     if (index<organs.size())
     {
         return organs.get(index).getMesh();
     }
     else
     {
       print ("outof bounds!");
       return null;
     }
   }
   
   //return the active organs image
   public PImage getActiveOrganImage(int index)
   {
      for(int i = 0; i<organs.size(); i++)
      {
          if (organs.get(i).isChecked())
          {
            
            return loadImage(organs.get(i).getImagefile(index));
          }
      }
      return null;
   }
   
   public OrganData getOrganData(int index)
   {
      return organs.get(index);
   }
}
