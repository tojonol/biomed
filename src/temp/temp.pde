import http.requests.*;
JSONObject json;
JSONArray patients, triangleJSON, tripleCoord, triplePixel;
//////////////////////////////CREATES Json
String[] patientid = { "0001230", "0005932", "1043300" };
String[] name = { "Jack Neilson", "Arnold Johnson", "Homer Shwartz"} ; 
String[] filename = { "front.jpg", "salad2.jpg", "mussels.jpg" };
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
for (int i = 0; i < patientid.length; i++) 
{

  JSONObject patient = new JSONObject();
  
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
  //
  
  patient.setString("name", name[i]);
  patient.setString("id", patientid[i]);
  patient.setString("file_name", filename[i]);
  patient.setJSONArray("mesh", triangleJSON);
  patients.setJSONObject(i, patient);
}
file.setString("other", "something useful to know about everyone");
file.setJSONArray("patients",patients);
saveJSONObject(file, "data/patients.txt");
/////////////////////////////////////////

//  String url = "http://alaromana.com/images/";
//  
//  String jsonpath = url+"patients.txt";
//  json = loadJSONObject(jsonpath);
////  patients = JSONArray(jsonpath);
//  patients = json.getJSONArray("patients");
//  JSONObject patient = patients.getJSONObject(0);
//  String filename = patient.getString("file_name");
//  print(patient.getString("id"));
//  print(url +filename);
//  String imgpath = url+filename;
////  image = loadImage(imgpath);
//
////  URL = "http://" + "api.ihackernews.com/page";
////  final JSONObject json = JSONObject.parse(loadStrings(URL)[0]);
////   
////  final String instanceID = json.getString("cachedOnUTC");
////  println(instanceID);
////  println();
////   
////  final JSONObject latestItem = json.getJSONArray("items").getJSONObject(0);
////  println(latestItem);
////  println();
////   
////  latestItem.setString("points", "100");
////  println(latestItem);
////}
////
////void draw() 
////{
////  background(0);
//////  image(image, 0, 0);
////}
////
////
//////GetRequest get = new GetRequest("http://alaromana.com/images/Dinner/ChxRavioli.jpg");
//////get.send();
//////println("Reponse Content: " + get.getContent());
//////println("Reponse Content-Length Header: " + get.getHeader("Content-Length"));
//////json = loadJSONObject("http://alaromana.com/images/patients.txt");
//////JSONObject goat = json.getJSONObject("tutorials");
//////
//////int id = goat.getInt("id");
//////String species = goat.getString("species");
////
//////println(id + ", " + species);

