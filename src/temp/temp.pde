
//  import http.requests.*;
JSONObject json;
JSONArray patients, triangleJSON, tripleCoord, triplePixel;
JSONObject patient;
JSONArray JSONOrgans;
//////////////////////////////CREATES Json
String[] patientid = { "0001230", "0005932", "1043300", "0005132"};
String[] name = { "Jack Neilson", "Arnold Johnson", "Homer Shwartz", "Beth Constantine"} ; 


JSONObject file = new JSONObject();
patients = new JSONArray();

String path = "C:\\Users\\Jonathan Olson\\biomed\\src\\temp\\";
//iterate through patients
//for (int i = 0; i < patientid.length; i++) 
for (int i = 0; i < 1; i++) 
{

  patient = new JSONObject();
  JSONOrgans = new JSONArray();
  String availableorgans = path+"input\\"+patientid[i];
  File organFiles = new File (availableorgans);
  String [] organs = organFiles.list();

  //iterate through the organs
  for(int organCounter = 0; organCounter < organs.length; organCounter++)
  {
    JSONObject JSONOrgan = new JSONObject();
    // This takes each value for each coordinate for each triangle and stores it in JSONArray
    triangleJSON = new JSONArray();
    JSONArray files = new JSONArray();
    JSONArray organImages = new JSONArray();

    File dir = new File (path+"input");
    String currimg = path+"input\\"+patientid[i]+"\\" +organs[organCounter];
    File organimgs = new File (currimg);
    String [] organImageArray = organimgs.list();
    if (organImageArray!=null)
    {
      for (int img = 0; img< organImageArray.length; img++)
      {
          if(organImageArray[img].substring(organImageArray[img].length()-3,organImageArray[img].length()).equals("png"))
            files.setString(img,organImageArray[img]);
      }
    }
    JSONOrgan.setJSONArray("files", organImages);
    JSONOrgan.setString("organ_name", organs[organCounter]);
    JSONOrgan.setJSONArray("files", files);
    String organMeshDir = path+"input\\"+patientid[i]+"\\" +organs[organCounter]+"\\";
    JSONObject meshObj = null;
    try{
      String meshtemp = organMeshDir + "mesh.json";
      meshObj = loadJSONObject(meshtemp);
    triangleJSON = meshObj.getJSONArray("mesh");
    JSONArray offsetJSON = meshObj.getJSONArray("offset");
    JSONOrgan.setJSONArray("offset", offsetJSON);
    JSONOrgan.setJSONArray("mesh", triangleJSON);
    JSONOrgans.setJSONObject(organCounter, JSONOrgan);
    }
    catch (Exception e)
    {
      print("mesh not found for: "+organs[organCounter]);
    }
  }
  patient.setString("name", name[i]);
  patient.setString("id", patientid[i]);
  patient.setJSONArray("organs", JSONOrgans);
  patients.setJSONObject(i, patient);
  }

file.setString("other", "something useful to know about everyone");
file.setJSONArray("patients",patients);
String output = "data/patients.txt";
saveJSONObject(file, output);

