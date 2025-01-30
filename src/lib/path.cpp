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

#include <gcore/dirmap.h>
#include <gcore/path.h>
#include <gcore/platform.h>
#include <gcore/unicode.h>
#include <iostream>

namespace gcore {

namespace details {

class DirLister {
  public:
    DirLister(PathList &l) : mLst(l) {}
    bool dirItem(const Path &p) {
        mLst.push_back(p);
        return true;
    }

  private:
    DirLister();
    DirLister &operator=(const DirLister &);

  protected:
    PathList &mLst;
};

} // namespace details

Path::Path() : mStdPath() {}

Path::Path(const char *s) : mStdPath(s) {}

Path::Path(const wchar_t *ws) : mStdPath(ws) {}

Path::Path(const String &s) : mStdPath(s.c_str()) {}

Path::Path(const Path &rhs) : mStdPath(rhs.mStdPath) {}

Path::Path(const std::filesystem::path &other) : mStdPath(other) {}

Path::~Path() {}

Path &Path::operator=(const Path &rhs) {
    if (this != &rhs) {
        mStdPath = rhs.mStdPath;
    }
    return *this;
}

Path &Path::operator=(const String &s) { return operator=(s.c_str()); }

Path &Path::operator=(const char *s) {
    mStdPath = s;
    return *this;
}

Path &Path::operator=(const wchar_t *ws) {
    mStdPath = ws;
    return *this;
}

Path &Path::operator+=(const Path &rhs) {
    if (isDir() && !rhs.isAbsolute()) {
        mStdPath /= rhs.mStdPath; // TODO not sure if that is intended behaviour
    }
    return *this;
}

bool Path::operator==(const Path &rhs) const {
    Path p0(*this);
    Path p1(rhs);

    p0.makeAbsolute().normalize();
    p1.makeAbsolute().normalize();

    return p0.mStdPath == p1.mStdPath;
}

Path::operator const String() const { return String(mStdPath.c_str()); }

Path::operator String() { return String(mStdPath.c_str()); }

// can use negative numbers -> index from the end
String Path::operator[](int idx) {
    auto begin = mStdPath.begin();
    auto end = mStdPath.end();
    auto size = std::distance(begin, end);

    if (idx < 0)
        idx += size;

    if (idx >= 0 && idx < size) {
        auto it = begin;
        std::advance(it, idx);
        auto el = (*it).string();
        return String(el);
    }
    static String _sEmpty;
    return _sEmpty;
}

const String Path::operator[](int idx) const {
    auto begin = mStdPath.begin();
    auto end = mStdPath.end();
    auto size = std::distance(begin, end);

    if (idx < 0)
        idx += size;

    if (idx >= 0 && idx < size) {
        auto it = begin;
        std::advance(it, idx);
        auto el = (*it).string();
        return String(el);
    }
    static String _sEmpty;
    return _sEmpty;
}

String Path::pop() {
    String rv;
    if (mStdPath.has_parent_path()) {
        rv = mStdPath.filename();
        mStdPath = mStdPath.parent_path();
    }
    return rv;
}

Path &Path::push(const String &s) {
    mStdPath.append(s.c_str());
    return *this;
}

bool Path::isAbsolute() const { return mStdPath.is_absolute(); }

// if path is relative, prepend current directory
Path &Path::makeAbsolute() {
    if (mStdPath.empty()) {
        mStdPath = std::filesystem::current_path();
    } else if (!isAbsolute()) { 
        mStdPath = std::filesystem::absolute(mStdPath);
    }
    return *this;
}

// remove any . or .. and make absolute if necessary
Path &Path::normalize() {
    mStdPath = mStdPath.lexically_normal();
    return *this;
}

String Path::basename() const {
    return String(mStdPath.filename()); // Not sure if should return directory name or not
}

String Path::dirname(char sep) const {
    auto parent = mStdPath.parent_path().string();
    std::replace(parent.begin(), parent.end(), '/', sep);
    std::replace(parent.begin(), parent.end(), '\\', sep);
    return String(parent);
}

String Path::fullname(char sep) const {
    auto path = mStdPath.string();
    std::replace(path.begin(), path.end(), '/', sep);
    std::replace(path.begin(), path.end(), '\\', sep);
    return String(path);
}

Date Path::lastModification() const {
    Date lm;
    std::error_code ec;
    auto ftime = std::filesystem::last_write_time(mStdPath, ec);
    if (!ec) {
        auto epoch_duration = ftime.time_since_epoch();
        auto seconds_since_epoch =
            std::chrono::duration_cast<std::chrono::seconds>(epoch_duration).count();
        lm.set(seconds_since_epoch);
    } else {
        lm.set(0);
    }
    return lm;
}

bool Path::isDir() const { return std::filesystem::is_directory(mStdPath); }

bool Path::isFile() const {
    // Should it return true for symlinks/block devices/... ?
    return std::filesystem::is_regular_file(mStdPath);
}

bool Path::exists() const { return std::filesystem::exists(mStdPath); }

// file extension without .
String Path::extension() const { 
    auto ext = mStdPath.extension().string();
    if (!ext.empty() && ext[0] == '.') {
        return String(ext.substr(1)); // Remove the leading dot
    }
    return String(ext); 
}

bool Path::checkExtension(const String &ext) const {
    String pext = extension();
    return (pext.casecompare(ext) == 0);
}

bool Path::setExtension(const String &ext) {
    if (!mStdPath.has_filename()) {
        return false;
    }

    mStdPath.replace_extension(ext.c_str());
    return true;
}

size_t Path::fileSize() const {
    if (isFile()) {
        return std::filesystem::file_size(mStdPath);
    }
    return 0;
}

Status Path::createDir(bool recursive) const {
    if (exists()) {
        return Status(true);
    }
    if (mStdPath.empty()) {
        return Status(false, "gcore::Path::createDir: Empty directory name.");
    }
    std::error_code ec;
    if (recursive) {
        std::filesystem::create_directories(mStdPath, ec);
    } else {
        std::filesystem::create_directory(mStdPath, ec);
    }
    return Status(ec);
}

void Path::forEach(ForEachFunc callback, bool recurse, unsigned short flags) const {
    if (recurse)
        forEach<std::filesystem::recursive_directory_iterator>(callback, flags);
    else
        forEach<std::filesystem::directory_iterator>(callback, flags);
}

template <typename Iterator> void Path::forEach(ForEachFunc callback, unsigned short flags) const {
    if (!isDir() || callback == 0) {
        return;
    }
    Path path(*this);
    auto target_path = mStdPath;
    if (target_path.empty())
        target_path = std::filesystem::current_path();

    for (auto const &p : Iterator{target_path}) {
#ifdef _WIN32
        DWORD attributes = GetFileAttributesW(p.path().c_str());
        if (((flags & FE_HIDDEN) == 0) && ((attributes & FILE_ATTRIBUTE_HIDDEN) != 0)) {
            continue;
        }
#else
        String fname(p.path().filename());
        if ((flags & FE_HIDDEN) == 0 && fname.startswith(".")) {
            continue;
        }
#endif
        if (p.is_directory() && ((flags & FE_DIRECTORY) == 0))
            continue;
        else if (!p.is_directory() && ((flags & FE_FILE) == 0))
            continue;

        if (!callback(Path(p.path())))
            break;
    }
}

size_t Path::listDir(PathList &l, bool recurse, unsigned short flags) const {
    ForEachFunc func;
    details::DirLister dl(l);
    Bind(&dl, &details::DirLister::dirItem, func);
    l.clear();
    forEach(func, recurse, flags);
    return l.size();
}

FILE *Path::open(const char *mode) const {
#ifdef _WIN32
    std::wstring wmode;
    // supposes mode and ASCII string, thus UTF-8 (should be)
    DecodeUTF8(mode, wmode);
    return _wfopen(mStdPath.c_str(), wmode.c_str());
#else
    return fopen(mStdPath.c_str(), mode);
#endif
}

bool Path::open(std::ifstream &inf, std::ios::openmode mode) const {
    inf.open(mStdPath, std::ios::in | mode);
    return inf.is_open();
}

bool Path::open(std::ofstream &outf, std::ios::openmode mode) const {
    outf.open(mStdPath);
    return outf.is_open();
}

Path Path::CurrentDir() { return Path(std::filesystem::current_path()); }

// ---

size_t MMap::PageSize() {
#ifdef _WIN32
    SYSTEM_INFO si;
    ZeroMemory(&si, sizeof(si));
    GetSystemInfo(&si);
    return size_t(si.dwAllocationGranularity);
#else
    return sysconf(_SC_PAGESIZE);
#endif
}

MMap::MMap()
    : mFlags(0), mOffset(0), mSize(0), mMapOffset(0), mMapSize(0), mPtr(0)
#ifdef _WIN32
      ,
      mFD(INVALID_HANDLE_VALUE), mMH(NULL)
#else
      ,
      mFD(-1)
#endif
{
}

MMap::MMap(const Path &path, unsigned char flags, size_t offset, size_t size)
    : mFlags(0), mOffset(0), mSize(0), mMapOffset(0), mMapSize(0), mPtr(0)
#ifdef _WIN32
      ,
      mFD(INVALID_HANDLE_VALUE), mMH(NULL)
#else
      ,
      mFD(-1)
#endif
{
    open(path, flags, offset, size);
}

MMap::~MMap() { close(); }

size_t MMap::size() const { return ((mFlags & READ_ONLY) != 0 ? mSize : mMapSize); }

unsigned char *MMap::data() { return mPtr + (mOffset - mMapOffset); }

const unsigned char *MMap::data() const { return mPtr + (mOffset - mMapOffset); }

bool MMap::valid() const { return (mPtr != 0); }

Status MMap::open(const Path &path, unsigned char flags, size_t offset, size_t size) {
    close();

    if (!path.isFile()) {
        return Status(false, "gcore::MMap::open: Invalid file path.");
    }

    if (size == 0) {
        if ((flags & READ_ONLY) != 0 && path.fileSize() == 0) {
            return Status(false,
                          "gcore::MMap::open: 'size' argument cannot be zero when file is empty.");
        }
    }

#ifdef _WIN32

    DWORD access = GENERIC_READ;
    DWORD sharemode = FILE_SHARE_READ;
    DWORD hint = FILE_ATTRIBUTE_NORMAL;

    if ((flags & READ_ONLY) == 0) {
        access = access | GENERIC_WRITE;
        sharemode = sharemode | FILE_SHARE_WRITE;
    }

    if ((flags & RANDOM_ACCESS) != 0) {
        if ((flags & SEQUENTIAL_ACCESS) == 0) {
            hint = FILE_FLAG_RANDOM_ACCESS;
        }
    } else if ((flags & SEQUENTIAL_ACCESS) != 0) {
        hint = FILE_FLAG_SEQUENTIAL_SCAN;
    }

    HANDLE fd = CreateFileW(path.internalName().c_str(), access, sharemode, NULL, OPEN_EXISTING,
                            hint, NULL);

    if (fd == INVALID_HANDLE_VALUE) {
        return Status(false, std_errno(), "gcore::MMap::open");
    }

    mFD = fd;
    mPath = path;
    mFlags = flags;

#else

    int oflags = ((flags & READ_ONLY) == 0 ? O_RDONLY : O_RDWR);

    int fd = ::open(path.internalName().c_str(), oflags);

    if (fd == -1) {
        return Status(false, std_errno(), "gcore::MMap::open");
    }

    mFD = fd;
    mPath = path;
    mFlags = flags;

#endif

    return remap(offset, size);
}

Status MMap::remap(size_t offset, size_t size) {
    if (!mPath.isFile()) {
        return Status(false, "gcore::MMap::remap: Invalid file path.");
    }

    if (mPtr) {
#ifdef _WIN32
        if (!UnmapViewOfFile(mPtr))
#else
        if (munmap(mPtr, mMapSize) != 0)
#endif
        {
            return Status(false, std_errno(), "gcore::MMap::remap");
        }

        mPtr = 0;
        mOffset = 0;
        mSize = 0;
        mMapOffset = 0;
        mMapSize = 0;
    }

    size_t ps = PageSize();
    size_t fs = mPath.fileSize();

    if (size == 0) {
        size = fs;
    }

    if ((mFlags & READ_ONLY) != 0) {
        // if (size == 0 && fs == 0)
        if (size == 0) {
            return Status(false,
                          "gcore::MMap::remap: 'size' argument cannot be zero when file is empty.");
        }
        if (offset > fs) {
            return Status(false, "gcore::MMap::remap: 'offset' argument pointing after file end.");
        }
    }

    // round down offset
    size_t moffset = ps * (offset / ps);

    // add remaining bytes to required size
    // size_t msize = (size == 0 ? fs : size) + (offset % ps);
    size_t msize = size + (offset % ps);

    // round up size to page size
    msize = ps * ((msize / ps) + (msize % ps ? 1 : 0));

#ifdef _WIN32

    // On windows, in readonly mode, msize must not go beyond file size
    if ((mFlags & READ_ONLY) != 0) {
        if (msize > fs) {
            msize = fs;
        }
    }

    if (mMH == NULL || msize > mMapSize) {
        DWORD prot = ((mFlags & READ_ONLY) != 0 ? PAGE_READONLY : PAGE_READWRITE);
        DWORD hsz = DWORD(msize >> 32);
        DWORD lsz = DWORD(msize & 0xFFFFFFFF);

        if (mMH != NULL) {
            CloseHandle(mMH);
            mMH = NULL;
        }

        HANDLE mh = CreateFileMapping(mFD, NULL, prot, hsz, lsz, NULL);

        if (mh == NULL) {
            return Status(false, std_errno(), "gcore::MMap::open");
        } else {
            mMH = mh;
        }
    }

    DWORD prot = ((mFlags & READ_ONLY) != 0 ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS);
    DWORD hoff = DWORD(moffset >> 32);
    DWORD loff = DWORD(moffset & 0xFFFFFFFF);

    mPtr = (unsigned char *)MapViewOfFile(mMH, prot, hoff, loff, msize);

    if (mPtr == NULL) {
        mPtr = 0;
        return Status(false, std_errno(), "gcore::MMap::remap");
    }

#else

    int prot = PROT_READ;
    int flags = MAP_FILE | MAP_SHARED;

    if ((mFlags & READ_ONLY) == 0) {
        prot = prot | PROT_WRITE;
    }

    mPtr = (unsigned char *)mmap(NULL, msize, prot, flags, mFD, moffset);

    if ((void *)mPtr == MAP_FAILED) {
        mPtr = 0;
        return Status(false, std_errno(), "gcore::MMap::remap");
    }

    int hint = MADV_NORMAL;

    if ((mFlags & RANDOM_ACCESS) != 0) {
        if ((mFlags & SEQUENTIAL_ACCESS) == 0) {
            hint = MADV_RANDOM;
        }
    } else if ((mFlags & SEQUENTIAL_ACCESS) != 0) {
        hint = MADV_SEQUENTIAL;
    }

    madvise(mPtr, msize, hint);

#endif

    mMapOffset = moffset;
    mMapSize = msize;

    mOffset = offset;
    mSize = size;

    // In read-only mode, adjust mSize to actual file size
    if ((mFlags & READ_ONLY) != 0) {
        if ((mOffset + mSize) > fs) {
            mSize = fs - mOffset;
        }
    }

#ifdef _DEBUG
    std::cout << "Required: offset=" << mOffset << ", size=" << mSize << std::endl;
    std::cout << "Mapped: offset=" << mMapOffset << ", size=" << mMapSize << std::endl;
#endif

    return Status(true);
}

Status MMap::sync(bool block) {
    if (mPtr) {
#ifdef _WIN32
        if (!FlushViewOfFile(mPtr, mMapSize))
#else
        // Other flags? MS_INVALIDATE
        if (msync(mPtr, mMapSize, (block ? MS_SYNC : MS_ASYNC)) != 0)
#endif
        {
            return Status(false, std_errno(), "gcore::MMap::sync");
        }
#ifdef _WIN32
        if (block) {
            // FlushViewOfFile is non-blocking
            if (!FlushFileBuffers(mFD)) {
                return Status(false, std_errno(), "gcore::MMap::sync");
            }
        }
#endif
    }
    return Status(true);
}

void MMap::close() {
    sync(true);

    if (mPtr) {
#ifdef _WIN32
        UnmapViewOfFile(mPtr);
#else
        munmap(mPtr, mMapSize);
#endif
    }

#ifdef _WIN32
    if (mMH != NULL) {
        CloseHandle(mMH);
    }
    if (mFD != INVALID_HANDLE_VALUE) {
        CloseHandle(mFD);
    }
#else
    if (mFD != -1) {
        ::close(mFD);
    }
#endif

    mPtr = 0;
    mSize = 0;
    mOffset = 0;
    mMapOffset = 0;
    mMapSize = 0;
    mPath = "";
    mFlags = 0;
#ifdef _WIN32
    mFD = INVALID_HANDLE_VALUE;
    mMH = NULL;
#else
    mFD = -1;
#endif
}

} // namespace gcore
