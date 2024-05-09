# MIT License
#
# Copyright (c) 2010 Gaetan Guidet
#
# This file is part of gcore.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# cython: c_string_type=unicode, c_string_encoding=utf8


cimport gcore
from cython.operator import dereference as deref
import sys

LOG_ERROR = gcore.LOG_ERROR
LOG_WARNING = gcore.LOG_WARNING
LOG_DEBUG = gcore.LOG_DEBUG
LOG_INFO = gcore.LOG_INFO
LOG_ALL = gcore.LOG_ALL

cdef class Log:
   cdef gcore.Log *_cobj
   cdef gcore.LogOutputFunc *_outfunc
   cdef bint _own
   
   @classmethod
   def SetLevelMask(cls, int mask):
      gcore.SetLevelMask(mask)
   
   @classmethod
   def LevelMask(cls):
      return gcore.LevelMask()
   
   @classmethod
   def SetIndentLevel(cls, int l):
      gcore.SetIndentLevel(l)
   
   @classmethod
   def IndentLevel(cls):
      return gcore.IndentLevel()
   
   @classmethod
   def Indent(cls):
      gcore.Indent()
   
   @classmethod
   def UnIndent(cls):
      gcore.UnIndent()
   
   @classmethod
   def SetIndentWidth(cls, int w):
      gcore.SetIndentWidth(w)
   
   @classmethod
   def IndentWidth(cls):
      return gcore.IndentWidth()
   
   @classmethod
   def SetColorOutput(cls, bint onoff):
      gcore.SetColorOutput(onoff)
   
   @classmethod
   def ColorOutput(cls):
      return gcore.ColorOutput()
   
   @classmethod
   def SetShowTimeStamps(cls, bint onoff):
      gcore.SetShowTimeStamps(onoff)
   
   @classmethod
   def ShowTimeStamps(cls):
      return gcore.ShowTimeStamps()
   
   @classmethod
   def PrintError(cls, char* msg):
      gcore.PrintError(msg)
   
   @classmethod
   def PrintWarning(cls, char* msg):
      gcore.PrintWarning(msg)
   
   @classmethod
   def PrintDebug(cls, char* msg):
      gcore.PrintDebug(msg)
   
   @classmethod
   def PrintInfo(cls, char* msg):
      gcore.PrintInfo(msg)
   
   @classmethod
   def SetOutputFunc(cls, outputFunc):
      gcore.Log_SetOutputFunc(<gcore.PyObject*>outputFunc)
   
   
   def __cinit__(self, *args, **kwargs):
      self._cobj = NULL
      self._own = False
   
   def __init__(self, *args, noalloc=False, **kwargs):
      self._outfunc = new gcore.LogOutputFunc()

      if noalloc:
         self._cobj = NULL
         self._own = False
         return
      
      if len(args) == 0:
         self._cobj = new gcore.Log()
      elif len(args) == 1:
         if isinstance(args[0], Path):
            self._cobj = new gcore.Log(deref((<Path>args[0])._cobj))
         elif isinstance(args[0], Log):
            self._cobj = new gcore.Log(deref((<Log>args[0])._cobj))
         elif isinstance(args[0], str):
            self._cobj = new gcore.Log(gcore.Path(<char*>args[0]))
         else:
            raise Exception("_gcore.Log() invalid argument type %s" % type(str))
      else:
         raise Exception("_gcore.Log() accepts at most 1 argument")
      
      self._own = True
   
   def __dealloc__(self):
      if self._own and self._cobj != NULL:
         del(self._cobj)
         self._cobj = NULL
      del(self._outfunc)
   
   def _printStdout(self, msg):
      sys.stdout.write(msg)
   
   def printError(self, msg):
      self._cobj.printError(<char*?>msg)
   
   def printWarning(self, msg):
      self._cobj.printWarning(<char*?>msg)
   
   def printDebug(self, msg):
      self._cobj.printDebug(<char*?>msg)
   
   def printInfo(self, msg):
      self._cobj.printInfo(<char*?>msg)
   
   @property
   def levelMask(self):
      return self._cobj.levelMask()
      def __set__(self, v): self._cobj.setLevelMask(<unsigned int>v)
   
   @property
   def indentLevel(self):
      return self._cobj.indentLevel()

   @indentLevel.setter
   def indentLevel(self, v): self._cobj.setIndentLevel(<unsigned int>v)
   
   @property
   def indentWidth(self):
      return self._cobj.indentWidth()

   @indentWidth.setter
   def indentWidth(self, v): self._cobj.setIndentWidth(<unsigned int>v)
   
   def indent(self):
      self._cobj.indent()
   
   def unIndent(self):
      self._cobj.unIndent()
   
   @property
   def colorOutput(self):
      return self._cobj.colorOutput()
   
   @colorOutput.setter
   def colorOutput(self, v): self._cobj.setColorOutput(<bint>v)
   
   @property
   def showTimeStamps(self):
      return self._cobj.showTimeStamps()

   @showTimeStamps.setter
   def showTimeStamps(self, v): self._cobj.setShowTimeStamps(<bint>v)
   
   def setOutputFunc(self, func):
      if func is None:
         self._outfunc.setPyFunc(<gcore.PyObject*>self._printStdout)
      else:
         self._outfunc.setPyFunc(<gcore.PyObject*>func)
      self._outfunc.assign(deref(self._cobj))
   
