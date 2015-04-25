#include <stdio.h>
#include <list>
#include <array>
#include <map>
#include <set>

#include "itkQuadEdgeMesh.h"
#include "itkMeshFileWriter.h"

#include "DICOMImage.h"

typedef itk::QuadEdgeMesh<float, 3> MeshType;
typedef MeshType::PointType PointType;
typedef std::array<float, 3> PointArrType;
typedef MeshType::PointIdentifier PointIdentifier;
typedef MeshType::PointsContainer PointsContainer;
typedef MeshType::PointsContainerPointer PointsContainerPointer;
typedef itk::QuadEdgeMeshPolygonCell<MeshType::CellType> PolygonType;
typedef itk::MeshFileWriter<MeshType> WriterType;

struct PointComp {
  bool operator()(const PointType& left, const PointType& right) const {
    if (left[0] > right[0]) return false;
    else if (left[1] > right[1]) return false;
    else if (left[2] > right[2]) return false;
    else if (left[0] == right[0] && left[1] == right[1] && left[2] == right[2]) return false;
    return true;
  }
};

struct Triangle {
  PointType a, b, c;
};

typedef std::list<Triangle> TriangleList;

void PrintPoint(PointType p) {
  std::cout << p[0] << ' ' << p[1] << ' ' << p[2] << '\n';
}

PointType UnitSphereProjection(PointType p) {
  PointType ret;
  float length = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);

  ret[0] = p[0] / length;
  ret[1] = p[1] / length;
  ret[2] = p[2] / length;

  return ret;
}

PointArrType p2arr(PointType p) {
  PointArrType pa;
  pa[0] = p[0]; pa[1] = p[1]; pa[2] = p[2]; 
  return pa;
}

PointType arr2p(PointArrType pa) {
  PointType p;
  p[0] = pa[0]; p[1] = pa[1]; p[2] = pa[2]; 
  return p;
}

PointType GetMidPoint(PointType p1, PointType p2) {
  PointType mid;
  mid[0] = (p1[0] + p2[0]) / 2.0;
  mid[1] = (p1[1] + p2[1]) / 2.0;
  mid[2] = (p1[2] + p2[2]) / 2.0;

  return UnitSphereProjection(mid);
}

std::string JSONify(TriangleList tlist) {
  std::stringstream json;

  json << "[\n";

  for (TriangleList::iterator it=tlist.begin(); it!=tlist.end(); it++) {
    Triangle tri = *it;
    json << " [\n";

    json << "  [";
    json << (int)tri.a[0] << ", " << (int)tri.a[1] << ", " << (int)tri.a[2];
    json << "],\n";

    json << "  [";
    json << (int)tri.b[0] << ", " << (int)tri.b[1] << ", " << (int)tri.b[2];
    json << "],\n";

    json << "  [";
    json << (int)tri.c[0] << ", " << (int)tri.c[1] << ", " << (int)tri.c[2];
    json << "],\n";


    json << " ],\n";
  }

  json << "]\n";

  return json.str();
}

MeshType::Pointer Meshulate(TriangleList tlist) {
  MeshType::Pointer mesh = MeshType::New();

  std::map<PointArrType, PointIdentifier> pcache;

  PointsContainerPointer points = PointsContainer::New();
  points->Reserve(tlist.size());
  PointIdentifier k;
  k = 0;

  for (TriangleList::iterator it=tlist.begin(); it!=tlist.end(); it++) {
    Triangle tri = *it;

    if (pcache.count(p2arr(tri.a)) == 0) {
      points->SetElement(k, tri.a);
      pcache.insert(std::pair<PointArrType, PointIdentifier>(p2arr(tri.a), k));
      k++;

    }

    if (pcache.count(p2arr(tri.b)) == 0) {
      points->SetElement(k, tri.b);
      pcache.insert(std::pair<PointArrType, PointIdentifier>(p2arr(tri.b), k));
      k++;
    }


    if (pcache.count(p2arr(tri.c)) == 0) {
      points->SetElement(k, tri.c);
      pcache.insert(std::pair<PointArrType, PointIdentifier>(p2arr(tri.c), k));
      k++;
    }
  }

  mesh->SetPoints(points);

  for (TriangleList::iterator it=tlist.begin(); it!=tlist.end(); it++) {
    Triangle tri = *it;

    PointIdentifier k1 = pcache.find(p2arr(tri.a))->second,
      k2 = pcache.find(p2arr(tri.b))->second,
      k3 = pcache.find(p2arr(tri.c))->second;

    mesh->AddFaceTriangle(k1, k2, k3);
  }

  return mesh;
}

TriangleList BetterIcoSphere(TriangleList tlist) {
  TriangleList better;

  for (TriangleList::iterator it=tlist.begin(); it!=tlist.end(); it++) {
    Triangle tri = *it;
    Triangle temp_tri;
    PointType p1, p2, p3, p12, p13, p23;

    p1 = tri.a;
    p2 = tri.b;
    p3 = tri.c;

    p12 = GetMidPoint(p1, p2);
    p13 = GetMidPoint(p1, p3);
    p23 = GetMidPoint(p3, p2);


    temp_tri.a = p1; temp_tri.b = p12; temp_tri.c = p13;
    better.push_back(temp_tri);

    temp_tri.a = p2; temp_tri.b = p23; temp_tri.c = p12;
    better.push_back(temp_tri);

    temp_tri.a = p12; temp_tri.b = p23; temp_tri.c = p13;
    better.push_back(temp_tri);

    temp_tri.a = p23; temp_tri.b = p3; temp_tri.c = p13;
    better.push_back(temp_tri);
  }

  return better;
}

TriangleList BuildIcoSphere(int refinement, float radius) {
  float t = (1.0 + sqrt(5.0)) / 2.0;

  PointIdentifier k = 0;
  PointType p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11;

  p0[0] = -1.0; p0[1] =    t; p0[2] =   0.0;
  p1[0] =  1.0; p1[1] =    t; p1[2] =   0.0;
  p2[0] = -1.0; p2[1] =   -t; p2[2] =   0.0;
  p3[0] =  1.0; p3[1] =   -t; p3[2] =   0.0;
  p4[0] =  0.0; p4[1] = -1.0; p4[2] =     t;
  p5[0] =  0.0; p5[1] =  1.0; p5[2] =     t;
  p6[0] =  0.0; p6[1] = -1.0; p6[2] =    -t;
  p7[0] =  0.0; p7[1] =  1.0; p7[2] =    -t;
  p8[0] =    t; p8[1] =  0.0; p8[2] =  -1.0;
  p9[0] =    t; p9[1] =  0.0; p9[2] =   1.0;
  p10[0] =  -t; p10[1] = 0.0; p10[2] = -1.0;
  p11[0] =  -t; p11[1] = 0.0; p11[2] =  1.0;

  p0 = UnitSphereProjection(p0);
  p1 = UnitSphereProjection(p1);
  p2 = UnitSphereProjection(p2);
  p3 = UnitSphereProjection(p3);
  p4 = UnitSphereProjection(p4);
  p5 = UnitSphereProjection(p5);
  p6 = UnitSphereProjection(p6);
  p7 = UnitSphereProjection(p7);
  p8 = UnitSphereProjection(p8);
  p9 = UnitSphereProjection(p9);
  p10 = UnitSphereProjection(p10);
  p11 = UnitSphereProjection(p11);

  TriangleList tlist;
  Triangle tri;

  tri = ((struct Triangle) {p0, p11, p5}); tlist.push_back(tri);
  tri = ((struct Triangle) {p0, p5, p1}); tlist.push_back(tri);
  tri = ((struct Triangle) {p0, p1, p7}); tlist.push_back(tri);
  tri = ((struct Triangle) {p0, p7, p10}); tlist.push_back(tri);
  tri = ((struct Triangle) {p0, p10, p11}); tlist.push_back(tri);

  tri = ((struct Triangle) {p1, p5, p9}); tlist.push_back(tri);
  tri = ((struct Triangle) {p5, p11, p4}); tlist.push_back(tri);
  tri = ((struct Triangle) {p11, p10, p2}); tlist.push_back(tri);
  tri = ((struct Triangle) {p10, p7, p6}); tlist.push_back(tri);
  tri = ((struct Triangle) {p7, p1, p8}); tlist.push_back(tri);

  tri = ((struct Triangle) {p3, p9, p4}); tlist.push_back(tri);
  tri = ((struct Triangle) {p3, p4, p2}); tlist.push_back(tri);
  tri = ((struct Triangle) {p3, p2, p6}); tlist.push_back(tri);
  tri = ((struct Triangle) {p3, p6, p8}); tlist.push_back(tri);
  tri = ((struct Triangle) {p3, p8, p9}); tlist.push_back(tri);

  tri = ((struct Triangle) {p4, p9, p5}); tlist.push_back(tri);
  tri = ((struct Triangle) {p2, p4, p11}); tlist.push_back(tri);
  tri = ((struct Triangle) {p6, p2, p10}); tlist.push_back(tri);
  tri = ((struct Triangle) {p8, p6, p7}); tlist.push_back(tri);
  tri = ((struct Triangle) {p9, p8, p1}); tlist.push_back(tri);

  for (int i=0; i<refinement; i++) {
    tlist = BetterIcoSphere(tlist);
  }

  for (TriangleList::iterator it=tlist.begin(); it!=tlist.end(); it++) {
    Triangle tri = *it;
    tri.a[0] = tri.a[0] * radius;
    tri.a[1] = tri.a[1] * radius;
    tri.a[2] = tri.a[2] * radius;

    tri.b[0] = tri.b[0] * radius;
    tri.b[1] = tri.b[1] * radius;
    tri.b[2] = tri.b[2] * radius;

    tri.c[0] = tri.c[0] * radius;
    tri.c[1] = tri.c[1] * radius;
    tri.c[2] = tri.c[2] * radius;

    it = tlist.erase(it);
    it = tlist.insert(it, tri);
  }

  return tlist;
}

int main(int argc, char* argv[]) {
  if(argc != 4) {
    std::cerr << "Usage: "<< std::endl;
    std::cerr << argv[0];
    std::cerr << "<Refinement> <Radius> <OutputFileName>";
    std::cerr << std::endl;
    return 1;
  }

  int refinement = std::atoi(argv[1]);
  float radius = std::atof(argv[2]);
  const char * outputFileName = argv[3];

  TriangleList icosphere = BuildIcoSphere(refinement, radius);

  std::cout << JSONify(icosphere);

  MeshType::Pointer mesh = Meshulate(icosphere);

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputFileName);
  writer->SetInput(mesh);
  try {
    writer->Update();
  } catch(itk::ExceptionObject & error) {
    std::cerr << "Error: " << error << std::endl;
    return 2;
  }

  return 0;
}

