//import apwidgets.*;

float yr = PI / 4;
float xr = PI /4;

int[][][] fromStrip(int[][] strip) 
{
  int[][][] triangles = new int[strip.length - 2][3][3];
  
  for (int i=2; i<strip.length; i++) 
  {
    int[][] threeTriangles = {strip[i-2], strip[i-1], strip[i]};
    triangles[i-2] = threeTriangles;
  }  
  return triangles;
}

class CutawayPlane 
{
  int DIM_X = 0, DIM_Y = 1, DIM_Z = 2;
  
  int location;
  int keepDirection = 1;
  
  int lastCut = -9999, lastOrganIdx = -1;
  PShape lastCutResult;
  
  // This could explode in a really bad way if anyone mutates it down the line,
  // let's really hope that doesn't happen
  int[][] nullArr = {null};
  
  // Note that `~0` mean the int with every bit set
  private int cutDim;  
  
  CutawayPlane(int location, int cutDim) 
  {
    this.location = location;  
    this.setCutDim(cutDim);
  }
  
  public void invalidateCache()
  {
    this.lastCut = -9999;
  }
  
  public void setCutDim(int cutDim) 
  {
    this.cutDim = cutDim;
  }
  
  public void cut(int change)
  {
    if(change>0 && location+change<= maxMesh+1)
    {
      if(change>0 && location<minMesh-1)
      {
         location = round(minMesh-1);
      }
      else
      {
        location = location+change;
      }
    }
    if(change<0 && location-change>= minMesh-1)
    {
      if(change<0 && location>maxMesh+1)
      {
         location = round(maxMesh+1); 
      }
      else
      {
        location = location+change;
      }
    }  
  }
  
  void draw(int[] offset)
  {
    fill(153);
    pushMatrix();
    translate(0 - offset[0], 0 - offset[1], this.location);
    if (this.cutDim == DIM_X) rotateY(HALF_PI);
    else if (this.cutDim == DIM_Y) rotateX(-HALF_PI);
    
    beginShape(QUADS);
    OrganData currOrgan = patientList.get(currentPatient).getOrganData(organSet)
    int sliceIndex = (int)((this.location / scale[2]) + currOrgan.offset[2]);
    texture(patientList.get(currentPatient).getActiveOrganImage(sliceIndex););
    vertex(0, 0,  0, 0);
    vertex(img.width * scale[0], 0,  img.width, 0);
    vertex(img.width * scale[0], img.height * scale[1],  img.width, img.height);
    vertex(0, img.height * scale[1],  0, img.height);
    endShape();

    popMatrix();
  }
  
  PShape cutPolies(int[][][] triangles)
  {
    int activeOrganIdx = patientList.get(currentPatient).getActiveOrgan();
    if (this.location == this.lastCut && activeOrganIdx == this.lastOrganIdx)
    {
      return this.lastCutResult;
    }
    
    PShape cutMesh = createShape();
    cutMesh.beginShape(TRIANGLES);
    
    for (int[][] triangle : triangles)
    {
      int[][] cutTri = this.cutPoly(triangle);
      
      if (cutTri[0] == null) {
        continue;
      }
      else if (cutTri.length == 4)
      {
        cutMesh.vertex(cutTri[0][0], cutTri[0][1], cutTri[0][2]);
        cutMesh.vertex(cutTri[1][0], cutTri[1][1], cutTri[1][2]);
        cutMesh.vertex(cutTri[2][0], cutTri[2][1], cutTri[2][2]);
        
        cutMesh.vertex(cutTri[1][0], cutTri[1][1], cutTri[1][2]);
        cutMesh.vertex(cutTri[2][0], cutTri[2][1], cutTri[2][2]);
        cutMesh.vertex(cutTri[3][0], cutTri[3][1], cutTri[3][2]);
      }
      else
      {
        cutMesh.vertex(cutTri[0][0], cutTri[0][1], cutTri[0][2]);
        cutMesh.vertex(cutTri[1][0], cutTri[1][1], cutTri[1][2]);
        cutMesh.vertex(cutTri[2][0], cutTri[2][1], cutTri[2][2]);
      }
    }
    
    cutMesh.endShape(CLOSE);
    
    this.lastCut = this.location;
    this.lastOrganIdx = activeOrganIdx;
    this.lastCutResult = cutMesh;
    
    return cutMesh;
  }
  
  float distToPoint(int[] point) 
  {
    return abs(this.location - point[this.cutDim]);
  }
    
  int pointsThisSide(int[][] poly) 
  {
    int i = 0;
    
    for (int k=0; k<poly.length; k++) 
    {
      // TODO: Care about keep direction
      if (poly[k][this.cutDim] > this.location) 
      {
        i++;
      }
    }
    return i;
  }
  
  int[] interpolate(int[] p1, int[] p2) 
  {
    float totes = dist(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]),
      l = this.distToPoint(p2) / (p2[2] - p1[2]),
      sx = (p2[0] - p1[0]),
      sy = (p2[1] - p1[1]),
      sz = (p2[2] - p1[2]);
      
    int[] intersectionPoint = {
      p2[0] + (int)(sx * l),
      p2[1] + (int)(sy * l),
      this.location//p2[2] + (int)(sz * d)
    };
   return intersectionPoint;
  }
  
  int[][] cutPoly(int[][] poly) 
  {
    int pts = this.pointsThisSide(poly);
    if (pts == 3) 
    {
      return poly;
    } 
    else if (pts == 0) 
    {
      return this.nullArr;
    } 
    else if (pts == 2) 
    {
      int didx = -1; // drop index
      
      while (true) 
      {
        if (poly[++didx][this.cutDim] <= this.location) 
          break;
      }
      
      int kidx1 = (didx + 1) % 3, // keep index
        kidx2 = (didx + 2) % 3;
        
      int[][] quad = {
        poly[kidx1],
        poly[kidx2],
        this.interpolate(poly[kidx2], poly[didx]),
        this.interpolate(poly[kidx1], poly[didx]),
      };
      return quad;      

    } 
    else if (pts == 1) 
    {
      int kidx = -1;  
      while (true) 
      {
        if (poly[++kidx][this.cutDim] > this.location) 
          break;
      }
      
      int didx1 = (kidx + 1) % 3,
        didx2 = (kidx + 2) % 3;
        
      int[][] tri = {
        poly[kidx],
        this.interpolate(poly[kidx], poly[didx1]),
        this.interpolate(poly[kidx], poly[didx2])
      };
      return tri;
    }    
    
    return this.nullArr;
  }
}

class Boxxen 
{
  //give it something to draw before image loads into memory
  int[][][] triangles = {
    {
      {0, 0, 0},
      {0, 100, 0},
      {100, 0, 0},
    }};
    public int[] offset;
  
  void drawPoly(int[][] poly) 
  {
    if (poly.length == 0 || poly[0] == null) return;
    beginShape();
    for (int i=0; i<poly.length; i++) 
    {
      if (poly[i] == null) 
        break;
      
      vertex(poly[i][0], poly[i][1], poly[i][2]);
    }
    vertex(poly[0][0], poly[0][1], poly[0][2]);
    endShape();
  }
  
  void draw(CutawayPlane cap) 
  {
    PShape polies = cap.cutPolies(triangles);
    
    fill(255);
    shape(polies);
  }
  
  public void update(JSONArray ja) 
  {
    maxMesh = 0; 
    minMesh = 0;
    JSONArray tripleCoord, triplePixel;
    
    int [][][] temptri = new int[ja.size()][3][3];
    for(int tri = 0; tri<ja.size(); tri++)
    {
        tripleCoord = ja.getJSONArray(tri);
        for (int coord = 0; coord < tripleCoord.size(); coord++)
        {
          triplePixel = tripleCoord.getJSONArray(coord);   
          for (int value = 0; value < triplePixel.size(); value++)
          {
            temptri[tri][coord][value] = triplePixel.getInt(value);
            if (minMesh>temptri[tri][coord][value])
              minMesh = temptri[tri][coord][value];
            if (maxMesh<temptri[tri][coord][value])
              maxMesh = temptri[tri][coord][value];
          }
        }
    }
    triangles = temptri;
  }
}
