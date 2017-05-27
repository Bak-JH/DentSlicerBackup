#include "support.h"

Support::Support(int type){
    Support::type = type;
}

void Support::generate(Slices& slices){
    switch (type)
    {
    case generalsupport:
        break;
    case kbranchsupport:
        generateKbranch(slices);
        printf ("support generation done\n");
        break;
    }
}
