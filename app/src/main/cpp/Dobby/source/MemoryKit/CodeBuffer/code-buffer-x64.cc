#include "common/macros/platform_macro.h"
#if defined(TARGET_ARCH_X64)

#include "./code-buffer-x64.h"

void CodeBuffer::Emit32(int32_t data) {
  ensureCapacity(getSize() + sizeof(int32_t));
  *reinterpret_cast<int32_t *>(getCursor()) = data;
  buffer_cursor_ += sizeof(int32_t);
  return;
}

void CodeBuffer::Emit64(int64_t data) {
  ensureCapacity(getSize() + sizeof(int64_t));
  *reinterpret_cast<int64_t *>(getCursor()) = data;
  buffer_cursor_ += sizeof(int64_t);
  return;
}

void CodeBuffer::FixBindLabel(int offset, int32_t disp) {
  *reinterpret_cast<uint32_t *>(buffer_ + offset) = disp;
  return;
}

#endif