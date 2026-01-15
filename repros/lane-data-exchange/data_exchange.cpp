#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_VersionInfo.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

namespace
{
std::string JsonEscape(const std::string& input)
{
  std::ostringstream out;
  out << '"';
  for (const unsigned char c : input)
  {
    switch (c)
    {
      case '\\':
        out << "\\\\";
        break;
      case '"':
        out << "\\\"";
        break;
      case '\b':
        out << "\\b";
        break;
      case '\f':
        out << "\\f";
        break;
      case '\n':
        out << "\\n";
        break;
      case '\r':
        out << "\\r";
        break;
      case '\t':
        out << "\\t";
        break;
      default:
        if (c < 0x20)
        {
          out << "\\u" << std::hex << std::setw(4) << std::setfill('0')
              << static_cast<int>(c) << std::dec << std::setw(0);
        }
        else
        {
          out << static_cast<char>(c);
        }
        break;
    }
  }
  out << '"';
  return out.str();
}

std::string ReturnStatusToString(const IFSelect_ReturnStatus s)
{
  switch (s)
  {
    case IFSelect_RetVoid:
      return "RetVoid";
    case IFSelect_RetDone:
      return "RetDone";
    case IFSelect_RetError:
      return "RetError";
    case IFSelect_RetFail:
      return "RetFail";
    case IFSelect_RetStop:
      return "RetStop";
  }
  return "Unknown";
}

struct ShapeCounts
{
  Standard_Integer compounds = 0;
  Standard_Integer solids = 0;
  Standard_Integer shells = 0;
  Standard_Integer faces = 0;
  Standard_Integer edges = 0;
  Standard_Integer vertices = 0;
};

ShapeCounts Count(const TopoDS_Shape& shape)
{
  ShapeCounts counts;
  TopTools_IndexedMapOfShape map;
  TopExp::MapShapes(shape, TopAbs_COMPOUND, map);
  counts.compounds = map.Size();
  map.Clear();
  TopExp::MapShapes(shape, TopAbs_SOLID, map);
  counts.solids = map.Size();
  map.Clear();
  TopExp::MapShapes(shape, TopAbs_SHELL, map);
  counts.shells = map.Size();
  map.Clear();
  TopExp::MapShapes(shape, TopAbs_FACE, map);
  counts.faces = map.Size();
  map.Clear();
  TopExp::MapShapes(shape, TopAbs_EDGE, map);
  counts.edges = map.Size();
  map.Clear();
  TopExp::MapShapes(shape, TopAbs_VERTEX, map);
  counts.vertices = map.Size();
  return counts;
}

void PrintBBox(std::ostream& out, const TopoDS_Shape& shape)
{
  Bnd_Box box;
  BRepBndLib::Add(shape, box);
  if (box.IsVoid())
  {
    out << "{ \"is_void\": true }";
    return;
  }
  Standard_Real xmin = 0, ymin = 0, zmin = 0, xmax = 0, ymax = 0, zmax = 0;
  box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
  out << "{ \"is_void\": false, \"min\": [";
  out << std::setprecision(17) << xmin << ", " << ymin << ", " << zmin;
  out << "], \"max\": [";
  out << std::setprecision(17) << xmax << ", " << ymax << ", " << zmax;
  out << "] }";
}

TopoDS_Shape MakeSourceShape()
{
  const TopoDS_Shape box = BRepPrimAPI_MakeBox(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 10.0, 10.0)).Shape();

  const gp_Ax2 ax(gp_Pnt(0.0, 0.0, 0.0), gp::DZ());
  gp_Trsf tr;
  tr.SetTranslation(gp_Vec(12.0, 0.0, 0.0));
  const TopoDS_Shape cyl = BRepPrimAPI_MakeCylinder(ax, 3.0, 10.0).Shape().Moved(TopLoc_Location(tr));

  BRep_Builder builder;
  TopoDS_Compound compound;
  builder.MakeCompound(compound);
  builder.Add(compound, box);
  builder.Add(compound, cyl);
  return compound;
}
} // namespace

int main(int argc, char** argv)
{
  std::cout.imbue(std::locale::classic());

  if (argc != 2)
  {
    std::cerr << "usage: data-exchange <out.step>\n";
    return 2;
  }

  const std::filesystem::path outStep = argv[1];

  const char* versionStr = OCCT_Version_String_Extended();
  const char* devStr = OCCT_DevelopmentVersion();

  const TopoDS_Shape src = MakeSourceShape();
  const ShapeCounts srcCounts = Count(src);

  STEPControl_Writer writer;
  writer.Model(true); // create model early for consistent behavior
  writer.Transfer(src, STEPControl_AsIs);
  const IFSelect_ReturnStatus writeStatus = writer.Write(outStep.string().c_str());

  STEPControl_Reader reader;
  const IFSelect_ReturnStatus readStatus = reader.ReadFile(outStep.string().c_str());
  const Standard_Integer nbRoots = reader.NbRootsForTransfer();
  const Standard_Integer nbTransferred = reader.TransferRoots();
  const TopoDS_Shape imported = reader.OneShape();

  const ShapeCounts importedCounts = Count(imported);
  const Handle(Interface_InterfaceModel) model = reader.Model();
  const Standard_Integer nbEntities = model.IsNull() ? 0 : model->NbEntities();

  std::cout << "{\n";
  std::cout << "  \"meta\": {\n";
  std::cout << "    \"occt_version\": " << JsonEscape(versionStr == nullptr ? "" : versionStr) << ",\n";
  std::cout << "    \"occt_dev\": " << JsonEscape(devStr == nullptr ? "" : devStr) << "\n";
  std::cout << "  },\n";

  std::cout << "  \"step\": {\n";
  std::cout << "    \"write_status\": { \"code\": " << static_cast<int>(writeStatus)
            << ", \"name\": " << JsonEscape(ReturnStatusToString(writeStatus)) << " },\n";
  std::cout << "    \"read_status\": { \"code\": " << static_cast<int>(readStatus)
            << ", \"name\": " << JsonEscape(ReturnStatusToString(readStatus)) << " },\n";
  std::cout << "    \"model_nb_entities\": " << nbEntities << ",\n";
  std::cout << "    \"nb_roots_for_transfer\": " << nbRoots << ",\n";
  std::cout << "    \"nb_roots_transferred\": " << nbTransferred << "\n";
  std::cout << "  },\n";

  std::cout << "  \"source\": {\n";
  std::cout << "    \"counts\": { \"compounds\": " << srcCounts.compounds << ", \"solids\": "
            << srcCounts.solids << ", \"shells\": " << srcCounts.shells << ", \"faces\": "
            << srcCounts.faces << ", \"edges\": " << srcCounts.edges << ", \"vertices\": "
            << srcCounts.vertices << " },\n";
  std::cout << "    \"bbox\": ";
  PrintBBox(std::cout, src);
  std::cout << "\n";
  std::cout << "  },\n";

  std::cout << "  \"imported\": {\n";
  std::cout << "    \"counts\": { \"compounds\": " << importedCounts.compounds << ", \"solids\": "
            << importedCounts.solids << ", \"shells\": " << importedCounts.shells << ", \"faces\": "
            << importedCounts.faces << ", \"edges\": " << importedCounts.edges << ", \"vertices\": "
            << importedCounts.vertices << " },\n";
  std::cout << "    \"bbox\": ";
  PrintBBox(std::cout, imported);
  std::cout << "\n";
  std::cout << "  }\n";

  std::cout << "}\n";
  return 0;
}

