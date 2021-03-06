#ifndef ITER_H__
#define ITER_H__

#include "datatype.h"

namespace bitsy_python {
extern Variable IterCreate(const Variable& var);
extern bool IterForLoopIter(const Variable& iter, Variable *elem);
extern bool IterForLoop(Variable::CustomType iter, uint32_t ind, Variable *elem);
}

#endif /* ITER_H__ */
