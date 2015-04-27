//import apwidgets.*;

float yr = PI / 4;
//float xr = -0.5;
float xr = PI /4;
//APWidgetContainer widgetContainer; 
//APButton right, left, in, out;

int[][][] fromStrip(int[][] strip) {
  int[][][] triangles = new int[strip.length - 2][3][3];
  
  for (int i=2; i<strip.length; i++) {
    int[][] threeTriangles = {strip[i-2], strip[i-1], strip[i]};
    triangles[i-2] = threeTriangles;
  }
  
  return triangles;
}

class CutawayPlane {
  int DIM_X = 0, DIM_Y = 1, DIM_Z = 2;
  
  int location;
  int keepDirection = 1;
  
  // This could explode in a really bad way if anyone mutates it down the line,
  // let's really hope that doesn't happen
  int[][] nullArr = {null};
  
  // Note that `~0` mean the int with every bit set
  private int cutDim;  
  
  CutawayPlane(int location, int cutDim) {
    this.location = location;
    
    this.setCutDim(cutDim);
  }
  
  public void setCutDim(int cutDim) {
    this.cutDim = cutDim;
  }
  
  void draw() {
//    fill(153);
//    noFill();
//    pushMatrix();
//    
//    if (this.cutDim == DIM_X) rotateY(HALF_PI);
//    else if (this.cutDim == DIM_Y) rotateX(-HALF_PI);
//    
//    // This might look a little wonky but it works since we're rotated in the 
//    // plane's direction, increasing z moves us "forwards" relative to our
//    // current (not absolute) facing.
//    translate(0, 0, this.location);
//   
//    
//    scale(0.75);    
//    rectMode(CENTER);
//    rect(0, 0, width, height);
    fill(153);
//    noFill();
    pushMatrix();
    translate(0,0, this.location);
    if (this.cutDim == DIM_X) rotateY(HALF_PI);
    else if (this.cutDim == DIM_Y) rotateX(-HALF_PI);

    beginShape(QUADS);
    texture(img);
    vertex(0, 0,  0, 0);
    vertex(100, 0,  100, 0);
    vertex(100, 100,  100, 100);
    vertex(0, 100,  0, 100);
//    vertex(0,0);
//    vertex(100,0);
//    vertex(100,100);
//    vertex(0,100);
    endShape();
    popMatrix();
//    popMatrix();
  }
  
  float distToPoint(int[] point) {
    return abs(this.location - point[this.cutDim]);
  }
    
  int pointsThisSide(int[][] poly) {
    int i = 0;
    
    for (int k=0; k<poly.length; k++) {
      // TODO: Care about keep direction
      if (poly[k][this.cutDim] > this.location) {
        i++;
      }
    }
    
    return i;
  }
  
  int[] interpolate(int[] p1, int[] p2) {
    float totes = dist(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]),
      slope = (p2[this.cutDim] - p1[this.cutDim]) / totes,
      d = this.location / slope,
      sx = (p2[0] - p1[0]) / totes,
      sy = (p2[1] - p1[1]) / totes,
      sz = (p2[2] - p1[2]) / totes;
      
    int[] intersectionPoint = {
      p2[0] + (int)(sx * d),
      p2[1] + (int)(sy * d),
      p2[2] + (int)(sz * d)
    };
   
   return intersectionPoint;
  }
  
  int[][] cutPoly(int[][] poly) {
    int pts = this.pointsThisSide(poly);
    if (pts == 3) {
      return poly;      
    } else if (pts == 0) {
      return this.nullArr;
    } else if (pts == 2) {
      int didx = -1; // drop index
      
      while (true) {
        if (poly[++didx][this.cutDim] <= this.location) break;
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

    } else if (pts == 1) {
      int kidx = -1;
      
      while (true) {
        if (poly[++kidx][this.cutDim] > this.location) break;
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

class Boxxen {
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
  
  
  
  void drawPoly(int[][] poly) {
    if (poly.length == 0 || poly[0] == null) return;
//    texture(img);
    beginShape();
    for (int i=0; i<poly.length; i++) {
      if (poly[i] == null) break;
      
      vertex(poly[i][0], poly[i][1], poly[i][2]);
    }
    vertex(poly[0][0], poly[0][1], poly[0][2]);
    endShape();
  }
  
  void draw(CutawayPlane cap) {
    fill(255);
    for (int i=0; i<triangles.length; i++) {
      this.drawPoly(cap.cutPoly(triangles[i]));
    }
  }
  public void update(JSONArray ja) {
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
          }
        }
    }
    triangles = temptri;
  }
}

//void setup() {
//  orientation(LANDSCAPE);
//  size(800, 600, P3D);
//  ortho();
//  box = new Boxxen();
//  cap = new CutawayPlane(10, 0);
  //Set up container and widgets
//  widgetContainer = new APWidgetContainer(this); 
//  right = new APButton(0, 50, "Right");
//  left = new APButton(0, 200, "Left"); 
//  in = new APButton(0, 350, "In"); 
//  out = new APButton(0, 500, "Out"); 
//  widgetContainer.addWidget(right); 
//  widgetContainer.addWidget(left); 
//  widgetContainer.addWidget(in); 
//  widgetContainer.addWidget(out); 
//}



//void keyPressed() {
//  if (key == CODED) {
//    if (keyCode == RIGHT) yr += 0.1;
//    else if (keyCode == LEFT) yr -= 0.1;
//    else if (keyCode == UP) xr += 0.1;
//    else if (keyCode == DOWN) xr -= 0.1;
//  } else {
//    if (keyCode == 'X') { cap.setCutDim(0); }
//    else if (keyCode == 'Y') cap.setCutDim(1);
//    else if (keyCode == 'Z') cap.setCutDim(2);
//    else if (keyCode == 61) cap.location += 2;
//    else if (keyCode == 45) cap.location -= 2;
//  }
//}

