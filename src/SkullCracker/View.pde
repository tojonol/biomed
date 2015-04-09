public class PatientData
{
  private String filename, id;
  APButton button;
   public PatientData(String filename_, String id_)
  {
     filename = filename_;
     id = id_;
  }
 public void placeButton(int offset)
{
    button = new APButton(width/2, 200+(offset*200), id); 
}
 public APButton getButton()
 {
    return button; 
 }
 public String getId()
 {
    return id; 
 }
}
