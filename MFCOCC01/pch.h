// pch.h: questo è un file di intestazione precompilata.
// I file elencati di seguito vengono compilati una sola volta, in modo da migliorare le prestazioni per le compilazioni successive.
// Questa impostazione influisce anche sulle prestazioni di IntelliSense, incluso il completamento codice e molte altre funzionalità di esplorazione del codice.
// I file elencati qui vengono però TUTTI ricompilati se uno di essi viene aggiornato da una compilazione all'altra.
// Non aggiungere qui file soggetti a frequenti aggiornamenti; in caso contrario si perderanno i vantaggi offerti in termini di prestazioni.

#ifndef PCH_H
#define PCH_H

#define GLFW_INCLUDE_GLU

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32

// aggiungere qui le intestazioni da precompilare
#include "framework.h"
#include <gl/GL.h>
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
//#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>
#include <Aspect_Window.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_RenderingContext.hxx>

#define WM_OUTPUTMSG_MESSAGE (WM_USER + 1)
#define WM_INSERTITEM_MESSAGE (WM_USER + 2)

struct Panel {
	gp_Pnt origin;
	Standard_Real height;
	Standard_Real width;
	Standard_Real thickness;
	Quantity_Color color;

};

struct FreeSpace {
    Standard_Real x, y, width, height;

    bool operator<(const FreeSpace& other) const {
        // Compare based on coordinates and then by size
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        if (width != other.width) return width < other.width;
        return height < other.height;
    }
};

/**************************************************************************************/

struct OutputMessageMsg {
	CString message;
};

struct InsertItemMsg {
    CString strItem;
};

#endif //PCH_H
