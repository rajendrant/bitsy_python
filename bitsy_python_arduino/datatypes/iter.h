#ifndef ITER_H__
#define ITER_H__

#include "datatype.h"

namespace bitsy_python {
extern Variable IterCreate(const Variable& var);
extern bool IterForLoop(const Variable& iter, Variable *elem);
}

#endif /* ITER_H__ */
