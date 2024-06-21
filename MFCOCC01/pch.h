// pch.h: questo è un file di intestazione precompilata.
// I file elencati di seguito vengono compilati una sola volta, in modo da migliorare le prestazioni per le compilazioni successive.
// Questa impostazione influisce anche sulle prestazioni di IntelliSense, incluso il completamento codice e molte altre funzionalità di esplorazione del codice.
// I file elencati qui vengono però TUTTI ricompilati se uno di essi viene aggiornato da una compilazione all'altra.
// Non aggiungere qui file soggetti a frequenti aggiornamenti; in caso contrario si perderanno i vantaggi offerti in termini di prestazioni.

#ifndef PCH_H
#define PCH_H

//#define USE_IMGUI

#define GLFW_INCLUDE_GLU

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32

// aggiungere qui le intestazioni da precompilare
#include "framework.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <vector>
#include <random>
#include <OpenGl_GlFunctions.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Point.hxx>
#include <AIS_ViewController.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <WNT_Window.hxx>
#include <Standard_ErrorHandler.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <AIS_Shape.hxx>
#include <Prs3d_Drawer.hxx>
#include <BRepBndLib.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_IntCS.hxx>
#include <list>
#include <thread>

#include <AIS_TextLabel.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <STEPControl_Reader.hxx>

//#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>
#include <Aspect_Window.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_RenderingContext.hxx>
#include <cstdarg> 
#include <Font_FontAspect.hxx>
/* ASSIMP */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Compound.hxx>

#define WM_OUTPUTMSG_MESSAGE (WM_USER + 1)
#define WM_INSERTITEM_MESSAGE (WM_USER + 2)

#define DIR_CATALOGUE "C:\\merda\\"

struct Panel {
	gp_Pnt origin;
	Standard_Real height;
	Standard_Real width;
	Standard_Real thickness;
	Quantity_Color color;
    Quantity_Color originalColor;
    Handle(AIS_Shape) shape;
};

struct objList {
	Handle(AIS_Shape) shape;
	TopoDS_Shape topo_shape;
	CString name;
	gp_Pnt origin;
	Quantity_Color color;
};

/**************************************************************************************/

struct OutputMessageMsg {
	CString message;
};

struct InsertItemMsg {
    CString strItem;
};

#endif //PCH_H
