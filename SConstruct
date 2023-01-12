from utils import *
#from init_env import init_environment
import platform
import SCons
import os
from SCons.Environment import *
from SCons.Variables import *
from SCons.Script import *


mc_enable = ARGUMENTS.get('MC',0)
if int(mc_enable):
    print "MC support is enabled - not supported for now"
    exit
#    env = init_environment("qt5 root geant4 clhep evio xercesc ccdb mlibrary clas12")
#   env.AppendUnique(CPPDEFINES='MC_SUPPORT_ENABLE')
else:
    print bcolors.WARNING," no MC support",bcolors.ENDC
    #Need a manual setting
    opts = Variables()
    cmloptions(opts)
    env = Environment(tools=['default'], options = opts, ENV = os.environ)
#    from loadccdb import loadccdb
#    loadccdb(env)
#    from loadclhep import loadclhep
#    loadclhep(env)

#A.C. necessary on older g++ versions
env.Append(CXXFLAGS = '-std=c++17')
#env.Append(CXXFLAGS = '-std=c++14')
#env.Append(CXXFLAGS = '-std=c++11')
if os.environ.get('JANA_HOME') is not None:
    env['JANA_HOME'] = os.environ.get('JANA_HOME')
    env.Append(CPPPATH=["$JANA_HOME/include", "$JANA_HOME/src/libraries"])  # n.b. we should not need $JANA_HOME/src/libraries !  DL
    env.Append(LIBPATH=["$JANA_HOME/lib"])
else:
    print("ERROR, JANA_HOME NOT DEFINED")
    exit
 
if os.environ.get('BOOSTROOT') is not None:
    env['BOOSTROOT'] = os.environ.get('BOOSTROOT')
    env.Append(CPPPATH=["$BOOSTROOT/include"])
else:
    print("ERROR, BOOSTROOT NOT DEFINED")
    exit    

if os.environ.get('CCDB_HOME') is not None:
    env['CCDB_HOME'] = os.environ.get('CCDB_HOME')
    env.Append(CPPPATH=["$CCDB_HOME/include"])
    env.Append(LIBPATH=["$CCDB_HOME/lib"])
else:
    print("ERROR, CCDB_HOME NOT DEFINED")
    exit    

debug = ARGUMENTS.get('debug', 0)
if int(debug):
   print "DEBUG IS ON"
   env.Append(CCFLAGS = '-g')

#A.C. need root for rootspy
if os.environ.get('ROOTSYS') is not None:
    env['ROOTSYS'] = os.environ.get('ROOTSYS')
    env.Append(CPPPATH=["$ROOTSYS/include"])
    env.Append(LIBPATH=["$ROOTSYS/lib"])
    
    rootlibs = []
    root_config_libs = []
    root_config_libs = os.popen('$ROOTSYS/bin/root-config --glibs').readline()[:-1].split()
    for l in root_config_libs:
        if l[:2] == '-l':
            rootlibs += [l[2:]]
    env.Append(LIBS = rootlibs)    
else:
    print("ERROR, ROOTSYS NOT DEFINED")
    exit  

#A.C. probably do this better
if (platform.system()=="Darwin"):
	print "We are on MAC"
	env.AppendUnique(LINKFLAGS='-flat_namespace')
	env.AppendUnique(SHLINKFLAGS=['-undefined', 'suppress'])
 


env.Append(CPPPATH=Dir('#/src/external').srcnode().abspath)
env.Append(CPPPATH=Dir('#/src/libraries').srcnode().abspath)
env.Append(CPPPATH=Dir('#/src/plugins').srcnode().abspath)
env.Append(CPPPATH=Dir('#/.').srcnode().abspath)
env.Append(LIBPATH = ['#/lib'])

env.Replace(RPATH=Dir('#/lib').srcnode().abspath)

#This drove me crazy! jlab_software jana was including this, and we need it for sophisticated rootspy features..
#This is basically related to loading symbols in shared libraries
env.Append(LINKFLAGS = '-rdynamic')
env.Append(LINKFLAGS = '-fPIC')

Export('env debug mc_enable')

libExt=SConscript('src/external/SConstruct')
lib=SConscript('src/libraries/SConstruct')
progs=SConscript('src/programs/SConstruct')
plugins=SConscript('src/plugins/SConstruct')
#users=SConscript('src/users/SConstruct')










