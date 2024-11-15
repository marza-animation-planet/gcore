#include "pybindgcore.h"

PYBIND11_MODULE(pybindecolo, m) {
    // bindStringconv(m);  # Redundant?
    bindArgparser(m);
    bindDirmap(m);
    bindEnv(m);
    bindLog(m);
    bindMD5(m);
    bindPath(m);
    bindPerflog(m);
    bindPlist(m);
    bindXML(m);
    bindPipe(m);
}