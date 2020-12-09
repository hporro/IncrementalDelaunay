import plotly.graph_objects as go

class PlotNeighbours (gdb.Command):
  def __init__ (self):
      super (PlotNeighbours, self).__init__ ("PlotNeighbours", gdb.COMMAND_USER)

  def invoke (self, arg, from_tty):
        mesh = gdb.parse_and_eval(arg.split()[0])
        t1 = int(gdb.parse_and_eval(arg.split()[1]))
        t2 = int(gdb.parse_and_eval(arg.split()[2]))

        triangle1 = mesh["triangles"][t1]
        triangle2 = mesh["triangles"][t2]

        vertices1 = (mesh["vertices"][triangle1["v"][0]]["pos"]["x"],mesh["vertices"][triangle1["v"][0]]["pos"]["y"],mesh["vertices"][triangle1["v"][1]]["pos"]["x"],mesh["vertices"][triangle1["v"][1]]["pos"]["y"],mesh["vertices"][triangle1["v"][2]]["pos"]["x"],mesh["vertices"][triangle1["v"][2]]["pos"]["y"])
        vertices2 = (mesh["vertices"][triangle2["v"][0]]["pos"]["x"],mesh["vertices"][triangle2["v"][0]]["pos"]["y"],mesh["vertices"][triangle2["v"][1]]["pos"]["x"],mesh["vertices"][triangle2["v"][1]]["pos"]["y"],mesh["vertices"][triangle2["v"][2]]["pos"]["x"],mesh["vertices"][triangle2["v"][2]]["pos"]["y"])

        fig1 = dict(type="path",path="M {0[0]} {0[1]} L {0[2]} {0[3]} L {0[4]} {0[5]} Z".format(vertices1), line_color="Crimson")
        fig2 = dict(type="path",path="M {0[0]} {0[1]} L {0[2]} {0[3]} L {0[4]} {0[5]} Z".format(vertices2))

        print(triangle1)
        print(triangle2)

        fig = go.Figure()
        fig.update_xaxes(range=[float(mesh["p0"]["x"]), float(mesh["p1"]["x"])])
        fig.update_yaxes(range=[float(mesh["p0"]["y"]), float(mesh["p2"]["y"])])
        fig.update_layout(shapes=[fig1,fig2])
        fig.show()

class PlotMesh (gdb.Command):
  def __init__ (self):
      super (PlotMesh, self).__init__ ("PlotMesh", gdb.COMMAND_USER)

  def invoke (self, arg, from_tty):
        mesh = gdb.parse_and_eval(arg)
        x = []
        y = []
        for i in range(mesh["tcount"]):
            triangle = mesh["triangles"][i]
            vertices = (mesh["vertices"][triangle["v"][0]]["pos"]["x"],mesh["vertices"][triangle["v"][0]]["pos"]["y"],mesh["vertices"][triangle["v"][1]]["pos"]["x"],mesh["vertices"][triangle["v"][1]]["pos"]["y"],mesh["vertices"][triangle["v"][2]]["pos"]["x"],mesh["vertices"][triangle["v"][2]]["pos"]["y"])
            x.append(vertices[0])
            x.append(vertices[2])
            x.append(vertices[4])
            x.append(vertices[0])
            y.append(vertices[1])
            y.append(vertices[3])
            y.append(vertices[5])
            y.append(vertices[1])
            x.append(None)
            y.append(None)

        x = [float(i) if i!=None else None for i in x]
        y = [float(i) if i!=None else None for i in y]
        fig = go.Figure(go.Scatter(x=x, y=y, line_color="Crimson"))
        fig.update_xaxes(range=[float(mesh["p0"]["x"]), float(mesh["p1"]["x"])])
        fig.update_yaxes(range=[float(mesh["p0"]["y"]), float(mesh["p2"]["y"])])
        fig.show()

class OldPlotMesh (gdb.Command):
  def __init__ (self):
      super (OldPlotMesh, self).__init__ ("OldPlotMesh", gdb.COMMAND_USER)

  def invoke (self, arg, from_tty):
        mesh = gdb.parse_and_eval(arg)
        figs = []
        for i in range(mesh["tcount"]):
            triangle = mesh["triangles"][i]
            vertices = (mesh["vertices"][triangle["v"][0]]["pos"]["x"],mesh["vertices"][triangle["v"][0]]["pos"]["y"],mesh["vertices"][triangle["v"][1]]["pos"]["x"],mesh["vertices"][triangle["v"][1]]["pos"]["y"],mesh["vertices"][triangle["v"][2]]["pos"]["x"],mesh["vertices"][triangle["v"][2]]["pos"]["y"])
            fig = dict(type="path",path="M {0[0]} {0[1]} L {0[2]} {0[3]} L {0[4]} {0[5]} Z".format(vertices), line_color="Crimson")
            figs.append(fig)
        fig = go.Figure()
        fig.update_xaxes(range=[float(mesh["p0"]["x"]), float(mesh["p1"]["x"])])
        fig.update_yaxes(range=[float(mesh["p0"]["y"]), float(mesh["p2"]["y"])])
        fig.show()

PlotNeighbours()
OldPlotMesh()
PlotMesh()
