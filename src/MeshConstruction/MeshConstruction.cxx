#include <stdio.h>
#include <list>
#include <map>
#include <set>

#include "itkQuadEdgeMesh.h"
#include "itkMeshFileWriter.h"

#include "DICOMImage.h"

typedef itk::QuadEdgeMesh<float, 3> MeshType;
typedef MeshType::PointType PointType;
typedef MeshType::PointIdentifier PointIdentifier;
typedef MeshType::PointsContainer PointsContainer;
typedef MeshType::PointsContainerPointer PointsContainerPointer;
typedef itk::QuadEdgeMeshPolygonCell<MeshType::CellType> PolygonType;
typedef itk::MeshFileWriter<MeshType> WriterType;

struct PointComp {
  bool operator()(const PointType& left, const PointType& right) const {
    return left[0]+left[1]+left[2] < right[0]+right[1]+right[2];
  }
};

PointType UnitSphereProjection(PointType p) {
  PointType ret;
  float length = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);

  ret[0] = p[0] / length;
  ret[1] = p[1] / length;
  ret[2] = p[2] / length;

  return ret;
}
  
PointType GetMidPoint(PointType p1, PointType p2) {
  PointType mid;
  mid[0] = (p1[0] + p2[0]) / 2.0;
  mid[1] = (p1[1] + p2[1]) / 2.0;
  mid[2] = (p1[2] + p2[2]) / 2.0;

  return UnitSphereProjection(mid);
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
  int refinement = 3;
  float radius = 10.0;

  MeshType::Pointer mesh = MeshType::New();

  PointsContainerPointer points = PointsContainer::New();
  points->Reserve(100);

  float t = (1.0 + sqrt(5.0)) / 2.0;

  PointIdentifier k = 0;
  PointType p;

  // oh no tricky post increment! SURELY NO ONE IS SMART ENOUGH TO UNDERSTAND
  // THIS CODE, NO ONE IN THE WORLD!
  p[0] = -1.0; p[1] = t; p[2] = 0.0;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  // The joke here is that it's actually the shitty copy-on-pass semantics of
  // c/c++ that make this code tricky, not the counting
  p[0] = 1.0; p[1] = t; p[2] = 0.0;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = -1.0; p[1] = -t; p[2] = 0.0;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = 1.0; p[1] = -t; p[2] = 0.0;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = 0.0; p[1] = -1.0; p[2] = t;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = 0.0; p[1] = 1.0; p[2] = t;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = 0.0; p[1] = -1.0; p[2] = -t;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = 0.0; p[1] = 1.0; p[2] = -t;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = t; p[1] = 0.0; p[2] = -1.0;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = t; p[1] = 0.0; p[2] = 1.0;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = -t; p[1] = 0.0; p[2] = -1.0;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  p[0] = -t; p[1] = 0.0; p[2] = 1.0;
  p = UnitSphereProjection(p);
  points->SetElement(k++, p);

  mesh->SetPoints(points);

  MeshType::EdgeListType edges;

  // Collecting edges like an edgelord
  edges.push_back(mesh->AddFaceTriangle(0, 11, 5));
  edges.push_back(mesh->AddFaceTriangle(0, 5, 1));
  edges.push_back(mesh->AddFaceTriangle(0, 1, 7));
  edges.push_back(mesh->AddFaceTriangle(0, 7, 10));
  edges.push_back(mesh->AddFaceTriangle(0, 10, 11));

  edges.push_back(mesh->AddFaceTriangle(1, 5, 9));
  edges.push_back(mesh->AddFaceTriangle(5, 11, 4));
  edges.push_back(mesh->AddFaceTriangle(11, 10, 2));
  edges.push_back(mesh->AddFaceTriangle(10, 7, 6));
  edges.push_back(mesh->AddFaceTriangle(7, 1, 8));

  edges.push_back(mesh->AddFaceTriangle(3, 9, 4));
  edges.push_back(mesh->AddFaceTriangle(3, 4, 2));
  edges.push_back(mesh->AddFaceTriangle(3, 2, 6));
  edges.push_back(mesh->AddFaceTriangle(3, 6, 8));
  edges.push_back(mesh->AddFaceTriangle(3, 8, 9));

  edges.push_back(mesh->AddFaceTriangle(4, 9, 5));
  edges.push_back(mesh->AddFaceTriangle(2, 4, 11));
  edges.push_back(mesh->AddFaceTriangle(6, 2, 10));
  edges.push_back(mesh->AddFaceTriangle(8, 6, 7));
  edges.push_back(mesh->AddFaceTriangle(9, 8, 1));


  WriterType::Pointer writer0 = WriterType::New();
  writer0->SetFileName("lol.obj");
  writer0->SetInput(mesh);
  writer0->Update();

  printf("here1\n");
  mesh = BetterIcoSphere(mesh);
  printf("here5\n");

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

