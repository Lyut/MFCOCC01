// pch.h: questo è un file di intestazione precompilata.
// I file elencati di seguito vengono compilati una sola volta, in modo da migliorare le prestazioni per le compilazioni successive.
// Questa impostazione influisce anche sulle prestazioni di IntelliSense, incluso il completamento codice e molte altre funzionalità di esplorazione del codice.
// I file elencati qui vengono però TUTTI ricompilati se uno di essi viene aggiornato da una compilazione all'altra.
// Non aggiungere qui file soggetti a frequenti aggiornamenti; in caso contrario si perderanno i vantaggi offerti in termini di prestazioni.

#ifndef PCH_H
#define PCH_H

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
#include <AIS_Shape.hxx>
#include <Prs3d_Drawer.hxx>
#include <BRepBndLib.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_IntCS.hxx>
#include <list>

struct Panel {
	gp_Pnt origin;
	Standard_Real height;
	Standard_Real width;
	Standard_Real thickness;
	Quantity_Color color;

};

#endif //PCH_H
