/*
MIT License

Copyright (c) 2010 Gaetan Guidet

This file is part of gcore.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __gcorepy_pathenumerator_h__
#define __gcorepy_pathenumerator_h__

#include <Python.h>
#include <gcore/path.h>

class PathEnumerator
{
public:
   
   PathEnumerator();
   PathEnumerator(PyObject *func);
   PathEnumerator(PathEnumerator &rhs);
   ~PathEnumerator();
   
   PathEnumerator& operator=(const PathEnumerator &rhs);
   
   void setPyFunc(PyObject *func);

   bool visit(const gcore::Path &path);
   void apply(const gcore::Path &path, bool recurse=false, unsigned short flags=gcore::Path::FE_ALL);
   
private:
   
   PyObject *mPyFunc;
   PyObject *mPyPath;
   PyObject *mPyArgs;
   gcore::Path::ForEachFunc mFunctor;
};

#endif
