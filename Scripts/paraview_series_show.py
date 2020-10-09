# Example of Python script that shows and saves the series of PLY polyhedra

for i in range(200):
    reader = PLYReader(FileName="/home/ilya/thesis/polyhedra-correction-library/result-cubecut3/cubecut.{:03d}.am2-recovered.ply".format(10 * i + 9))
    SetDisplayProperties(reader, Representation="Wireframe", LineWidth=10)
    Show(reader)
    SaveScreenshot("/home/ilya/thesis/polyhedra-correction-library/result-cubecut3/cubecut.{:03d}.am2-recovered.png".format(10 * i + 9))
    Hide(reader)
