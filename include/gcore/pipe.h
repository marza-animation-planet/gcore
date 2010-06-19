/*

Copyright (C) 2009  Gaetan Guidet

This file is part of gcore.

gcore is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at
your option) any later version.

gcore is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
USA.

*/

#ifndef __gcore_pipe_h_
#define __gcore_pipe_h_

#include <gcore/config.h>

namespace gcore {

#ifdef _WIN32
  
  typedef void* PipeID;
  const PipeID INVALID_PIPE = 0;
  
  inline bool IsValidPipeID(PipeID pi) {
    return (pi != 0);
  }
  
#else
  
  typedef int PipeID;
  const PipeID INVALID_PIPE = -1;
  
  inline bool IsValidPipeID(PipeID pi) {
    return (pi > 0);
  }
  
#endif
  
  class GCORE_API Pipe {
    
    public:
    
      Pipe();
      Pipe(const Pipe &rhs);
      virtual ~Pipe();
      
      bool canRead() const;
      bool canWrite() const;
      void close();
      void create();
      void closeRead();
      void closeWrite();
      int read(std::string &str) const;
      int write(const std::string &str) const;
      PipeID readId() const;
      PipeID writeId() const;
      
      Pipe& operator=(const Pipe &rhs);
      
    private:
    
      PipeID mDesc[2];
  };
}

#endif

