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

MeshType::Pointer Meshulate(TriangleList tlist) {
  MeshType::Pointer mesh = MeshType::New();

  std::map<PointArrType, PointIdentifier> pcache;

  PointsContainerPointer points = PointsContainer::New();
  points->Reserve(tlist.size());
  PointIdentifier k;
  k = 0;

  for (TriangleList::iterator it=tlist.begin(); it!=tlist.end(); it++) {
    Triangle tri = *it;
    std::cout << k << '\n';

    std::cout << tri.a[0] << ' ' << tri.a[1] << ' ' << tri.a[2] << '\n';
    if (pcache.count(p2arr(tri.a)) == 0) {
      points->SetElement(k, tri.a);
      pcache.insert(std::pair<PointArrType, PointIdentifier>(p2arr(tri.a), k));
      k++;

    } else { std::cout << "CAUGHT!\n"; }
    std::cout << '\n';

    if (pcache.count(p2arr(tri.b)) == 0) {
      points->SetElement(k, tri.b);
      pcache.insert(std::pair<PointArrType, PointIdentifier>(p2arr(tri.b), k));
      k++;
    } else { std::cout << "CAUGHT!\n"; }


    if (pcache.count(p2arr(tri.c)) == 0) {
      points->SetElement(k, tri.c);
      pcache.insert(std::pair<PointArrType, PointIdentifier>(p2arr(tri.c), k));
      k++;
    } else { std::cout << "CAUGHT!\n"; }
  }

  mesh->SetPoints(points);

  for (TriangleList::iterator it=tlist.begin(); it!=tlist.end(); it++) {
    Triangle tri = *it;

    std::cout << tri.a[0] << ' ' << tri.a[1] << ' ' << tri.a[2] << '\n';
    std::cout << tri.b[0] << ' ' << tri.b[1] << ' ' << tri.b[2] << '\n';
    std::cout << tri.c[0] << ' ' << tri.c[1] << ' ' << tri.c[2] << '\n';
    std::cout << '\n';

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

MeshType::Pointer BetterIcoSphere(MeshType::Pointer mesh) {
  MeshType::Pointer better = MeshType::New();

  std::map< PointIdentifier, PointIdentifier > verticesMap;
  std::set< MeshType::CellIdentifier > facesSet;

  MeshType::PointsContainer::Pointer mpoints = mesh->GetPoints();
  MeshType::PointsContainer::Iterator pIt = mpoints->Begin();
  MeshType::PointsContainer::Iterator pEnd = mpoints->End();

  printf("here2\n");
  while (pIt != pEnd) {
    PointType p = pIt->Value();

    printf("here2.01\n");
    if (p[2] < 0.) {
      MeshType::QEType* qe = p.GetEdge();
      MeshType::QEType* temp = qe;
      do {
        printf("here2.03\n");
        // insert the corresponding faces into std::set
        MeshType::CellIdentifier cell = temp->GetLeft();
        printf("here2.035\n");
        facesSet.insert(cell);
        printf("here2.04\n");
        temp = temp->GetLnext();
      } while( qe != temp );
    }
    ++pIt;
  }
  printf("here2.1\n");

  MeshType::CellsContainerPointer cells = mesh->GetCells();
  PointsContainerPointer points = PointsContainer::New();
  points->Reserve(1000);

  printf("here2.2\n");

  std::map<PointType, PointIdentifier, PointComp> pcache;
  std::list<PointIdentifier> triangles;

  printf("here2.3\n");

  for(std::set< MeshType::CellIdentifier >::iterator fIt = facesSet.begin();
      fIt != facesSet.end(); ++fIt ) {
    PolygonType* face = dynamic_cast< PolygonType* >( cells->ElementAt( *fIt ) );
    PointIdentifier k;

    MeshType::QEType *qe = face->GetEdgeRingEntry();

    PointType p1, p2, p3, p12, p13, p23;
    PointIdentifier pi1, pi2, pi3, pi12, pi13, pi23;

    p1 = mesh->GetPoint(qe->GetOrigin());
    qe = qe->GetLnext();
    p2 = mesh->GetPoint(qe->GetOrigin());
    qe = qe->GetLnext();
    p3 = mesh->GetPoint(qe->GetOrigin());


    printf("here2.4\n");

    p12 = GetMidPoint(p1, p2);
    p13 = GetMidPoint(p1, p3);
    p23 = GetMidPoint(p3, p2);


    printf("here2.5\n");

    if (pcache.find(p1) != pcache.end()) { 
      pi1 = pcache.find(p1)->second; 
    } else {
      points->SetElement(k, p1);
      pcache.insert(std::pair<PointType, PointIdentifier>(p1, k));
      pi1 = k++;
    }

    if (pcache.find(p2) != pcache.end()) { 
      pi1 = pcache.find(p2)->second; 
    } else {
      points->SetElement(k, p2);
      pcache.insert(std::pair<PointType, PointIdentifier>(p2, k));
      pi1 = k++;
    }

    if (pcache.find(p3) != pcache.end()) { 
      pi1 = pcache.find(p3)->second; 
    } else {
      points->SetElement(k, p3);
      pcache.insert(std::pair<PointType, PointIdentifier>(p3, k));
      pi3 = k++;
    }

    if (pcache.find(p12) != pcache.end()) { 
      pi1 = pcache.find(p12)->second; 
    } else {
      points->SetElement(k, p12);
      pcache.insert(std::pair<PointType, PointIdentifier>(p12, k));
      pi1 = k++;
    }

    if (pcache.find(p13) != pcache.end()) { 
      pi1 = pcache.find(p13)->second; 
    } else {
      points->SetElement(k, p13);
      pcache.insert(std::pair<PointType, PointIdentifier>(p13, k));
      pi1 = k++;
    }

    if (pcache.find(p23) != pcache.end()) { 
      pi1 = pcache.find(p23)->second; 
    } else {
      points->SetElement(k, p23);
      pcache.insert(std::pair<PointType, PointIdentifier>(p23, k));
      pi1 = k++;
    }


    triangles.push_back(pi1); triangles.push_back(pi12); triangles.push_back(pi13);
    triangles.push_back(pi2); triangles.push_back(pi23); triangles.push_back(pi12);
    triangles.push_back(pi12); triangles.push_back(pi23); triangles.push_back(pi13);
    triangles.push_back(pi23); triangles.push_back(pi3); triangles.push_back(pi13);
    printf("here3.5\n");
  }

  better->SetPoints(points);

  for (std::list<PointIdentifier>::iterator it=triangles.begin();
      it!=triangles.end(); it++) {
    PointIdentifier p1 = *it,
      p2 = *(++it),
      p3 = *(it);

    better->AddFaceTriangle(p1, p2, p3);
  }

  return better;
}

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cerr << "Usage: "<< std::endl;
    std::cerr << argv[0];
    std::cerr << " <OutputFileName>";
    std::cerr << std::endl;
    return 1;
  }

  const char * outputFileName = argv[1];
  int refinement = 1;
  float radius = 1.0;

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


  MeshType::Pointer mesh = Meshulate(tlist);

  WriterType::Pointer writer0 = WriterType::New();
  writer0->SetFileName("lol.obj");
  writer0->SetInput(mesh);
  writer0->Update();

  tlist = BetterIcoSphere(tlist);
  mesh = Meshulate(tlist);

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

