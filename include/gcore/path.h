/*
MIT License

Copyright (c) 2009 Gaetan Guidet

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

#ifndef __gcore_path_h_
#define __gcore_path_h_

#include <filesystem>
#include <gcore/date.h>
#include <gcore/functor.h>
#include <gcore/list.h>
#include <gcore/platform.h>
#include <gcore/status.h>
#include <gcore/string.h>
#include <system_error>

namespace gcore {
class GCORE_API Path {
  public:
    static Path CurrentDir();

    typedef Functor1wR<bool, const Path &> ForEachFunc;

    enum ForEachTarget {
        FE_FILE = 0x01,
        FE_DIRECTORY = 0x02,
        FE_HIDDEN = 0x04, // file/dir starting with a .
        FE_ALL = FE_FILE | FE_DIRECTORY | FE_HIDDEN
    };

    Path();
    Path(const char *s);
    Path(const wchar_t *ws);
    Path(const String &s);
    Path(const Path &rhs);
    Path(const std::filesystem::path &other);
    ~Path();

    Path &operator=(const Path &rhs);
    Path &operator=(const String &s);
    Path &operator=(const char *s);
    Path &operator=(const wchar_t *ws);

    Path &operator+=(const Path &rhs);

    bool operator==(const Path &rhs) const;
    bool operator!=(const Path &rhs) const;

    // those will use '/'
    operator const String() const;
    operator String();

    // can use negative numbers -> index from the end
    int depth() const;
    String operator[](int idx);
    const String operator[](int idx) const;

    bool isAbsolute() const;

    // if path is relative, prepend current directory
    // but keeps . and ..
    Path &makeAbsolute();

    // remove any . or .. and make absolute if necessary
    Path &normalize();

    String basename() const;
    String dirname(char sep = '/') const;
    String fullname(char sep = '/') const;
    // file extension without .
    String extension() const;
    bool checkExtension(const String &ext) const;
    bool setExtension(const String &ext);

    bool exists() const;
    bool isDir() const;
    bool isFile() const;

    size_t fileSize() const;
    Date lastModification() const;

    Status createDir(bool recursive) const;

    // For files:
    //   - if 'to' doesn't exist a file of that name will be created
    //   - if 'to' exists and is a directory, a file with the same basename as this will be created
    //   in 'to'
    //   - 'recursive' doesn't have any effect
    Status copy(const Path &to, bool recursive, bool createMissingDirs, bool overwrite) const;
    Status remove(bool recursive);

    // Status copyStats(const Path &to) const;

    FILE *open(const char *mode) const;
    bool open(std::ifstream &inf, std::ios::openmode mode = std::ios::in) const;
    bool open(std::ofstream &outf, std::ios::openmode mode = std::ios::out) const;

    // flags is a bit wise combination of constants defined in ForEachTarget enum
    template <typename Iterator>
    void forEach(ForEachFunc callback, unsigned short flags) const;
    void forEach(ForEachFunc cb, bool recurse = false, unsigned short flags = FE_ALL) const;
    size_t listDir(List<Path> &l, bool recurse = false, unsigned short flags = FE_ALL) const;

    String pop();
    Path &push(const String &s);

#ifdef _WIN32
    const std::wstring internalName() const;
#else
    const std::string internalName() const;
#endif

  protected:
    // FIXME
    // StringList mPaths;
    std::filesystem::path mStdPath;
};

class GCORE_API MMap {
  public:
    static size_t PageSize();

    enum Flags { READ_ONLY = 0x01, RANDOM_ACCESS = 0x02, SEQUENTIAL_ACCESS = 0x04 };

  public:
    MMap();
    MMap(const Path &path, unsigned char flags = READ_ONLY, size_t offset = 0, size_t size = 0);
    ~MMap();

    bool valid() const;
    Status open(const Path &path, unsigned char flags = READ_ONLY, size_t offset = 0,
                size_t size = 0);
    Status remap(size_t offset, size_t size);
    Status sync(bool block);
    // lock/unlock
    void close();

    size_t size() const;
    unsigned char *data();
    const unsigned char *data() const;
    inline unsigned char at(size_t i) const { return data()[i]; }
    inline unsigned char &at(size_t i) { return data()[i]; }

    inline unsigned char operator[](size_t i) const { return data()[i]; }
    inline unsigned char &operator[](size_t i) { return data()[i]; }

  private:
    MMap(const MMap &rhs);
    MMap &operator=(const MMap &rhs);

  private:
    Path mPath;
    unsigned char mFlags;
    size_t mOffset;
    size_t mSize;
    size_t mMapOffset;
    size_t mMapSize;
    unsigned char *mPtr;
#ifdef _WIN32
    HANDLE mFD;
    HANDLE mMH;
#else
    int mFD;
#endif
};

// ---

inline bool Path::operator!=(const Path &rhs) const { return !operator==(rhs); }

// FIXME
// inline int Path::depth() const { return int(mPaths.size()); }

inline Status Path::copy(const Path &to, bool recursive, bool createMissingDirs,
                         bool overwrite) const {
    std::filesystem::copy_options options;
    if (recursive)
        options |= std::filesystem::copy_options::recursive;
    if (overwrite)
        options |= std::filesystem::copy_options::overwrite_existing;
    if (createMissingDirs) {
        if (isDir())
            std::filesystem::create_directories(mStdPath);
        else
            std::filesystem::create_directories(mStdPath.parent_path());
    }
    std::error_code ec;
    std::filesystem::copy(mStdPath, to.mStdPath, options, ec);
    return Status(ec);
}

inline Status Path::remove(bool recursive) {
    if (!isDir() && !isFile()) {
        return Status(false, "gcore::Path::remove: Invalid path '%s'.", mStdPath.c_str());
    }
    std::error_code ec;
    if (recursive) {
        std::filesystem::remove_all(mStdPath, ec);
    } else {
        std::filesystem::remove(mStdPath, ec);
    }
    return Status(ec);
}

#ifdef _WIN32
inline const std::wstring Path::internalName() const {
    return mStdPath.wstring();
}
#else
inline const std::string Path::internalName() const {
    return mStdPath.string();
}
#endif

inline Path operator+(const Path &p0, const Path &p1) {
    Path rv(p0);
    rv += p1;
    return rv;
}

inline std::ostream &operator<<(std::ostream &os, const Path &p) {
    os << p.fullname();
    return os;
}

typedef List<Path> PathList;

} // namespace gcore

#endif
