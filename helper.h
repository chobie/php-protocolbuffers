static inline uint32_t zigzag_encode32(int32_t n) {
  // Note:  the right-shift must be arithmetic
  return (n << 1) ^ (n >> 31);
}

static inline int32_t zigzag_decode32(uint32_t n) {
  return (n >> 1) ^ - (int32_t)(n & 1);
}

static inline uint64_t zigzag_encode64(int64_t n) {
  // Note:  the right-shift must be arithmetic
  return (n << 1) ^ (n >> 63);
}

static inline int64_t zigzag_decode64(uint64_t n) {
  return (n >> 1) ^ - (int64_t)(n & 1);
}
