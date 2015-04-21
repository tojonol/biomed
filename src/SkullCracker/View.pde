public class OrganData
{
  String urlprefix = "http://alaromana.com/images/";
  String organName, filename, id;
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
 public APRadioButton getOrganButton()
 {
    return radio; 
 }
 public void downloadImage()
 {
     String imageloc = urlprefix + filename;
     print(imageloc);
     img = loadImage(imageloc);
     String objectfile =  "p"+id+filename;
     print("save"+objectfile);
     img.save(objectfile);
 }
 public boolean isChecked()
 {
    if(radio.isChecked())
     {
        return true;
     }
    return false; 
 }
 public String getImagefile()
 {
     String objectfile = "p"+id+filename;
    return objectfile; 
 }
}

public class PatientData
{
  private String filename, id, name;
  PImage img;
  APButton button;
  APRadioGroup radioGroup;
  ArrayList<OrganData> organs;
  
   public PatientData(String id_, String name_, ArrayList<OrganData> organList)
  {
     radioGroup = new APRadioGroup(width-300, 200);
     radioGroup.setOrientation(APRadioGroup.VERTICAL);
     id = id_;
     name = name_;
     organs = organList;
//     organs = new ArrayList<APRadioButton>();
     for (int i = 0; i < organList.size(); i++)
     {
//       APRadioButton radio = new APRadioButton(organList.get(i).getOrganName());
//       organs.add(radio); 
       radioGroup.addRadioButton(organList.get(i).getOrganButton());
     }
     organList.get(0).getOrganButton().setChecked(true);
  }
 public void placePatientButton(int offset)
{
    String buttonlabel = id+ ": "+name;
    button = new APButton(200, 200+(offset*200), buttonlabel); 
}
 public APRadioGroup getRadioGroup()
 {
    return radioGroup; 
 }
 public APButton getPatientButton()
 {
    return button; 
 }
 public void downloadImgs()
 {
    for(int i = 0; i< organs.size();i++)
    {
       organs.get(i).downloadImage(); 
    }
 }
 public String getId()
 {
    return id; 
 }
 public String getpName()
 {
    return name; 
 }
 public String getActiveOrgan()
 {
    for(int i = 0; i<organs.size(); i++)
    {
        if (organs.get(i).isChecked())
        {
          return organs.get(i).getImagefile();
        }
    }
    return null;
 }
}
