#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
setup.py file for Libgexf
"""

from setuptools import Extension, setup
#from distutils.core import Extension, setup

libgexf_module = Extension(
  '_libgexf', # genere un _libgexf.so
  include_dirs=['/usr/include/libxml2'],
  sources=[
    
   # 'libgexf.i', # genere un libgexf.py (ne fonctionne que pour les sources C et pas C++)

    # sources C: les .o seront automatiquement généré,
    # et automatiquement linké avec le module
    
    #io::input
    '../../libgexf/filereader.cpp',
    '../../libgexf/abstractparser.cpp',
    '../../libgexf/gexfparser.cpp',
    '../../libgexf/legacyparser.cpp',
    '../../libgexf/rngvalidator.cpp',
    '../../libgexf/schemavalidator.cpp',
    
    #io::output
    '../../libgexf/filewriter.cpp',
    '../../libgexf/legacywriter.cpp',
    
    #io::utils
    '../../libgexf/conv.cpp',
    
    #db::topo
    '../../libgexf/graph.cpp',
    '../../libgexf/dynamicgraph.cpp',
    '../../libgexf/directedgraph.cpp',
    '../../libgexf/undirectedgraph.cpp',
    '../../libgexf/nodeiter.cpp',
    '../../libgexf/edgeiter.cpp',
    
    #db::data
    '../../libgexf/data.cpp',
    '../../libgexf/metadata.cpp',
    '../../libgexf/attributeiter.cpp',
    '../../libgexf/attvalueiter.cpp',
    
    #main
    '../../libgexf/gexf.cpp',
    '../../libgexf/memoryvalidator.cpp',

    # chemin du wrapper généré automatiquement par SWIG (ce wrapper doit déjà exister donc)
    'libgexf_wrap.cpp',
  ],

  # eventuellement, les librairies à "linker"
  # par exemple si on a besoin de libxml2, c'est ici qu'on le spécifie au compilateur
  # attention aux habitués de gcc et de la compilation en ligne de commande:
  # ici inutile de donner le format spécifique à gcc ("-lpthread") ou spécifique à visual studio etc..
  # il suffit de mettre "pthread" et le script python va rajouter le "-l" devant si nécessaire
  libraries=[
     'stdc++',
     'xml2' #see xml2-config --libs to get the linker flags
     #'z', # zlib (compression) (inutile sous ubuntu par exemple, car déjà intégré au packaging de base pour développer)
     #'pthread' # Posix Threads (multithreading posix) (inutile sous linux, car posix fait déjà partie du système)
  ] 
)

setup (
       name='libgexf', # important, c'est le vrai nom du module, qui sera utilisé quand on fera un "import libgexf;" par exemple

       # metadonnees diverses
       version='0.1.2', 
       author="Sebastien Heymann",
       author_email="sebastien.heymann@gephi.org",
       url="http://gexf.net",
       description="""Toolkit library for GEXF file format.""",
       long_description="""""",
       
       # liste des modules à compiler. 
       # le module "libgexf_module" a été défini ligne 12
       # 
       ext_modules=[ libgexf_module, ],

       # si on veut rajouter un package python
       # par exemple 
       #             packages = ["monpackage"]
       # va rajouter le packag
       #             monpackage/
       # puisqu'en python les packages sont enfait tout simplement des répertoires contenant 
       # un fichier "constructeur" __init__.py (c'est un peu du système de fichier orienté objet)
       # cela aura pour effet de rajouter de manière récursive
       #             monpackage/__init__.py
       #             monpackage/sous/sous/sous/package/fichier.py
       # etc.. 
       #packages= ["monpackage", ], # 
       
       # si on veut rajouter des scripts python en plus
       # par exemple
       #              py_modules = ["monmodule"]
       # va rajouter le fichier
       #              monmodule.py (dans le répertoire courant)
       # dans le package
       py_modules = ["libgexf"], # UNCOMMENT TO USE THE SWIG WRAPPER

       # on peut rajouter des fichiers divers aussi (readme, examples, licences, doc html etc..)
       #data_files = [('share/libgexf-python/',['readme.txt']),],

       # encore des meta donnees, pour la base de donnees en ligne des modules python (python.org)
       classifiers=[
       "Development Status :: 2 - Pre-Alpha",
       "Intended Audience :: Science/Research",
       "Intended Audience :: Developers",
       "Intended Audience :: Information Technology",
       "License :: Free for non-commercial use",
       "Operating System :: POSIX :: Linux",
       "Topic :: Software Development :: Libraries :: Python Modules", ],
       )
