//Organ Data Class
public class OrganData
{
  String urlprefix = "http://alaromana.com/images/", organName, filename, id;
  JSONArray mesh;
  APRadioButton radio;
  public OrganData(String id_, String organ_name, String f_name, JSONArray omesh)
  {
    id = id_;
    organName = organ_name;
    filename = f_name;
    mesh = omesh;
    radio = new APRadioButton(organName);
  } 
 
 //get the radio button for a given organ
 public APRadioButton getOrganButton()
 {
    return radio; 
 }
 
 //download the corresponding organ's image
 public void downloadImage()
 {
     String imageloc = urlprefix + filename;
     img = loadImage(imageloc);
     String objectfile =  "p"+id+filename;
     img.save(objectfile);
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
 public String getImagefile()
 {
     String objectfile = "p"+id+filename;
    return objectfile; 
 }
 
 //get organ's mesh
 public JSONArray getMesh()
 {
    return mesh; 
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
   public APRadioGroup getRadioGroup()
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
         organs.get(i).downloadImage(); 
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
   public JSONArray getOrgan(int index)
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
   public PImage getActiveOrganImage()
   {
      for(int i = 0; i<organs.size(); i++)
      {
          if (organs.get(i).isChecked())
          {
            return loadImage(organs.get(i).getImagefile());
          }
      }
      return null;
   }
}
