//general structure of JSON output:
//each patient has:
  //name:
  //id:
  //organs:
    //file_name:
    //mesh:
//all share:
  //other:

//  import http.requests.*;
JSONObject json;
JSONArray patients, triangleJSON, tripleCoord, triplePixel;
JSONObject patient;
JSONArray JSONOrgans;
//////////////////////////////CREATES Json
String[] patientid = { "0001230", "0005932", "1043300" };
String[] name = { "Jack Neilson", "Arnold Johnson", "Homer Shwartz"} ; 
String[] filename = { "front.jpg", "salad2.jpg", "mussels.jpg" };
String[] organs = {"lung","heart","stomache"};
int[][][] triangles = {
  {
    {0, 0, 0},
    {0, 100, 0},
    {100, 0, 0},
  },
  {
    {100, 100, 0},
    {0, 100, 0},
    {100, 0, 0},
  },
  {
    {0, 0, 100},
    {0, 100, 100},
    {100, 0, 100},
  },
  {
    {100, 100, 100},
    {0, 100, 100},
    {100, 0, 100},
  },
  {
    {0, 0, 0},
    {0, 0, 100},
    {0, 100, 100}
  },
  {
    {0, 0, 0},
    {0, 100, 0},
    {0, 100, 100}
  },
  {
    {100, 0, 0},
    {100, 0, 100},
    {100, 100, 100}
  },
  {
    {100, 0, 0},
    {100, 100, 0},
    {100, 100, 100}
  },
  {
    {0, 100, 0},
    {100, 100, 0},
    {0, 100, 100}
  },
  {
    {0, 100, 100},
    {100, 100, 0},
    {100, 100, 100}
  },
  {
    {0, 0, 0},
    {100, 0, 0},
    {0, 0, 100}
  },
  {
    {0, 0, 100},
    {100, 0, 0},
    {100, 0, 100}
  }
};

JSONObject file = new JSONObject();
patients = new JSONArray();

//iterate through patients
for (int i = 0; i < patientid.length; i++) 
{

  patient = new JSONObject();
  JSONOrgans = new JSONArray();
  
  //iterate through the organs
  for(int organCounter = 0; organCounter < organs.length; organCounter++)
  {
    JSONObject JSONOrgan = new JSONObject();
    // This takes each value for each coordinate for each triangle and stores it in JSONArray
    triangleJSON = new JSONArray();
    for(int tri = 0; tri<triangles.length; tri++)
    {
        tripleCoord = new JSONArray();
        //print("tri: " + tri);
        for (int coord = 0; coord < triangles[tri].length; coord++)
        {
          triplePixel = new JSONArray();   
          //print("coord: "+coord);
          for (int value = 0; value < triangles[tri][coord].length; value++)
          {
            
            //print("value: "+value);
            //print(" ->"+ triangles[tri][coord][value]+"\n");
            triplePixel.setInt(value, triangles[tri][coord][value]);
          }
          tripleCoord.setJSONArray(coord, triplePixel);
        }
        
        triangleJSON.setJSONArray(tri, tripleCoord);
    }
    JSONOrgan.setString("organ_name", organs[organCounter]);
    JSONOrgan.setString("file_name", filename[i]);
    JSONOrgan.setJSONArray("mesh", triangleJSON);
    JSONOrgans.setJSONObject(organCounter, JSONOrgan);
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

