#!/usr/bin/python
## scons script for building the kde test applications
## Thomas Nagy, 2004, 2005

## This file can be reused freely for any project (see COPYING)

## It contains some code necessary for building kde programs
## The kde detection is located in kde.py (have a look!)


"""
scons files are python scripts, comments begin by a "#" symbol
or are enclosed between sequences of triple quotes, so 
this is a comment :)

There is a lot of documentation and comments, but you can
remove them when you convert your program
"""

"""
----------------------------------
How to enjoy bksys full and plenty
(and forget about the autohell ? :)
-----------------------------------

The program scons is usually launched as "scons"
When it is not intalled globally, one can run
"python scons.py" instead (ie : to use the local scons 
that comes with bksys - try ./unpack_local_scons.sh)

To compile the project, you will then only need to launch
scons on the top-level directory, the scripts find and
cache the proper environment automatically :
-> scons
(or python scons.py)

To clean the project
-> scons -c
(or python scons.py -c)

To install the project
-> scons install
(or python scons.py scons install)

To uninstall the project
-> scons -c install

To compile while being in a subdirectory
-> cd src; scons -u

To (re)configure the project and give particular arguments, use ie :
-> scons configure debug=1
-> scons configure prefix=/tmp/ita debug=full extraincludes=/usr/local/include:/tmp/include prefix=/usr/local
or -> python scons.py configure debug=1
etc ..
The variables are saved automatically after the first run
(look at kde.cache.py, ..)

Here is a quick list of options used (look at generic.py and kde.py) :
prefix
exec_prefix
datadir
libdir
kdeincludes
qtincludes
kdelibs
qtlibs
extraincludes (a list of paths separated by ':')

"""

###########################################
## Common section, for loading the tools

## Load the builders in config
env = Environment(TARGS=COMMAND_LINE_TARGETS, ARGS=ARGUMENTS, tools=['default', 'generic', 'kde'], toolpath=['./'])
#env = Environment(TARGS=COMMAND_LINE_TARGETS, ARGS=ARGUMENTS, tools=['default', 'generic', 'kde', 'cg'], toolpath=['./'])
#env = Environment(TARGS=COMMAND_LINE_TARGETS, ARGS=ARGUMENTS, tools=['default', 'generic', 'kde', 'sound'], toolpath=['./'])
#env = Environment(TARGS=COMMAND_LINE_TARGETS, ARGS=ARGUMENTS, tools=['default', 'generic', 'kde', 'libxml'], toolpath=['./'])

## the configuration should be done by now, quit
if 'configure' in COMMAND_LINE_TARGETS:
	env.Exit(0)

"""
Overview of the module system :

Each module (kde.py, generic.py, sound.py..) tries to load a stored 
configuration when run. If the stored configuration does not exist
or if 'configure' is given on the command line (scons configure), 
the module launches the verifications and detectioins and stores 
the results. Modules also call exit when the detection fail.

For example, kde.py stores its config into kde.cache.py

This has several advantages for both developers and users :
  - Users do not have to run ./configure to compile
  - The build is insensitive to environment changes
  - The cache maintains the objects so the config can be changed often
  - Each module adds its own help via env.Help("message")
"""

## Use the variables available in the environment - unsafe, but moc, meinproc need it :-/
import os
env.AppendUnique( ENV = os.environ )

## The target make dist requires the python module shutil which is in 2.3
env.EnsurePythonVersion(2, 3)

## Bksys requires scons 0.96
env.EnsureSConsVersion(0, 96)

"""
Explanation of the 'env = Environment...' line :
* the command line arguments and targets are stored in env['TARGS'] and env['ARGS'] for use by the tools
* the part 'tools=['default', 'generic ..' detect and load the necessary functions for doing the things
* the part "toolpath=['./']" tells that the tools can be found in the current directory (generic.py, kde.py ..)
"""

"""
To load more configuration modules one should only have to add the appropriate tool
ie: to detect alsa and add the proper cflags, ldflags .. 
    a file alsa.py file will be needed, and one should then use :
    env = Environment(TARGS=COMMAND_LINE_TARGETS, ARGS=ARGUMENTS, tools=['default', 'generic', 'kde', 'alsa'], toolpath=['./'])

You can also load environments that are targetted to different platforms
ie: if os.sys.platform = "darwin":
	env = Environment(...
    elsif os.sys.platform = "linux":
	env = Environment(...

"""

## Setup the cache directory - this avoids recompiling the same files over and over again
## this is very handy when working with cvs
env.CacheDir('cache')

## Avoid spreading .sconsign files everywhere - keep this line
env.SConsignFile('scons_signatures')

## If you need more libs and they rely on pkg-config
## ie: add support for GTK (source: the scons wiki on www.scons.org)
# env.ParseConfig('pkg-config --cflags --libs gtk+-2.0')

"""
This tell scons that there are no rcs or sccs files - this trick 
can speed up things a bit when having lots of #include
in the source code and for network file systems
"""
env.SourceCode(".", None)

dirs = [ '.', 'test1-kconfigxt', 'test2-kdcop', 
'test3-kparts', 'test4-libtool', 'test5-kioslave',
'test8-kate']

for dir in dirs:
	env.SourceCode(dir, None)

## If we had only one program (named kvigor) to build, 
## we could add before exporting the env (some kde 
## helpers in kde.py need it) :
# env['APPNAME'] = 'kvigor'

## Use this define if you are using the kde translation scheme (.po files)
#env.Append( CPPFLAGS = ['-DQT_NO_TRANSLATION'] )

## Uncomment the following if you need threading support threading
#env.Append( CPPFLAGS = ['-DQT_THREAD_SUPPORT', '-D_REENTRANT'] )
#if os.uname()[0] == "FreeBSD":
#	env.Append(LINKFLAGS=["-pthread"])

## To use kdDebug(intvalue)<<"some trace"<<endl; you need to define -DDEBUG - it is done
## in generic.py automatically when you do scons configure debug=1

## There are also many other defines :
### -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -DQT_CLEAN_NAMESPACE -DQT_NO_ASCII_CAST -DQT_NO_STL -DQT_NO_COMPAT -DQT_NO_TRANSLATION
# where are they documented ? if you know, mail tnagyemail-mail@yahoo@fr please

## Important : export the environment so that SConscript files can the 
## configuration and builders in it
Export("env")


####################################################
## Process the SConscript files to build the targets
## the sconscript files are comparable to Makefile.am_s
## except that no makefile is generated here :)

env.SConscript("src/SConscript")
env.SConscript("src/pics/SConscript")

## demonstration of the error handling
#env.SConscript("error_handling/SConscript")

## It is also possible to give the scripts in a row :
#buildscripts = [
#'test1-kconfigxt/SConscript',
#'test2-kdcop/SConscript',
#'test3-kparts/SConscript',
##'test4-libtool/SConscript', # <- if you really want libtool
#'test5-kioslave/SConscript',
#'test6-mocfiesta/SConscript',
#'test7-kmdi/SConscript',
#'test8-kate/SConscript',
#'test8-kate/otherversion/SConscript',
#'doc/en/SConscript',
#'po/SConscript',
#]
#env.SConscript(buildscripts)


####################################################
## Quick helper to distribute the program

"""
'scons dist' creates a tarball named bksys-version.tar.bz2 
containing the source code - this is handy
do not forget to remove the object files by scons -c
"""

### to make a tarball of your masterpiece
if 'dist' in COMMAND_LINE_TARGETS:
        APPNAME = 'knetwalk'
        VERSION = os.popen("cat VERSION").read().rstrip()
        FOLDER  = APPNAME+'-'+VERSION
        ARCHIVE = FOLDER+'.tar.bz2'

        GREEN  ="\033[92m"
        NORMAL ="\033[0m"

        import shutil
        import glob

        ## check if the temporary directory already exists
        if os.path.isdir(FOLDER):
                shutil.rmtree(FOLDER)

        ## create a temporary directory
        startdir = os.getcwd()
        shutil.copytree(startdir, FOLDER)

        ## remove the unnecessary files
        os.popen("find "+FOLDER+" -name \"{arch}\" | xargs rm -rf")
        os.popen("find "+FOLDER+" -name \".arch-ids\" | xargs rm -rf")
        os.popen("find "+FOLDER+" -name \".arch-inventory\" | xargs rm -f")
        os.popen("find "+FOLDER+" -name \"sconsign*\" | xargs rm -f")
        os.popen("find "+FOLDER+" -name \"*cache*\" | xargs rm -rf")
        os.popen("find "+FOLDER+" -name \"kdiss*-data\" | xargs rm -rf")
        os.popen("find "+FOLDER+" -name \"*.pyc\" | xargs rm -f")
        os.popen("rm -f "+FOLDER+"/config.py*")

        ## make the tarball
        print GREEN+"Writing archive "+ARCHIVE+NORMAL
        os.popen("tar cjf "+ARCHIVE+" "+FOLDER)

        ## remove the temporary directory
        if os.path.isdir(FOLDER):
                shutil.rmtree(FOLDER)

        env.Default(None)
        env.Exit(0)

