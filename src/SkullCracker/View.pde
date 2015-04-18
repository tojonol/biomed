public class PatientData
{
  private String filename, id, name;
  APButton button;
   public PatientData(String filename_, String id_, String name_)
  {
     filename = filename_;
     id = id_;
     name = name_;
  }
 public void placeButton(int offset)
{
    String buttonlabel = id+ ": "+name;
    button = new APButton(200, 200+(offset*200), buttonlabel); 
}
 public APButton getButton()
 {
    return button; 
 }
 public String getId()
 {
    return id; 
 }
 public String getpName()
 {
    return name; 
 }
 public String getfilename()
 {
    return filename; 
 }
}
